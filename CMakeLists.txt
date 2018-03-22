cmake_minimum_required(VERSION 3.5)
project(cmake-build-debug/Numbskull)
find_package(Boost 1.58.0 COMPONENTS mpi serialization REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


set(CMAKE_CXX_STANDARD 17)

add_executable(Numbskull
        Factor.cpp Factor.h
        Variable.cpp Variable.h
        FactorGraph.cpp FactorGraph.h
        main.cpp Utility.cpp Utility.h Edge.cpp Edge.h)
target_link_libraries(Numbskull ${Boost_LIBRARIES})