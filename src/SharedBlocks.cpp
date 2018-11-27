// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <limits>
#include "opengl.h"
#include "glm/mat4x4.hpp"
#include "SharedBlocks.h"

GLuint ViewAndProjectionBlock::VIEW_OFFSET = std::numeric_limits<GLuint>::max();
GLuint ViewAndProjectionBlock::PROJECTION_OFFSET = std::numeric_limits<GLuint>::max();

ViewAndProjectionBlock::ViewAndProjectionBlock()
    : m_view{1.0},
      m_projection{1.0}
{}

ViewAndProjectionBlock::ViewAndProjectionBlock(glm::mat4x4 &view, glm::mat4x4 &projection)
    : m_view{view},
      m_projection{projection}
{}

ViewAndProjectionBlock::~ViewAndProjectionBlock() {}

void ViewAndProjectionBlock::setOffsets(GLuint program, const char *block_name) {
    // Get the index of this uniform block.
    GLuint block_index = glGetUniformBlockIndex(program, block_name);

    // Get how many uniforms are in this block.    
    GLint num_unifs;
    glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_unifs);

    // Get the indices of the uniforms in this block. OpenGL gives you the
    // indices as ints, but wants them back as unsigned ints. Of course.
    GLint *unif_indices_int = new GLint[num_unifs];
    GLuint *unif_indices = new GLuint[num_unifs];
    glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, unif_indices_int);
    for (int i = 0; i < num_unifs; ++i) {
        unif_indices[i] = unif_indices_int[i];
    }

    // Get the offsets of the uniforms in the block.
    GLint *unif_offs = new GLint[num_unifs];
    glGetActiveUniformsiv(program, num_unifs, unif_indices, GL_UNIFORM_OFFSET, unif_offs);

    // Assume view comes first and projection second.
    VIEW_OFFSET = unif_offs[0];
    PROJECTION_OFFSET = unif_offs[1];

    // Cleanup.
    delete[] unif_indices_int;
    delete[] unif_indices;
    delete[] unif_offs;
}

void ViewAndProjectionBlock::setView(const glm::mat4x4 &new_view) {
    m_view = new_view;
}

const glm::mat4x4& ViewAndProjectionBlock::view() const {
    return m_view;
}

void ViewAndProjectionBlock::setProjection(const glm::mat4x4 &new_proj) {
    m_projection = new_proj;
}

const glm::mat4x4& ViewAndProjectionBlock::projection() const {
    return m_projection;
}

// void ViewAndProjectionBlock::writeToBuffer(GLuint buffer) {
// }
