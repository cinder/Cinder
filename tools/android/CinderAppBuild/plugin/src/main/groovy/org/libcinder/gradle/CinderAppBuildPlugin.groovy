package org.libcinder.gradle;

import org.gradle.api.GradleException;
import org.gradle.api.InvalidUserDataException;
import org.gradle.api.Project;
import org.gradle.api.Plugin;
import org.gradle.api.DefaultTask;
import org.gradle.api.tasks.*;
import org.gradle.internal.reflect.Instantiator;
import java.nio.file.Path;
import java.nio.file.Paths;

/*
import org.gradle.api.file.FileTree;
*/

import org.gradle.api.tasks.incremental.IncrementalTaskInputs;


/** @class CFlags
 *
 */
class CFlags {   
    String name;
    Project project;

    def debug = "";
    def release = "";

    CFlags(String name, Project project) {
        this.name = name;
        this.project = project;
    }
}

/** @class CPPFlags
 *
 */
class CPPFlags {
    String name;
    Project project;

    def debug = "";
    def release = "";

    CPPFlags(String name, Project project) {
        this.name = name;
        this.project = project;
    }
}

/** @class CinderAppBuildPluginExtension
 *
 */
class CinderAppBuildPluginExtension {
    def ndkDir           = "";
    def cinderDir        = "";
    def verbose          = false;
    def moduleName       = "";
    def gles2            = false;
    def srcFiles         = [];
    def srcFilesExclude  = [];
    def srcDirs          = [];
    def includeDirs      = [];
    def ldLibs           = [];
    def libCinder        = [];
    def staticLibs       = [];
    def stl              = "gnustl_static";
    def toolChainVersion = "4.9";
    def archs            = [];

    def disableFormatStringChecks  = false;
}

/** @class CompileNdkTask
 *
 */
class CompileNdkTask extends DefaultTask {
    CinderAppBuildPlugin plugin;

    @InputFiles
    List<File> getInputFiles() {
        if( null == plugin ) {
            throw new GradleException("plugin is null - was it not set in cinderCompileDebugNdk or cinderCompileReleaseNdk?" );
        }

        ArrayList<File> result = new ArrayList<File>();

        // Source files
        plugin.mSourceFilesFullPath.each {
            result.add( new File( it ) ); 
        }

       // Static libs
        plugin.mStaticLibsFullPaths.each {
            result.add( new File( it ) ); 
        }
       
        return result;
    }

    @OutputFile
    File getGeneratedAndroidMk() {
        if( null == plugin ) {
            throw new GradleException("plugin is null - was it not set in cinderCompileDebugNdk or cinderCompileReleaseNdk?" );
        }

        File result = new File( "${plugin.mNdkBuildDir}/Android.mk" );
        return result;
    }

    @TaskAction
    void generate(IncrementalTaskInputs inputs) {

        inputs.outOfDate { change ->
            println "DEP CHANGED: ${change.file}";
        }
    }
}

/** @class CinderAppBuildPlugin
 *
 */
class CinderAppBuildPlugin implements Plugin<Project> {

    static final kValidArchs = ["arm64-v8a", "armeabi", "armeabi-v7a", "mips", "mips64", "x86","x86_64"]

    def mProjectDir   = "";
    def mBuildDir     = "";
    def mBuildType    = "";
    def mNdkBuildDir  = "";

    def mSourceFiles  = [];
    def mIncludeDirs  = [];
    def mSharedLibs   = [];
    def mStaticLibs   = [];
    def mStaticBlocks = [];
    def mArchs        = [];

    def mArchFlagsBlocks = [];
    def mSourceFilesFullPath = []; // Used to for depenedency check
    def mStaticLibsFullPaths = []; // Used to for depenedency check
    def mSourceFilesExcludeFullPath = [];

    void apply(Project project) {
        project.extensions.create("cinder", CinderAppBuildPluginExtension)

        project.cinder.extensions.cFlags = project.container(CFlags) { String name ->
            CFlags archFlags = project.gradle.services.get(Instantiator).newInstance(CFlags, name, project);
            return archFlags;
        }

        project.cinder.extensions.cppFlags = project.container(CPPFlags) { String name ->
            CPPFlags archFlags = project.gradle.services.get(Instantiator).newInstance(CPPFlags, name, project);
            return archFlags;
        }

        this.mProjectDir = project.projectDir;
        this.mBuildDir  = project.buildDir;
        this.mBuildType = 'debug';

        // TASK: cinderGenerateDebugNdkBuild
        project.task('cinderGenerateDebugNdkBuild' ) << {
            def minSdkVersion = project.android.defaultConfig.minSdkVersion.apiLevel;
            if( minSdkVersion < 18 ) {
                project.cinder.gles2 = true;
                if( project.cinder.verbose ) {
                    println "Forcing OpenGL ES 2 for API Level ${minSdkVersion}";
                }
            }

            this.mBuildType = "debug";
            this.mNdkBuildDir = "${this.mBuildDir}/cinder-ndk/${this.mBuildType}";
            this.cinderGenerateNdkBuild(project);  
        }
      
        // TASK: cinderCompileDebugNdk
        project.task('cinderCompileDebugNdk', type: CompileNdkTask, dependsOn: 'cinderGenerateDebugNdkBuild') {
            plugin = this;
            doLast {
                this.cinderCompileNdk(project);
            }
        }

        // TASK: cinderGenerateReleaseNdkBuild
        project.task('cinderGenerateReleaseNdkBuild') << {
            def minSdkVersion = project.android.defaultConfig.minSdkVersion.apiLevel;
            if( minSdkVersion < 18 ) {
                project.cinder.gles2 = true;
                if( project.cinder.verbose ) {
                    println "Forcing OpenGL ES 2 for API Level ${minSdkVersion}";
                }
            }

            this.mBuildType = "release";
            this.mNdkBuildDir = "${this.mBuildDir}/cinder-ndk/${this.mBuildType}"
            this.cinderGenerateNdkBuild(project);
        }  

        // TASK: cinderCompileReleaseNdk
        project.task('cinderCompileReleaseNdk', type: CompileNdkTask, dependsOn: 'cinderGenerateReleaseNdkBuild') {
            plugin = this;
            doLast {
                this.cinderCompileNdk(project);
            }
        }
    }

    void writeAndroidMk(Project project, String filePath) {
        def genDateTime = (new Date()).toString();

        def lines = []
        lines.add("# Generated by CinderAppBuildPlugin: ${genDateTime}")
        lines.add("#")
        lines.add("LOCAL_PATH := \$(call my-dir)")
        lines.add("")           
        lines.add(this.mStaticBlocks.join("\n"))
        lines.add("# ------------------------------------------------------------------------------")
        lines.add("")
        lines.add("include \$(CLEAR_VARS)")
        lines.add("")
        lines.add("# Module Name" )
        lines.add("LOCAL_MODULE := ${project.cinder.moduleName}")
        lines.add("")
        lines.add("# Module Flags")
        lines.add("LOCAL_DISABLE_FORMAT_STRING_CHECKS := " + (project.cinder.disableFormatStringChecks ? "true" : "false" ) );
        lines.add("");
        lines.add("# C++ Source Files" )
        lines.add("LOCAL_SRC_FILES := "+ this.mSourceFiles.join("\n"))
        lines.add("");
        lines.add("LOCAL_CPP_FEATURES := rtti exceptions");
        lines.add("");
        lines.add(this.mArchFlagsBlocks.join("\n"));
        lines.add("")
        lines.add("# Include Directories" )
        lines.add("LOCAL_C_INCLUDES := " + this.mIncludeDirs.join("\n"))
        lines.add("")
        lines.add("# Shared Libraries" )
        lines.add("LOCAL_LDLIBS := " + this.mSharedLibs.join("\n"))
        lines.add("")
        lines.add("# Static Libraries" )
        lines.add("LOCAL_STATIC_LIBRARIES := " + this.mStaticLibs.join("\n"))
        lines.add("")
        lines.add("include \$(BUILD_SHARED_LIBRARY)")
        lines.add("")
        
        def outFile = new File( "${filePath}" )
        outFile.text = lines.join("\n")

        if( project.cinder.verbose && outFile.exists() ) {
            println "Wrote ${filePath}"
        }
    }

    void generateAndroidMk(Project project) {
        // Paths
        def dirPath = "${this.mNdkBuildDir}"
        def filePath = "${dirPath}/Android.mk" 
        def outDir  = new File( "${dirPath}" )
        if( ! outDir.exists() ) {
            outDir.mkdirs()
            if( project.cinder.verbose ) {
                println "Created ${dirPath}"
            }
        }

        // Directory where cpp builds take place
        def cppBuildDir = (new File(filePath)).getParentFile().getCanonicalPath();
       
        // Project archs
        try {
            this.parseArchs( project );                
        }
        catch( e ) {
            throw new GradleException("Build archs parse failed, e=" + e)
        } 

        // Source files 
        try {
            this.parseSourceFiles(project, cppBuildDir)
        } catch( e ) {
            throw new GradleException("Source files parse failed, e=" + e)
        }
            
        // Include dirs
        try {
            this.parseIncludeDirs(project)
        } catch( e ) {
            throw new GradleException("Include dirs parse failed, e=" + e)            
        }

        // Flags
        try {
            this.parseFlags(project)
        } catch( e ) {
            throw new GradleException("C/C++ Flags parse failed, e=" + e)            
        }

        // Shared libs
        try {
            this.parseSharedLibs(project)
        } catch( e ) {
            throw new GradleException("Shared libs parse failed, e=" + e)           
        }

        // Static libs
        try {
            this.parseStaticLibs(project)
        } catch( e ) {
            throw new GradleException("Static libs parse failed, e=" + e)            
        }

        // Write
        this.writeAndroidMk(project, filePath)
    }
    
    void writeApplicationMk(Project project, String filePath) {
        def minSdkVersion = project.android.defaultConfig.minSdkVersion.apiLevel;

        String archsStr = this.mArchs.join(",");
        if( archsStr.endsWith( "," ) ) {
            archsStr = archsStr.substring( 0, archsStr.length() - 1 );
        }

        def lines = []
        lines.add("# Generated by CinderAppBuildPlugin")
        lines.add("#")
        lines.add("")
        lines.add("APP_BUILD_SCRIPT := ${this.mNdkBuildDir}/Android.mk");
        lines.add("APP_PLATFORM := android-${minSdkVersion}");
        lines.add("NDK_OUT := ${this.mNdkBuildDir}/obj");
        lines.add("NDK_LIBS_OUT := ${this.mProjectDir}/src/main/jniLibs");
        lines.add("APP_STL := gnustl_static");
        lines.add("APP_ABI := ${archsStr}");
        lines.add("NDK_TOOLCHAIN_VERSION := ${project.cinder.toolChainVersion}");
        lines.add("APP_CPPFLAGS := ");
        
        def outFile = new File( "${filePath}" );
        outFile.text = lines.join("\n");

        if( project.cinder.verbose && outFile.exists() ) {
            println "Wrote ${filePath}"
        }
    }
    

    void generateApplicationMk(Project project) {
        // Paths
        def dirPath = "${this.mNdkBuildDir}"
        def filePath = "${dirPath}/Application.mk" 
        def outDir  = new File( "${dirPath}" )
        if( ! outDir.exists() ) {
            outDir.mkdirs()
            if( project.cinder.verbose ) {
                println "Created ${dirPath}";
            }
        }

        // Write
        this.writeApplicationMk(project, filePath);
    }

    void cinderGenerateNdkBuild(Project project) {
        this.ndkDirCheck(project)

        this.generateAndroidMk(project);
        //this.generateApplicationMk(project);
    }

    void cinderCompileNdk(Project project) {
        this.ndkDirCheck(project)

        String ndkDir = (new File( project.cinder.ndkDir )).getCanonicalPath(); //project.plugins.findPlugin('com.android.application').getNdkFolder().toString()
        if( project.cinder.verbose ) {
            println "NDK_ROOT: ${ndkDir}";
        }
       
        def ndkBuildCmd  = "${ndkDir}/ndk-build";
        def ndkBuildArgs = [];
        
        def minSdkVersion = project.android.defaultConfig.minSdkVersion.apiLevel;

        String archsStr = this.mArchs.join(",");
        if( archsStr.endsWith( "," ) ) {
            archsStr = archsStr.substring( 0, archsStr.length() - 1 );
        }

        def numCores = Runtime.getRuntime().availableProcessors();
        ndkBuildArgs.add(this.makeNdkArg("--jobs", "${numCores}"));

        if( project.cinder.verbose ) {
            ndkBuildArgs.add(this.makeNdkArg("V", "1"));
        }       

        ndkBuildArgs.add(this.makeNdkArg("NDK_PROJECT_PATH",      "null"));
        ndkBuildArgs.add(this.makeNdkArg("APP_BUILD_SCRIPT",      "${this.mNdkBuildDir}/Android.mk"));
        ndkBuildArgs.add(this.makeNdkArg("APP_PLATFORM",          "android-${minSdkVersion}"));
        ndkBuildArgs.add(this.makeNdkArg("NDK_OUT",               "${this.mNdkBuildDir}/obj"));
        ndkBuildArgs.add(this.makeNdkArg("NDK_LIBS_OUT",          "${this.mProjectDir}/src/main/jniLibs"));
        ndkBuildArgs.add(this.makeNdkArg("APP_STL",               "gnustl_static"));
        ndkBuildArgs.add(this.makeNdkArg("APP_ABI",               "${archsStr}"));
        ndkBuildArgs.add(this.makeNdkArg("NDK_TOOLCHAIN_VERSION", "${project.cinder.toolChainVersion}"));
        ndkBuildArgs.add(this.makeNdkArg("APP_CPPFLAGS",          "-I."));
        ndkBuildArgs.add(this.makeNdkArg("APP_OPTIM",             "${this.mBuildType}"));


        if( project.cinder.verbose ) {
            def execStr = ndkBuildCmd + " " + ndkBuildArgs.join(" ")
            println "EXEC DIR: ${this.mNdkBuildDir}"
            println "EXEC CMD: ${execStr}"
        }

        project.exec {
            workingDir = this.mNdkBuildDir
            executable = ndkBuildCmd
            args = ndkBuildArgs
        }
    }

    void parseArchs(Project project) {
        this.mArchs = []
       
        boolean hasAll = false;
 
        project.cinder.archs.each {
            String tok = it;
            tok = tok.trim();
            if( CinderAppBuildPlugin.kValidArchs.contains( tok ) ) {
                this.mArchs.push( tok );
            }
            
            if( "all" == tok.toLowerCase() ) {
                hasAll = true;
            }
        }

        if( hasAll ) {
            this.mArchs = [];
            CinderAppBuildPlugin.kValidArchs.each {
                this.mArchs.push( it )
            }
        }
    }

    boolean isValidSourceFileExt( String fileName ) {
        String lower = fileName.toLowerCase();
        return lower.endsWith( ".cpp" ) ||
               lower.endsWith( ".cxx" ) ||    
               lower.endsWith( ".cc" )  ||    
               lower.endsWith( ".c" );    
    }

    void addSourceFile( String path, cppBuildDir ) {
        if( isValidSourceFileExt( path ) ) {
            if( ! path.startsWith( "." ) ) {
                if( ! mSourceFilesExcludeFullPath.contains( path ) ) {
                    String relPath = this.relativePath( cppBuildDir, path )
                    this.mSourceFiles.add("\t" + relPath + " \\")
                    this.mSourceFilesFullPath.add( path );
                }
            }
        }
        else {
            println( "IGNORING (unknown source file ext): ${path}" );
        }
    }

    void parseSourceFiles(Project project, cppBuildDir) {
        this.mSourceFiles = [];
        this.mSourceFilesFullPath = [];
        this.mSourceFilesExcludeFullPath = [];

        // Exclude Files
        if( ! project.cinder.srcFilesExclude.isEmpty() ) {
            project.cinder.srcFilesExclude.each {
                def file = new File("${project.projectDir}/" + it)
                String path = file.canonicalPath.toString()
                mSourceFilesExcludeFullPath.add( path )
            }
        }

        // Files
        if(! project.cinder.srcFiles.isEmpty()) {
            this.mSourceFiles.add("\\")
            project.cinder.srcFiles.each {
                def file = new File("${project.projectDir}/" + it)
                String path = file.canonicalPath.toString();
                addSourceFile( path, cppBuildDir );
            }
        }
        
        // Dirs
        if(! project.cinder.srcDirs.isEmpty()) {
            if(this.mSourceFiles.isEmpty()) {
                this.mSourceFiles.add("\\")
            }
            project.cinder.srcDirs.each {
                def dir = new File("${project.projectDir}/" + it)                                       
                dir.eachFile() {
                    String path = it.canonicalPath.toString();
                    addSourceFile( path, cppBuildDir );
                }
            }
        }
    }

    void parseIncludeDirs(Project project) {
        this.mIncludeDirs = []
        if( ! project.cinder.includeDirs.empty ) {
            this.mIncludeDirs.add("\\")
            project.cinder.includeDirs.each {
                // Add prefix if it's a relative path.
                String pathPrefix = it.startsWith( "../" ) ? "${project.projectDir}/" : "";
                String path = (new File( "${pathPrefix}" + it)).canonicalPath.toString()
                this.mIncludeDirs.add("\t" + path + " \\")
            }
        }
    }

    void parseFlags(Project project) {
        this.mArchFlagsBlocks = [];

        def cFlags = [:];
        def cppFlags = [:];

        project.cinder.cFlags.each {
            cFlags[it.name] = it;
        }   

        project.cinder.cppFlags.each {
            cppFlags[it.name] = it;
        }       

        def gles2Flags = "";
        if( project.cinder.gles2 ) {
            gles2Flags = "-DCINDER_GL_ES_2";
        }
        
        def flagGroup = cFlags["all_archs"] ?: null;
        if( flagGroup ) {
            def flags = ("debug" == this.mBuildType) ? flagGroup.debug : flagGroup.release;
            this.mArchFlagsBlocks.add("LOCAL_CFLAGS   := ${flags} ${gles2Flags}");
        }

        flagGroup = cppFlags["all_archs"] ?: null;
        if( flagGroup ) {
            def flags = ("debug" == this.mBuildType) ? flagGroup.debug : flagGroup.release;
            this.mArchFlagsBlocks.add("LOCAL_CPPFLAGS := ${flags} ${gles2Flags}");
        }

        if( this.mArchFlagsBlocks.size() > 1 ) {
            this.mArchFlagsBlocks.add( "" );
        }

        boolean useElse = false;
        CinderAppBuildPlugin.kValidArchs.each {
            def archName = it; 
            def addElse = useElse ? "else " : "";
            this.mArchFlagsBlocks.add("${addElse}ifeq (\$(TARGET_ARCH_ABI),${archName})");
            
            flagGroup = cFlags[archName] ?: null;
            if( flagGroup ) {
                def flags = ("debug" == this.mBuildType) ? flagGroup.debug : flagGroup.release;
                this.mArchFlagsBlocks.add("\tLOCAL_CFLAGS   += ${flags}");
            }

            flagGroup = cppFlags[archName] ?: null;
            if( flagGroup ) {
                def flags = ("debug" == this.mBuildType) ? flagGroup.debug : flagGroup.release;
                this.mArchFlagsBlocks.add("\tLOCAL_CPPFLAGS += ${flags}");
            }

            useElse = true;
        }
        this.mArchFlagsBlocks.add("endif");
    }


    void parseSharedLibs(Project project) {
        this.mSharedLibs = [];
        if( ! project.cinder.ldLibs.empty ) { 
            mSharedLibs.add("\\");
            project.cinder.ldLibs.each {
                def libName = it;
                if(("GLESv3" == libName) && (true == project.cinder.gles2)) {
                    libName = "GLESv2";
                }
                mSharedLibs.add("\t" + "-l" + libName + "\\");
            }
        }
    }

    void parseStaticLibs(Project project) {
        this.mStaticLibs = []
        this.mStaticBlocks = []
        if( ! project.cinder.staticLibs.empty ) {
            this.mStaticLibs.add("\\");

            // libcinder
            def libCinder = project.cinder.libCinder[this.mBuildType];            
            if( project.cinder.gles2 ) {
                File f = new File( libCinder );
                def libName = f.getName();
                def libParent = f.getParent();
                if( "libcinder.a" == libName ) {
                    libName = "libcinder-es2.a";
                }
                else if( "libcinder_d.a" == libName ) {
                    libName = "libcinder-es2_d.a";
                }
                libCinder = (new File( libParent, libName )).toString();
            }            

            def finalLibs = [];           
            boolean insertedLibCinder = false;
            project.cinder.staticLibs.each {
                def lib = it;
                // Check for: $(LIBCINDER)
                if( "\$(LIBCINDER)" == lib.toUpperCase() ) {
                    lib = libCinder;
                    insertedLibCinder = true;
                }
                finalLibs.add( lib );
            }
            if( ! insertedLibCinder ) {
                finalLibs.add( 0, libCinder );
            }               
            //
            mStaticLibsFullPaths = []
            def minSdkVersion = project.android.defaultConfig.minSdkVersion.apiLevel;
            //
            finalLibs.each {
                String path = (new File(it)).canonicalPath.toString();
                
                for(int i = 0; i < this.mArchs.size(); ++i) {
                    def itArch = this.mArchs[i];
                    String expandedPath = path.replace( "\$(TARGET_ARCH_ABI)", itArch );
                    expandedPath = expandedPath.replace( "\$(TARGET_PLATFORM)", "android-${minSdkVersion}" );
                    mStaticLibsFullPaths.push( expandedPath );
                }
                //println ".a lib path: ${path}";


                // short name
                String shortName = path
                def lastSlashPos = shortName.lastIndexOf( "/" )
                if( -1 != lastSlashPos ) {
                    shortName = shortName.substring(lastSlashPos + 1, shortName.length())
                }
                shortName = shortName.replace(".a", "")
                this.mStaticLibs.add("\t" + shortName + " \\")


                this.mStaticBlocks.add("include \$(CLEAR_VARS)")
                this.mStaticBlocks.add("LOCAL_MODULE    := " + shortName)
                this.mStaticBlocks.add("LOCAL_SRC_FILES := " + path)
                this.mStaticBlocks.add("include \$(PREBUILT_STATIC_LIBRARY)")
                this.mStaticBlocks.add("")
            }
        }
    }

    String relativePath( aBaseDir, aFilePath ) {
        Path baseDir  = Paths.get(aBaseDir);
        Path filePath = Paths.get(aFilePath);
        Path result = baseDir.relativize(filePath);
        return result.toString()
    }

    void ndkDirCheck(Project project) {
        String ndkDir = project.cinder.ndkDir; //project.plugins.findPlugin('com.android.application').getNdkFolder()
        if( null == ndkDir ) {
            throw new InvalidUserDataException("ndkDir is null! Make sure ndk.dir in build.gradle's cinder {} section is set.")
        }
        //
        if( ! (new File( ndkDir )).getCanonicalFile().exists() ) {
            throw new InvalidUserDataException("ndkDir does not exist! (ndkDir=" + ndkDir.toString()+")")
        }
    }

    String makeNdkArg(String key, String value) {
        String result = value.isEmpty() ?  "${key}" : "${key}=${value}"
        return result
    }
}

