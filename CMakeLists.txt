cmake_minimum_required(VERSION 3.11.0)
project(planet)

if(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};../deps/glfw-msys;../deps/glm")
endif()

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(vendor/glad)

add_executable(planet src/planet.cpp)
target_compile_features(planet PUBLIC cxx_std_14)
target_link_libraries(planet PUBLIC glad glfw glm)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(planet PUBLIC "-Wall")
endif()
