cmake_minimum_required(VERSION 4.0.0)
project(planet)

# find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# GLM changed their library link target in a way that we can't really detect.
if(TARGET glm::glm)
    set(glm_library "glm::glm")
elseif(TARGET glm)
    set(glm_library "glm")
else()
    message(FATAL_ERROR "No glm link target found")
endif()

add_subdirectory(vendor/embed-resource)
add_subdirectory(vendor/glad)

embed_resources(SHADERS
    src/shaders/curve.vert
    src/shaders/curve.frag
    src/shaders/ocean.vert
    src/shaders/ocean.frag
    src/shaders/terrain.vert
    src/shaders/terrain.frag)

add_executable(planet
    src/Curve.cpp
    src/Models.cpp
    src/Noise.cpp
    src/Ocean.cpp
    src/OpenGLUtils.cpp
    src/SharedBlocks.cpp
    src/Terrain.cpp
    src/planet.cpp
    ${SHADERS})
target_include_directories(planet PUBLIC vendor/embed-resource)
target_compile_features(planet PUBLIC cxx_std_14)
target_link_libraries(planet PUBLIC
    glad
    glfw
    ${glm_library})
