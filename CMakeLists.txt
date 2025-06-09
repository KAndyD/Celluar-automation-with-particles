cmake_minimum_required(VERSION 3.10)
project(ParticleSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(ParticleSim
    main.cpp
    simulation.cpp
    renderer.cpp
    statistics.cpp
)


