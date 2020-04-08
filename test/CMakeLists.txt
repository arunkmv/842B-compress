project(842compdecompTest)

cmake_minimum_required(VERSION 3.14)
add_executable(test.out CompDecompTest.cpp
        ../compressor/Compressor.h ../compressor/Compressor.cpp ../compressor/HashManager.h ../compressor/HashManager.cpp
        ../decompressor/Decompressor.h ../decompressor/Decompressor.cpp
        ../common/CompressorConfig.h ../common/CompressorConfig.cpp )

set(CMAKE_CXX_FLAGS " -Ofast -march=native")
set(CMAKE_CXX_COMPILER g++-8)