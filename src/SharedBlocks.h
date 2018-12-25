// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#ifndef _PLANET_SHARED_BLOCKS_H_
#define _PLANET_SHARED_BLOCKS_H_

#include "opengl.h"
#include "glm/mat4x4.hpp"

class ViewAndProjectionBlock {
public:
    ViewAndProjectionBlock();
    ViewAndProjectionBlock(glm::mat4x4 &view, glm::mat4x4 &projection);
    ~ViewAndProjectionBlock();

    static void setOffsets(GLuint program, const char *block_name);

    void setView(const glm::mat4x4 &new_view);
    const glm::mat4x4& view() const;

    void setProjection(const glm::mat4x4 &new_proj);
    const glm::mat4x4& projection() const;

    void writeToBuffer();
    void bindToIndex(GLuint index) const;
    void unbindIndex(GLuint index) const;

private:
    void createBuffer();
    void destroyBuffer();

    GLuint m_buffer;
    glm::mat4x4 m_view, m_projection;
    static GLuint SIZE, VIEW_OFFSET, PROJECTION_OFFSET;
};

#endif
