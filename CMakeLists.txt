cmake_minimum_required(VERSION 3.11.0)
project(planet)

set(gcc_like_cxx "$<IN_LIST:$<CXX_COMPILER_ID>,AppleClang;Clang;GNU>")
set(msvc_cxx "$<IN_LIST:$<CXX_COMPILER_ID>,MSVC>")

if(WIN32)
    if (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};../deps/boost-mingw;../deps/glfw-mingw-x64;../deps/glm")
    elseif(MSVC AND "${CMAKE_SIZEOF_VOID_P}" STREQUAL 8)
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};../deps/boost-msvc;../deps/glfw-msvc-x64;../deps/glm")
    elseif(MSVC)
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};../deps/boost-msvc;../deps/glfw-msvc;../deps/glm")
    endif()
endif()

set(Boost_USE_STATIC_LIBS "On")
find_package(Boost REQUIRED COMPONENTS filesystem)
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
target_compile_options(planet PRIVATE
    "$<${gcc_like_cxx}:-Wall>"
    "$<$<AND:${gcc_like_cxx},$<CONFIG:Debug>>:-Og;-ggdb>"
    "$<${msvc_cxx}:/W4>")
target_link_libraries(planet PUBLIC
    glad
    glfw
    ${glm_library})
target_link_options(planet PRIVATE
    "$<$<AND:${gcc_like_cxx},$<PLATFORM_ID:Windows>>:-static-libgcc;-static-libstdc++;-static>")
