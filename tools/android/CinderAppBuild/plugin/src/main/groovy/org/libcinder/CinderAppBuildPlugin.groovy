/*

Example cinder property struct:

    cinder {
        ndkDir = "/Users/hai/DevTools/android/ndk"
        verbose = true
        moduleName = "BasicApp"
        srcFiles = ["../../../src/BasicApp.cpp"]
        cppFlags = "-std=c++11 -fexceptions -g -mfpu=neon"
        includeDirs = ["../../../../../include", "../../../../../boost"]
        ldLibs = ["log", "android", "EGL", "GLESv3", "z"]
        staticLibs = [
                "${projectDir}/../../../../../lib/android/\$(TARGET_PLATFORM)/\$(TARGET_ARCH_ABI)/libcinder_d.a",
                "${projectDir}/../../../../../lib/android/\$(TARGET_PLATFORM)/\$(TARGET_ARCH_ABI)/libboost_filesystem.a",
                "${projectDir}/../../../../../lib/android/\$(TARGET_PLATFORM)/\$(TARGET_ARCH_ABI)/libboost_system.a"
        ]
        stl = "gnustl_static"
        toolChainVersion = "4.9"
        archs = ["armeabi", "armeabi-v7a", "arm64-v8a", "mips", "mips64", "x86","x86_64"]
    }

*/
package org.libcinder.gradle

import org.gradle.api.GradleException
import org.gradle.api.InvalidUserDataException
import org.gradle.api.Project
import org.gradle.api.Plugin
import org.gradle.api.tasks.*
import org.gradle.internal.reflect.Instantiator
import java.nio.file.Path;
import java.nio.file.Paths;

class ArchTarget {
    String name;
}

class CinderAppBuildPlugin implements Plugin<Project> {
    static final kValidArchs = ["arm64-v8a", "armeabi", "armeabi-v7a", "mips", "mips64", "x86","x86_64"]

    def mProjectDir   = "";
    def mBuildDir     = "";
    def mBuildType    = "";
    def mNdkBuildDir  = "";

    def mSourceFiles  = []
    def mIncludeDirs  = []
    def mSharedLibs   = []
    def mStaticLibs   = []
    def mStaticBlocks = []
    def mArchs        = []

    String relativePath( aBaseDir, aFilePath ) {
        Path baseDir  = Paths.get(aBaseDir);
        Path filePath = Paths.get(aFilePath);
        Path result = baseDir.relativize(filePath);
        return result.toString()
    }

    void parseSourceFiles(Project project, cppBuildDir) {
        this.mSourceFiles = []
        // Files
        if(! project.cinder.srcFiles.isEmpty()) {
            this.mSourceFiles.add("\\")
            project.cinder.srcFiles.each {
                def file = new File("${project.projectDir}/" + it)
                String path = file.canonicalPath.toString()                    
                if( ! path.startsWith( "." ) ) {
                    String relPath = this.relativePath( cppBuildDir, path )
                    this.mSourceFiles.add("\t" + relPath + " \\")
                }
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
                    String path = it.canonicalPath.toString()                    
                    if( ! path.startsWith( "." ) ) {
                        String relPath = this.relativePath( cppBuildDir, path )
                        this.mSourceFiles.add("\t" + relPath + " \\")
                    }
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

    void parseSharedLibs(Project project) {
        this.mSharedLibs = []
        if( ! project.cinder.ldLibs.empty ) { 
            mSharedLibs.add("\\")
            project.cinder.ldLibs.each {
                mSharedLibs.add("\t" + "-l" + it + "\\")
            }
        }
    }

    void parseStaticLibs(Project project) {
        this.mStaticLibs = []
        this.mStaticBlocks = []
        if( ! project.cinder.staticLibs.empty ) {
            this.mStaticLibs.add("\\")
            project.cinder.staticLibs.each {
                String path = (new File(it)).canonicalPath.toString()

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

    void writeAndroidMk(Project project, String filePath) {
        def lines = []
        lines.add("# Generated by CinderAppBuildPlugin")
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
        lines.add("# C++ Source Files" )
        lines.add("LOCAL_SRC_FILES := "+ this.mSourceFiles.join("\n"))
        lines.add("")
        lines.add("# C++ Flags" )
        lines.add("LOCAL_CPPFLAGS += ${project.cinder.cppFlags}")
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
        lines.add("")
        
        def outFile = new File( "${filePath}" )
        outFile.text = lines.join("\n")

        if( project.cinder.verbose && outFile.exists() ) {
            println "Wrote ${filePath}"
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

    String makeNdkArg(key, value) {
        String result = "${key}=${value}"
        return result
    }

    void cinderGenerateNdkBuild(Project project) {
        this.ndkDirCheck(project)

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

    void cinderCompileNdk(Project project) {
        this.ndkDirCheck(project)

        String ndkDir = (new File( project.cinder.ndkDir )).getCanonicalPath(); //project.plugins.findPlugin('com.android.application').getNdkFolder().toString()
        if( project.cinder.verbose ) {
            println "NDK_ROOT: ${ndkDir}";
        }

        def minSdkVersion = project.android.defaultConfig.minSdkVersion.apiLevel;
       
        def ndkBuildCmd  = "${ndkDir}/ndk-build"
        def ndkBuildArgs = []
        
        if( project.cinder.verbose ) {
            ndkBuildArgs.add(this.makeNdkArg("V", "1"))
        }        

        try {
            this.parseArchs( project );                
        }
        catch( e ) {
            throw new GradleException("Failed parsing cinder.archs[], e=" + e)
        } 

        String archsStr = this.mArchs.join(",");
        if( archsStr.endsWith( "," ) ) {
            archsStr = archsStr.substring( 0, archsStr.length() - 1 );
        }

        ndkBuildArgs.add(this.makeNdkArg("NDK_PROJECT_PATH",      "null"))
        ndkBuildArgs.add(this.makeNdkArg("APP_BUILD_SCRIPT",      "${this.mNdkBuildDir}/Android.mk"))
        ndkBuildArgs.add(this.makeNdkArg("APP_PLATFORM",          "android-${minSdkVersion}"))
        ndkBuildArgs.add(this.makeNdkArg("NDK_OUT",               "${this.mNdkBuildDir}/obj"))
        ndkBuildArgs.add(this.makeNdkArg("NDK_LIBS_OUT",          "${this.mProjectDir}/src/main/jniLibs"))            
        ndkBuildArgs.add(this.makeNdkArg("APP_STL",               "gnustl_static"))
        ndkBuildArgs.add(this.makeNdkArg("APP_ABI",               "${archsStr}"))
        ndkBuildArgs.add(this.makeNdkArg("NDK_TOOLCHAIN_VERSION", "${project.cinder.toolChainVersion}"))
   
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
    
    void apply(Project project) {
        project.extensions.create("cinder", CinderAppBuildPluginExtension)
        project.cinder.extensions.archTargets = project.container(ArchTarget) { String name ->
            ArchTarget archTarget = project.gradle.services.get(Instantiator).newInstance(ArchTarget, name)
            return archTarget
        }

        this.mProjectDir = project.projectDir;
        this.mBuildDir  = project.buildDir;
        this.mBuildType = 'debug';
               
        // TASK: cinderGenerateDebugNdkBuild
        project.task('cinderGenerateDebugNdkBuild') << {
            this.mBuildType = "debug";
            this.mNdkBuildDir = "${this.mBuildDir}/cinder/${this.mBuildType}"
            this.cinderGenerateNdkBuild(project);
        }  

        // TASK: cinderCompileDebugNdk
        project.task('cinderCompileDebugNdk', dependsOn: 'cinderGenerateDebugNdkBuild') << {
            this.cinderCompileNdk(project);
        }

        // TASK: cinderGenerateReleaseNdkBuild
        project.task('cinderGenerateReleaseNdkBuild') << {
            this.mBuildType = "release";
            this.mNdkBuildDir = "${this.mBuildDir}/cinder/${this.mBuildType}"
            this.cinderGenerateNdkBuild(project);
        }  

        // TASK: cinderCompileReleaseNdk
        project.task('cinderCompileReleaseNdk', dependsOn: 'cinderGenerateReleaseNdkBuild') << {
            this.cinderCompileNdk(project);
        }
    }
}

class CinderAppBuildPluginExtension {
    def ndkDir = ""
    def cinderDir = ""
    def verbose = false
    def moduleName = ""
    def srcFiles = []
    def srcDirs = []
    def cppFlags = ""
    def includeDirs = []
    def ldLibs = []
    def staticLibs = []
    def stl = "gnustl_static"
    def toolChainVersion = "4.9"
    def archs = [];
}

