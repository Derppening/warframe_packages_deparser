cmake_minimum_required(VERSION 3.8)
project(warframe_packages_deparser)

include(compile_flags.cmake)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -static")

file(GLOB SOURCE_FILES *.cpp)
add_executable(warframe_packages_deparser ${SOURCE_FILES})