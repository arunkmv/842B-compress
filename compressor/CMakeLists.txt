project(842compressor)

add_executable(compressor.out
        main.cpp
        Compressor.cpp Compressor.h CompressorConfig.cpp CompressorConfig.h)

set(CMAKE_CXX_FLAGS " -Ofast -march=native")
set(CMAKE_CXX_COMPILER g++-8)
