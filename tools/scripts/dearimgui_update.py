import glob, os, shutil, sys
# "Usage: dearimgui_update.py SOURCE DEST"

im_path = os.path.abspath(sys.argv[1])
ci_path = os.path.abspath(sys.argv[2])

def copy_files(src_dir, target_dir, extension):
    files = glob.iglob(os.path.join(src_dir, extension))
    for src in files:
        if os.path.isfile(src):
            print( src + ' => ' + target_dir )
            shutil.copy2(src, target_dir)

if __name__ == '__main__':
    dearimgui_folders = ['', 'misc/cpp', 'misc/freetype']
    for src in dearimgui_folders:
        copy_files( os.path.join(im_path, src), os.path.join(ci_path, 'include/imgui'), '*.h')
        copy_files( os.path.join(im_path, src), os.path.join(ci_path, 'src/imgui'), '*.cpp')
    
    src = os.path.join(im_path,'examples/imgui_impl_opengl3.h')
    dest = os.path.join(ci_path,'include/imgui')
    shutil.copy2(src, dest)
    print( src + ' => ' + dest )

    src = os.path.join(im_path,'examples/imgui_impl_opengl3.cpp')
    dest = os.path.join(ci_path,'src/imgui')
    shutil.copy2(src, dest)
    print( src + ' => ' + dest )

    # Fixes linking on platforms such as Angle (prevents auto-detection of glad headers)
    with open( os.path.join(ci_path, 'include/imgui/imgui_impl_opengl3.h'), 'r+' ) as file:
        data = file.read().replace('#pragma once', '#pragma once\n#include \"cinder/CinderImGuiConfig.h\"\n')
        file.seek(0)
        file.write(data)
        file.truncate()
