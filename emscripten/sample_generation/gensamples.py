'''
    This is a script for generating working sample files fit for 
    building with Emscripten. It will generate a folder called "emscripten" at the root
    of each sample and in it, it will generate a CMakeLists.txt file as well as a build directory 
    to run that cmake file.  
'''

import os
from shutil import copy

BASE_PATH = "../../samples"
dirs = os.listdir(BASE_PATH)

#ignored folders of places we don't need to generate something in. 
ignore = [
    "_AllSamples",
    "vc2015",
    "xcode",
    "xcode_ios",
    "_audio",
    "resources",
    "src",
    "assets",
    "include",
    "androidstudio",
    "xcode_iOS",
    "vc2013_winrt",
    "vc2015_winrt",
    ".jpg",
    ".png",
    "LICENSE",
    "models",
    "sound",
    "winrt",
    ".icns",
    ".ico",
    "emsdk_set_env.sh",
    "environment_maps",
    "QuickTimeAvfWriter",
    "iosKeyboard",
    "iosNativeControls",
    "LocationManager",
    "SerialCommunication",
    "ClothSimulation",
    "GeometryShaderBasic",
    "NVidiaComputeParticles",
    "ParticleSphereCS",
    "BufferPlayer",
    "common",
    "DelayFeedback",
    "InputAnalyser",
    "MultichannelOutput",
    "NodeAdvanced",
    "NodeBasic",
    "NodeSubclassing",
    "VoiceBasic",
    "VoiceBasicProcessing",
    "PBOReadBack"
]

# checks the filename you pass in against the ignore list to see if
# it's something that needs to be ignored. 
def should_ignore(file):
    shouldIgnore = False
    for x in ignore :
        if file.find(x) != -1:
            shouldIgnore = True
    return shouldIgnore


print("Generating Emscripten project folder for samples.")
for dir in dirs :
    if should_ignore(dir) == False:
        subdirs = os.listdir(BASE_PATH + "/" + dir)
        template = ""
    
        for subdir in subdirs:
            # check to make sure there aren't directories that should be ignored. 
            # if not the proceed. 
            if should_ignore(subdir) != True:
                projectPath = BASE_PATH + "/" + dir

                if(subdir == "proj"):
                    template = open( BASE_PATH + "/" + dir + "/proj/cmake/CMakeLists.txt").read()
                elif os.path.exists(BASE_PATH + "/" + dir + "/" + subdir + "/proj/cmake/CMakeLists.txt"): 
                    template = open( BASE_PATH + "/" + dir + "/" + subdir + "/proj/cmake/CMakeLists.txt").read()
                
                # replace cinderMakeApp with cinderEmscriptenApp
                template = template.replace("cinderMakeApp","cinderEmscriptenApp")

                # replace ci_make_app with ci_emscripten_app
                template = template.replace("ci_make_app","ci_emscripten_app")

                # fix pathing for Cinder
                template = template.replace(
                    'CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../.." ABSOLUTE',
                    'CINDER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../" ABSOLUTE'
                )

                # fix pathing for src 
                template = template.replace(
                    'APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE',
                    'APP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE'
                )

                
                ######## make a build directory at proj / emscripten / build #################3
                if(subdir == "proj"):
                    
                    path = projectPath + "/emscripten"
                    if os.path.exists(path) is False:
                        os.makedirs(path)
                    if os.path.exists(path + "/build") is False:
                        os.makedirs(path + "/build")

                else:
                    path = projectPath + "/" + subdir
                    
                    if os.path.exists(path + "/emscripten") is False:
                        os.makedirs(path + "/emscripten")
                    if os.path.exists(path + "/emscripten/build") is False:
                        os.makedirs(path + "/emscripten/build")
                
                ######### copy cmake template ###############
                templatePath = ""
                if subdir == "proj":
                    templatePath = projectPath + "/emscripten"
                else:
                    templatePath = projectPath + "/" + subdir + "/emscripten"
                
                with open(templatePath + "/CMakelists.txt","w+") as f:
                    f.write(template)
                
print("Done generating - you'll find an emscripten folder at <sample name> / emscripten")
                    


                

                       






                

               

            
