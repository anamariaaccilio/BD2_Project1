cmake_minimum_required(VERSION 3.22)
project(BD2_Project1)

set(CMAKE_CXX_STANDARD 14)

add_executable(BD2_Project1
        main.cpp
        node.h
        avl.h
        extendibleHashF.h
        parser.h
        sequentialFile.h)
