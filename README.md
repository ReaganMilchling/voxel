# voxel

My first attempt to make a minecraft clone in c++ using openGL.
It's more of a rendering demo than a game currently. 

# Todo items
- make game loop independent of refresh rate and add separate tick rate
- proper physics for falling and jumping
- shade cardinal directions differently for contrast
- block breaking and placing
- saving world

# Build & Compilation

Setup lsp:
- cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
- Move file to root dir

build & play:
- mkdir build && cd build
- cmake .. && make
- ./voxel
