# voxel

My first attempt to make a minecraft clone in c++ using openGL.
It's more of a rendering demo than a game currently. 

# Todo items
- real threading
- proper physics for falling and jumping
- shade cardinal directions differently for contrast
- block breaking and placing
- saving world

# Build & Compilation

Clone recursively:
```git clone --recurse-submodules {url}```
Or download later:
```git submodule update --init --recursive```

Setup lsp:
- cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
- Move file to root dir

Build & play:
- mkdir build && cd build
- cmake .. && make
- ./voxel
