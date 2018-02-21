cmake_minimum_required(VERSION 3.9)
project(make-build-debug/Numbskull)
find_package(Boost 1.58.0 COMPONENTS mpi serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


set(CMAKE_CXX_STANDARD 14)

add_executable(Numbskull
        Factor.cpp Factor.h
        AssignInfo.h AssignInfo.cpp
        Variable.cpp Variable.h
        Weight.cpp Weight.h
        FactorGraph.cpp FactorGraph.h
        FactorType.cpp FactorType.h
        main.cpp)
target_link_libraries(Numbskull ${Boost_LIBRARIES})