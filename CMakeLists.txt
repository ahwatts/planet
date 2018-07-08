cmake_minimum_required(VERSION 3.11.0)
project(planet)

add_executable(planet src/planet.cpp)
target_compile_features(planet PUBLIC cxx_std_17)
