set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)
target_link_libraries(glad PUBLIC OpenGL::GL)
