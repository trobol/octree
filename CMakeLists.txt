  
cmake_minimum_required(VERSION 3.6)
project(octree)

set(CMAKE_CXX_STANDARD 14)  # for c++17

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()


file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB_RECURSE IMGUI_FILES "deps/imgui/*.cpp")

add_executable(octree ${SOURCE_FILES} ${IMGUI_FILES})

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )

add_definitions(-DASSET_PATH="${CMAKE_SOURCE_DIR}/assets")

include_directories(include)
include_directories(deps)

#add_subdirectory(benchmark)
#target_link_libraries(octree benchmark::benchmark)

add_subdirectory(glfw)
include_directories(glfw/include)

source_group(TREE "${CMAKE_SOURCE_DIR}/include" PREFIX "Header Files" FILES ${HEADER_LIST})

target_link_libraries(octree glfw ${GLFW_LIBRARIES})
target_link_libraries(octree glfw ${OPENGL_glu_LIBRARY} ${GLFW_LIBRARIES})

target_link_libraries(octree ${OPENGL_LIBRARIES})