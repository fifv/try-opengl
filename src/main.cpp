#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <print>

auto main() -> int {
    std::println("hi");
    using namespace gl;

    glfwSetErrorCallback([](int error, const char *description) {
        /*  */
        fprintf(stderr, "Error: %s\n", description);
    });
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(1280, 720, "OpenGL Triangle", NULL, NULL);

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });
    glfwMakeContextCurrent(window);
    glbinding::initialize(glfwGetProcAddress);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        static auto count = 0;
        count++;
        std::print("Frame x{}\r", count);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    std::println("");

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}