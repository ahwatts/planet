// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cstring>
#include <limits>

#include "opengl.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "SharedBlocks.h"

const GLuint MAX_GLUINT = std::numeric_limits<GLuint>::max();

GLuint ViewAndProjectionBlock::SIZE = 0;
GLuint ViewAndProjectionBlock::VIEW_OFFSET = MAX_GLUINT;
GLuint ViewAndProjectionBlock::PROJECTION_OFFSET = MAX_GLUINT;

ViewAndProjectionBlock::ViewAndProjectionBlock()
    : m_buffer{0},
      m_view{1.0},
      m_projection{1.0}
{}

ViewAndProjectionBlock::ViewAndProjectionBlock(glm::mat4x4 &view, glm::mat4x4 &projection)
    : m_buffer{0},
      m_view{view},
      m_projection{projection}
{}

ViewAndProjectionBlock::~ViewAndProjectionBlock() {
    destroyBuffer();
}

void ViewAndProjectionBlock::setOffsets(GLuint program, const char *block_name) {
    // Get the index of this uniform block.
    GLuint block_index = glGetUniformBlockIndex(program, block_name);

    // Get the size of the buffer necessary for the block.
    GLint block_size;
    glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);
    SIZE = block_size;

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

    GLint name_len;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &name_len);
    char *name = new char[name_len];

    for (int i = 0; i < num_unifs; ++i) {
        glGetActiveUniformName(program, unif_indices[i], name_len, nullptr, name);
        if (std::strncmp("view", name, name_len) == 0) {
            VIEW_OFFSET = unif_offs[i];
        } else if (std::strncmp("projection", name, name_len) == 0) {
            PROJECTION_OFFSET = unif_offs[i];
        }
    }

    // Cleanup.
    delete[] unif_indices_int;
    delete[] unif_indices;
    delete[] unif_offs;
    delete[] name;
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

void ViewAndProjectionBlock::writeToBuffer() {
    if (m_buffer == 0) {
        createBuffer();
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    uint8_t *data = static_cast<uint8_t*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
    std::memcpy(data + VIEW_OFFSET, glm::value_ptr(m_view), sizeof(m_view));
    std::memcpy(data + PROJECTION_OFFSET, glm::value_ptr(m_projection), sizeof(m_projection));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ViewAndProjectionBlock::bindToIndex(GLuint index) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, m_buffer);
}

void ViewAndProjectionBlock::unbindIndex(GLuint index) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, index, 0);
}

void ViewAndProjectionBlock::createBuffer() {
    if (m_buffer > 0) {
        destroyBuffer();
        m_buffer = 0;
    }

    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    glBufferData(GL_UNIFORM_BUFFER, SIZE, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ViewAndProjectionBlock::destroyBuffer() {
    if (glIsBuffer(m_buffer)) {
        glDeleteBuffers(1, &m_buffer);
    }
    m_buffer = 0;
}
