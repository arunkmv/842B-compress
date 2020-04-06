project(842decompressor)

cmake_minimum_required(VERSION 3.14)
add_executable(decompressor.out
        main.cpp Decompressor.cpp Decompressor.h
        ../common/842defs.h ../common/crc32.h ../common/crc32table.h
        ../common/CompressorConfig.cpp ../common/CompressorConfig.h)

set(CMAKE_CXX_FLAGS " -Ofast -march=native")
set(CMAKE_CXX_COMPILER g++-8)
