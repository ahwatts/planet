// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_TERRAIN_H_
#define _PLANET_TERRAIN_H_

#include <vector>

#include "glm_defines.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "opengl.h"

#include "SharedBlocks.h"

class NoiseFunction;

struct TerrainVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

class Terrain {
public:
    Terrain(const NoiseFunction &noise);
    Terrain(const Terrain &other) = delete;
    Terrain(Terrain &&other) = delete;
    ~Terrain();

    Terrain& operator=(const Terrain &other) = delete;
    Terrain& operator=(Terrain &&other) = delete;

    void render(glm::mat4x4 &model);

private:
    Terrain();

    void initGeometry(const NoiseFunction &noise);
    void initBuffers();
    void initProgram();
    void initVAO();

    std::vector<TerrainVertex> m_vertices;
    std::vector<GLuint> m_indices;
    
    GLuint m_array_buffer, m_elem_buffer;
    
    GLuint m_vertex_shader, m_fragment_shader, m_program;
    GLint m_position_loc, m_normal_loc;
    GLint m_model_loc;
    
    GLuint m_array_object;
};

#endif
