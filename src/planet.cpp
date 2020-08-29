// -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; -*-

#include <iostream>
#include <string>

#include "glm_defines.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "opengl.h"

#include "Curve.h"
#include "Noise.h"
#include "Ocean.h"
#include "OpenGLUtils.h"
#include "SharedBlocks.h"
#include "Terrain.h"

void bailout(const std::string &msg);
void handleGlfwError(int code, const char *desc);
void initGlad();
void initGlfw(int width, int height, const char *title, GLFWwindow **window);
void initOpenGL();
void keypress(GLFWwindow *window, int key, int scancode, int action, int mods);
void runMainLoop(GLFWwindow *window);

const int WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 768;
const char *WINDOW_TITLE = "Planet Demo";

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100)
#endif
int main(int argc, char **argv) {
    GLFWwindow *window;
    initGlfw(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, &window);
    initGlad();
    initOpenGL();

    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << std::endl;

    runMainLoop(window);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

void bailout(const std::string &msg) {
    std::cerr << msg << std::endl;
    glfwTerminate();
    std::exit(1);
}

void handleGlfwError(int code, const char *desc) {
    std::cerr << "GLFW Error Code " << code << "\n"
              << desc << std::endl;
}

void initGlad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        bailout("Could not initialize GLAD");
    }
}

void initGlfw(int width, int height, const char *title, GLFWwindow **window) {
    glfwSetErrorCallback(handleGlfwError);
    if (!glfwInit()) {
        bailout("Could not initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(_DEBUG) || !defined(NDEBUG)
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    *window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!*window) {
        bailout("Could not create window");
    }

    glfwMakeContextCurrent(*window);
    glfwSetKeyCallback(*window, keypress);
}

void initOpenGL() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(handleDebugMessage, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}

void keypress(GLFWwindow *window, int key, int scancode, int action, int mode) {
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
    default:
        std::cout << "key: " << key
                  << " scancode: " << scancode
                  << " action: " << action
                  << " mode: " << mode
                  << std::endl;
    }
}

void runMainLoop(GLFWwindow *window) {
    const Perlin base_noise{2.0, 2.0, 2.0};
    const Octave octave_noise{base_noise, 4, 0.3};
    CubicSpline spline;
    spline
        .addControlPoint(-1.0, -1.0)
        .addControlPoint(-0.5, -0.5)
        .addControlPoint(0.0, -0.1)
        .addControlPoint(0.5, 0.8)
        .addControlPoint(0.75, 1.2)
        .addControlPoint(1.0, 1.2);
    const Curve curved_noise{octave_noise, spline};

    // CurveDisplay curve_disp{spline, -1.0, 1.0, -1.0, 1.0, 1000};
    Terrain terrain{curved_noise};
    Ocean ocean;

    ViewAndProjectionBlock vp_block{};
    static float angle = 0.0;
    glm::mat4x4 model{1.0};
    glm::mat4x4 view = glm::lookAt(
        glm::vec3{ 0.0, 0.0, 5.0 },
        glm::vec3{ 0.0, 0.0, 0.0 },
        glm::vec3{ 0.0, 1.0, 0.0 }
    );
    glm::mat4x4 projection = glm::perspectiveFov(
        20.0f, (float)WINDOW_WIDTH, (float)WINDOW_HEIGHT, 0.1f, 100.0f
    );
    vp_block.setProjection(projection);
    vp_block.setView(view);
    vp_block.writeToBuffer();

    LightListBlock light_block{};
    light_block.enableLight(0, glm::normalize(glm::vec3(-1.0, -1.0, -1.0)));
    light_block.writeToBuffer();

    while (!glfwWindowShouldClose(window)) {
        glm::mat4x4 model2 = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 1.0, 0.0));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vp_block.bind();
        light_block.bind();
        terrain.render(model2);
        ocean.render(model2);
        vp_block.unbind();
        light_block.unbind();

        // curve_disp.render();

        glfwSwapBuffers(window);

        glfwPollEvents();

        angle += 0.5;
        if (angle > 360.0) {
            angle = 0.0;
        }
    }
}
