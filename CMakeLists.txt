  
cmake_minimum_required(VERSION 3.6)
project(octree)

set(CMAKE_CXX_STANDARD 17)  # for c++17


file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")

add_executable(octree ${SOURCE_FILES})

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )


include_directories(include)

#add_subdirectory(benchmark)
#target_link_libraries(octree benchmark::benchmark)

add_subdirectory(glfw)
include_directories(glfw/include)

target_link_libraries(octree glfw ${GLFW_LIBRARIES})
target_link_libraries(octree glfw ${OPENGL_glu_LIBRARY} ${GLFW_LIBRARIES})

target_link_libraries(octree ${OPENGL_LIBRARIES})