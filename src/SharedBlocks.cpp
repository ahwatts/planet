// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <cstring>
#include <limits>
#include <regex>
#include <string>
#include <vector>

#include "opengl.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SharedBlocks.h"

const GLuint MAX_GLUINT = std::numeric_limits<GLuint>::max();

const GLuint ViewAndProjectionBlock::BINDING_INDEX = 0;
const GLuint LightListBlock::BINDING_INDEX = 1;

GLuint ViewAndProjectionBlock::SIZE = 0;
GLuint ViewAndProjectionBlock::VIEW_OFFSET = MAX_GLUINT;
GLuint ViewAndProjectionBlock::VIEW_INV_OFFSET = MAX_GLUINT;
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
        } else if (std::strncmp("view_inv", name, name_len) == 0) {
            VIEW_INV_OFFSET = unif_offs[i];
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

    glm::mat4x4 view_inv = glm::inverse(m_view);

    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    uint8_t *data = static_cast<uint8_t*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY));
    std::memcpy(data + VIEW_OFFSET, glm::value_ptr(m_view), sizeof(m_view));
    std::memcpy(data + VIEW_INV_OFFSET, glm::value_ptr(view_inv), sizeof(view_inv));
    std::memcpy(data + PROJECTION_OFFSET, glm::value_ptr(m_projection), sizeof(m_projection));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void ViewAndProjectionBlock::bind() const {
    glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_INDEX, m_buffer);
}

void ViewAndProjectionBlock::unbind() const {
    glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_INDEX, 0);
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

LightInfo::LightInfo()
    : enabled{GL_FALSE},
      direction{0.0, 0.0, 0.0}
{}

LightInfo::~LightInfo() {}

GLuint LightListBlock::SIZE = 0;
GLuint LightListBlock::NUM_LIGHTS = 0;
std::vector<LightOffsetInfo> LightListBlock::OFFSETS{};

LightListBlock::LightListBlock()
    : m_buffer{0},
      m_light_info{NUM_LIGHTS}
{}

LightListBlock::~LightListBlock() {
    destroyBuffer();
}

void LightListBlock::setOffsets(GLuint program, const char *block_name) {
    std::regex light_field_name("^lights\\[(\\d+)\\]\\.(\\w+)$");
    std::cmatch light_field_parts;

    // Get the index of this uniform block.
    GLuint block_index = glGetUniformBlockIndex(program, block_name);

    // Get the size of the buffer necessary for the block.
    GLint block_size;
    glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &block_size);
    SIZE = block_size;

    // Allocate a buffer for the names of the uniforms in the block.
    GLint max_name_len;
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
    char *name = new char[max_name_len];

    // Get how many uniforms are in this block.
    GLint num_unifs;
    glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_unifs);

    // We know there are 2 uniforms per array element, so set the size of the array.
    NUM_LIGHTS = num_unifs / 2;
    OFFSETS.resize(NUM_LIGHTS);

    // Get the indices of the uniforms in this block. OpenGL gives you the
    // indices as ints, but wants them back as unsigned ints. Of course.
    GLint *unif_indices_int = new GLint[num_unifs];
    GLuint *unif_indices = new GLuint[num_unifs];
    GLint *unif_offs = new GLint[num_unifs];

    // Get the offsets of the uniforms in the block.
    glGetActiveUniformBlockiv(program, block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, unif_indices_int);
    for (int i = 0; i < num_unifs; ++i) { unif_indices[i] = unif_indices_int[i]; }
    glGetActiveUniformsiv(program, num_unifs, unif_indices, GL_UNIFORM_OFFSET, unif_offs);

    // Set up the OFFSETS vector.
    for (int i = 0; i < num_unifs; ++i) {
        glGetActiveUniformName(program, unif_indices[i], max_name_len, nullptr, name);
        if (std::regex_search(name, light_field_parts, light_field_name)) {
            int light_index = std::stoi(light_field_parts[1]);
            std::string field_str{light_field_parts[2]};

            if (field_str == "enabled") {
                OFFSETS[light_index].enabled = unif_offs[i];
            } else if (field_str == "direction") {
                OFFSETS[light_index].direction = unif_offs[i];
            // } else if (field_str == "color") {
            //     OFFSETS[light_index].color = unif_offs[i];
            // } else if (field_str == "specular_exp") {
            //     OFFSETS[light_index].specular_exp = unif_offs[i];
            }
        }
    }

    // Cleanup.
    delete[] unif_indices_int;
    delete[] unif_indices;
    delete[] unif_offs;
    delete[] name;
}


void LightListBlock::enableLight(unsigned int index, const glm::vec3 &direction) {
    m_light_info[index].enabled = GL_TRUE;
    m_light_info[index].direction = direction;
}

void LightListBlock::writeToBuffer() {
    if (m_buffer == 0) {
        createBuffer();
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    uint8_t *data = static_cast<uint8_t*>(glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE));

    for (unsigned int i = 0; i < NUM_LIGHTS; ++i) {
        std::memcpy(data + OFFSETS[i].enabled, &m_light_info[i].enabled, sizeof(m_light_info[i].enabled));
        std::memcpy(data + OFFSETS[i].direction, glm::value_ptr(m_light_info[i].direction), sizeof(m_light_info[i].direction));
        // std::memcpy(data + OFFSETS[i].color, glm::value_ptr(m_light_info[i].color), sizeof(m_light_info[i].color));
        // std::memcpy(data + OFFSETS[i].specular_exp, &m_light_info[i].specular_exp, sizeof(m_light_info[i].specular_exp));
    }

    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightListBlock::bind() const {
    glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_INDEX, m_buffer);
}

void LightListBlock::unbind() const {
    glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_INDEX, 0);
}

void LightListBlock::createBuffer() {
    if (m_buffer > 0) {
        destroyBuffer();
    }

    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer);
    glBufferData(GL_UNIFORM_BUFFER, SIZE, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightListBlock::destroyBuffer() {
    if (glIsBuffer(m_buffer)) {
        glDeleteBuffers(1, &m_buffer);
    }
    m_buffer = 0;
}
