<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta content="text/html; charset=ISO-8859-1" http-equiv="content-type"><title>CinderBlocks: Bullet</title>
</head>
CINDERBLOCKS: BULLET PHYSICS


Installing or updating Bullet
-----------------------------

1. Download the Bullet source from http://bulletphysics.com
2. Follow the Bullet build instructions:
	- download CMake from http://cmake.org and install
	- generate Xcode or Visual Studio projects by typing, for example, "cmake . -G Xcode" in the Bullet directory
	    NOTE FOR WINDOWS: when generating Visual Studio files with CMake make sure to set the option USE_MSVC_RUNTIME_LIBRARY_DLL       
	    to OFF (via CMake gui or in the CMakeLists.txt file)
	- build Bullet with Xcode or Visual Studio
	
3. Put the Bullet folder (e.g. "bullet-2.75") in your cinder/blocks/bullet directory and rename it to "bullet"


Creating a project using Bullet
-------------------------------

1. Create a new Project using TinderBox. It should reside in a subfolder of cinder/blocks/bullet, e.g. cinder/blocks/bullet/apps
2. MAC OS X:
        a.  Open the XCode Project.
		b. 	Under "Project"->"Edit Active Targets", under "Linker" find the entry "Other Linker Flags" 
		        and add the paths to the bullet libraries; the essential ones are:
		        <your-cinder-path>/blocks/bullet/bullet/src/BulletCollision/<Debug|Release>/libBulletCollision.a 
		        <your-cinder-path>/blocks/bullet/bullet/src/BulletDynamics/<Debug|Release>/libBulletDynamics.a 
		        <your-cinder-path>/blocks/bullet/bullet/src/LinearMath/<Debug|Release>/libLinearMath.a 
		c.  Under "Project"->"Edit Project Settings", in the "Build" tab under "Search Paths" find the entry 
		        "User Header Search Paths" and add "<your-cinder-path>/blocks/bullet/bullet/src"
		d.	Add the following line to the includes in your app:
				#include "../blocks/bullet/CinderBullet.h"
				
   WINDOWS / VISUAL STUDIO:
        a.  Open the VS project.
        b.  Make the following adjustments in "Project"->"Properties":
                - Configuration Properties -> C/C++ -> General -> Additional Include Directories: 
                    Add <your-cinder-path>\blocks\bullet\bullet\src
                - Configuration Properties -> Linker -> General -> Additional Library Directories: 
                    Add <your-cinder-path>\blocks\bullet\bullet\lib\<Debug|Release>
                - Configuration Properties -> Linker -> Input -> Additional Dependencies: 
                    Add: BulletDynamics.lib BulletCollision.lib LinearMath.lib
        c.	Add the following line to the includes in your app:
            	#include "../blocks/bullet/CinderBullet.h"
				

You're good to go! The Bullet CinderBlock includes some functions for conveniently creating common shapes from Cinder data structures, such as boxes, spheres, imported OBJ models and heightfield images. We recommend referring to the apps in the cinder/blocks/bullet/samples folder to see how to set up a basic physics environment. 

		
		