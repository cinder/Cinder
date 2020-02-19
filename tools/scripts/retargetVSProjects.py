import os
import fnmatch
import sys
import shutil
from distutils import dir_util

if len(sys.argv) != 2:
    print("Usage:\npython retarget_vs_projects.py [directory]\n  Ex: python retargetVSProject.py samples")
    sys.exit(-1)

START_DIR = sys.argv[1]

# Customize these for the current upgrade:
VC_DIR_OLD = 'vc2015'
VC_DIR_NEW = 'vc2019'
DEVENV_EXE = "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.exe"

# These only need to be changed for debugging purposes
RUN_DEVENV = True            # disable if you don't want to run the devenv.exe command
CLEANUP_OLD = True           # disabling this will leave the old project files around
MODIFY_IN_PLACE = False      # debug switch to modify the project files within the old folder

# # This will hopefully open Visual Studio and print devenv.exe's options
# devenvCommand = "\"" + DEVENV_EXE + "\" /?"
# print("executing: " + devenvCommand)
# os.system(devenvCommand)
# sys.exit(0)

for root, dirs, files in os.walk(START_DIR):
    for build_match in fnmatch.filter(dirs, VC_DIR_OLD):
        # copy old VS directory to new location
        vcPathOldRelative = os.path.join(root, VC_DIR_OLD)
        vcPathNewRelative = os.path.join(root, VC_DIR_NEW)

        if MODIFY_IN_PLACE:
            vcPathNewRelative = vcPathOldRelative
        else:
            dir_util.copy_tree(vcPathOldRelative, vcPathNewRelative)

        # find .vcxproj file in new vc dir
        vcxprojFilePath = ''
        for file in os.listdir(vcPathNewRelative):
            if file.endswith(".vcxproj"):
                vcxprojFilePath = os.path.join(vcPathNewRelative, file)
                break

        if vcxprojFilePath == '':
            print("could not find .vcxproj file in directory: " + vcPathNewRelative + ", skipping.")
            continue

        vcxprojFilePath = os.path.abspath(vcxprojFilePath)
        print("vcxproj file: " + vcxprojFilePath)

        # run devenv program
        if RUN_DEVENV:
            cmd = "\"" + DEVENV_EXE + "\" " + vcxprojFilePath + " /Upgrade"
            os.system(cmd)

        # TODO: clean up temp files from devenv.exe
        # - I was lazy and did it with 'git clean -dfx'

        # delete old vc dir
        if CLEANUP_OLD:
            shutil.rmtree(vcPathOldRelative)

print("complete.")
