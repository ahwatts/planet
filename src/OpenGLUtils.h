// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef _PLANET_OPENGL_UTILS_H_
#define _PLANET_OPENGL_UTILS_H_

#include <map>
#include <string>
#include <vector>

#include "opengl.h"

typedef std::map<std::string, GLuint> IndexMap;

GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
GLuint createProgramFromShaders(GLuint vertex_shader, GLuint fragment_shader);
void getAttachedShaders(GLuint program, std::vector<GLuint> &shaders);
void getAttributeInfo(GLuint program, IndexMap &attributes);
void getUniformInfo(GLuint program, IndexMap &uniforms);
void getUniformBlockInfo(GLuint program, IndexMap &uniform_blocks);

int sizeOfGLType(GLenum type);
std::string translateGLType(GLenum type);
void dumpOpenGLState();
void dumpProgramAttributes(GLuint progid, const char *prefix);
void dumpProgramUniforms(GLuint progid, const char *prefix);

// void checkOpenGLError(const char *msg, bool throw_ex);

void APIENTRY handleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

#endif
