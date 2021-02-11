cmake_minimum_required(VERSION 3.12)

project(tcr C CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(gifdec gifdec/gifdec.c)
target_include_directories(gifdec PUBLIC gifdec/gifdec.h)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake" 
                 TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(REQUIRES stb/20200203
                REQUIRES freetype/2.10.4
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

include_directories(include)
include_directories(gifdec)

add_executable(main ${SOURCE_FILES})
target_link_libraries(main CONAN_PKG::stb)
target_link_libraries(main CONAN_PKG::freetype)
target_link_libraries(main gifdec)
