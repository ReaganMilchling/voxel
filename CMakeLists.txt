cmake_minimum_required(VERSION 3.27)
project(voxel)
set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE voxelSource src/*.cpp src/*.h)
add_executable(
    voxel 
    ${voxelSource}
    third_party/glad/src/glad.c
)

add_subdirectory(third_party/glfw EXCLUDE_FROM_ALL)
add_subdirectory(third_party/glm EXCLUDE_FROM_ALL)
include_directories(third_party/glad/include EXCLUDE_FROM_ALL)
include_directories(res)

target_link_libraries(voxel PRIVATE glfw glm)

add_custom_command(
    TARGET voxel PRE_BUILD COMMAND
    ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res
)
