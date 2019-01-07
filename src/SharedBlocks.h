// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_SHARED_BLOCKS_H_
#define _PLANET_SHARED_BLOCKS_H_

#include <vector>

#include "opengl.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class ViewAndProjectionBlock {
public:
    ViewAndProjectionBlock();
    ViewAndProjectionBlock(glm::mat4x4 &view, glm::mat4x4 &projection);
    ~ViewAndProjectionBlock();

    static const GLuint BINDING_INDEX;
    static void setOffsets(GLuint program, const char *block_name);

    void setView(const glm::mat4x4 &new_view);
    const glm::mat4x4& view() const;

    void setProjection(const glm::mat4x4 &new_proj);
    const glm::mat4x4& projection() const;

    void writeToBuffer();
    void bind() const;
    void unbind() const;

private:
    void createBuffer();
    void destroyBuffer();

    GLuint m_buffer;
    glm::mat4x4 m_view, m_projection;
    static GLuint SIZE, VIEW_OFFSET, VIEW_INV_OFFSET, PROJECTION_OFFSET;
};

struct LightInfo {
    GLuint enabled;
    glm::vec3 direction;
    // glm::vec4 color;
    // GLuint specular_exp;

    LightInfo();
    ~LightInfo();
};

struct LightOffsetInfo {
    GLuint enabled, direction; // , color, specular_exp;
};

class LightListBlock {
public:
    LightListBlock();
    ~LightListBlock();

    static const GLuint BINDING_INDEX;
    static void setOffsets(GLuint program, const char *block_name);

    void enableLight(unsigned int index, const glm::vec3 &direction);

    void writeToBuffer();
    void bind() const;
    void unbind() const;

private:
    void createBuffer();
    void destroyBuffer();

    GLuint m_buffer;
    std::vector<LightInfo> m_light_info;
    static GLuint SIZE, NUM_LIGHTS;
    static std::vector<LightOffsetInfo> OFFSETS;
};

#endif
