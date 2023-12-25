# voxel

My first attempt to make a minecraft clone in c++ using openGL.
It's more of a rendering demo than a game currently. 

Setup lsp:
- XX=clang++ CC=clang cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
- Move file to root dir

build & play:
- mkdir build && cd build
- CXX=clang++ CC=clang cmake .. && make or (cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang ..)
- ./voxel
