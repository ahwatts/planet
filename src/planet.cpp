// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <iostream>
#include <string>

#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>

void bailout(const std::string &msg);
void handleGlfwError(int code, const char *desc);
void initGlfw(int width, int height, const char *title, GLFWwindow **window);

const int WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 768;
const char *WINDOW_TITLE = "Planet Demo";

int main(int argc, char **argv) {
    GLFWwindow *window;
    initGlfw(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, &window);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void bailout(const std::string &msg) {
    std::cerr << msg << std::endl;
    glfwTerminate();
    std::exit(1);
}

void handleGlfwError(int code, const char *desc) {
    std::cerr << "GLFW Error Code " << code << "\n"
              << desc << std::endl;
}

void initGlfw(int width, int height, const char *title, GLFWwindow **window) {
    glfwSetErrorCallback(handleGlfwError);
    if (!glfwInit()) {
        bailout("Could not initialize GLFW");
    }

    *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!*window) {
        bailout("Could not create window");
    }

    glfwMakeContextCurrent(*window);

    // std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    // std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    // std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    // std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;
}
