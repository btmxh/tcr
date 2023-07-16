cmake_minimum_required(VERSION 3.12)

project(tcr C CXX)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

include_directories(include)
include_directories(gifdec)

find_package(Freetype REQUIRED)

add_executable(main ${SOURCE_FILES})
target_link_libraries(main PRIVATE Freetype::Freetype)
