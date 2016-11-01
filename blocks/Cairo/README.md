# Cairo

## INFO

Block that wraps the Cairo Graphics library. Included is also a build script, supporting multiple platform and architecture build options.

## INSTALLATION 

- If a needed platform is not included, you can simply build it using either the `install.sh` for Linux and Macosx and Ios or `install.bat` for Windows.
- Simply pass the platform into the script, i.e. linux, ios, macosx or windows, and depending on the speed of your machine, go get a cup of coffee.
- The platform specific libraries will be built and placed in the correct `include/` and `lib/` folders. 
- There'll be a folder in the install dir, `tmp/`, which holds the full library installs for use with other libraries that depend on this one, such as harfbuzz or pango. If you're not building those libraries you can delete the `tmp/` folder.


