cmake_minimum_required(VERSION 3.11.0)
project(planet)

if(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};../deps/boost-msys;../deps/glfw-msys;../deps/glm")
    set(BOOST_LIBRARYDIR "C:/Users/andrew/Projects/deps/boost-msys/lib")
endif()

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(vendor/embed-resource)
add_subdirectory(vendor/glad)

embed_resources(SHADERS
  src/shaders/terrain.vert
  src/shaders/terrain.frag)

add_executable(planet
  src/planet.cpp
  src/Models.cpp
  src/Terrain.cpp
  ${SHADERS})
target_compile_features(planet PUBLIC cxx_std_14)
target_link_libraries(planet PUBLIC glad glfw glm)

if(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options(planet PUBLIC "-Wall")
endif()
