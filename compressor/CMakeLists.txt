project(842compressor)

cmake_minimum_required(VERSION 3.14)
add_executable(compressor.out
        main.cpp 842defs.h
        Compressor.cpp Compressor.h CompressorConfig.cpp CompressorConfig.h
        HashManager.cpp HashManager.h)

set(CMAKE_CXX_FLAGS " -Ofast -march=native")
set(CMAKE_CXX_COMPILER g++-8)
