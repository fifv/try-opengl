#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <fmt/base.h>

auto createProgram() -> uint32_t {
    const auto vertexShader = gl::glCreateShader(gl::GLenum::GL_VERTEX_SHADER);
    const auto fragmentShader = gl::glCreateShader(gl::GLenum::GL_FRAGMENT_SHADER);
    const auto vertexShaderSource = R"(
        #version 460 core
        layout(location = 0) in vec4 myPosition;
        void main() {
            gl_Position = myPosition;
        }
    )";
    const auto fragmentShaderSource = R"(
        #version 460 core
        layout(location = 0) out vec4 myColor;
        void main() {
            myColor = vec4(1.0, 0.5, 0.0, 1.0);
        }
    )";

    gl::glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    gl::glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    gl::glCompileShader(vertexShader);
    gl::glCompileShader(fragmentShader);

    const auto program = gl::glCreateProgram();
    gl::glAttachShader(program, vertexShader);
    gl::glAttachShader(program, fragmentShader);
    gl::glLinkProgram(program);

    return program;
}

auto main() -> int {
    fmt::println("hi");
    // using namespace gl;

    glfwSetErrorCallback([](int error, const char *description) {
        /*  */
        fprintf(stderr, "Error: %s\n", description);
    });
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); /* seems not required? */
    GLFWwindow *window = glfwCreateWindow(1280, 720, "OpenGL Triangle", NULL, NULL);

    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    });
    glfwMakeContextCurrent(window);
    glbinding::initialize(glfwGetProcAddress);

    /**
     * all the boilerplate needed to enable error callback
     * 1. glDebugMessageCallback()
     * 2. glEnable(gl::GL_DEBUG_OUTPUT);
     */
    gl::glDebugMessageCallback(
        [](gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length, const gl::GLchar *message,
           const void *userParam) {
            /* */
            fprintf(
                stderr, "[GL%s] (type = 0x%x, severity = 0x%x) \n\t %s\n", (type == gl::GL_DEBUG_TYPE_ERROR) ? "/ERROR" : "",
                type, severity, message
            );
        },
        nullptr
    );
    glEnable(gl::GL_DEBUG_OUTPUT);

    /* setup finished, now do the actual drawing */


    /**
     * each attribute's elements seem should be same type, and number of elements should be 1,2,3,4
     */
    /**
     * opengl coordinates are from left-bottom to right-top
     */
    struct VertexAttributePosition {
        float x;
        float y;
    };
    struct VertexAttributeDummy {
        int w;
        int t;
        int f;
    };
    /**
     * each vertex contains multiple attributes
     */
    struct VertexAttributes {
        VertexAttributeDummy dummy_;
        VertexAttributePosition position;
    };

    auto triangleBuffer = std::to_array<VertexAttributes>({
        {{1, 2, 3}, {-0.5f, +0.5f}},
        {{1, 2, 3}, {+0.5f, +0.5f}},
        {{1, 2, 3}, {+0.5f, -0.5f}},
        {{1, 2, 3}, {-0.5f, -0.5f}},
        {{1, 2, 3}, {-0.5f, +0.5f}},
        {{1, 2, 3}, {+0.5f, +0.5f}},
    });

    auto vertexArray = ([]() {
        auto vertexArray = 0u;
        gl::glGenVertexArrays(1, &vertexArray);
        gl::glBindVertexArray(vertexArray);
        return vertexArray;
    })();

    auto buffer = ([&]() {
        auto buffer = 0u;
        gl::glGenBuffers(1, &buffer);
        /**
         * GL_ARRAY_BUFFER etc. is a "binding target"
         * there are limited number of "binding target"
         * bind to one target won't affect other targets
         * e.g. bina bufferA to GL_ARRAY_BUFFER and bufferB to GL_ELEMENT_ARRAY_BUFFER can happen at same time, and they are
         * independent
         */
        gl::glBindBuffer(gl::GLenum::GL_ARRAY_BUFFER, buffer);
        gl::glBufferData(gl::GLenum::GL_ARRAY_BUFFER, sizeof(triangleBuffer), &triangleBuffer, gl::GLenum::GL_DYNAMIC_DRAW);
        /**
         * the last param, `pointer`, is an offset, in Bytes, from the vertex attributes beginning to the data
         * can be used to skip some paddings
         */
        gl::glVertexAttribPointer(
            0, 2, gl::GLenum::GL_FLOAT, false, sizeof(VertexAttributes), (const void *)(offsetof(VertexAttributes, position))
        );
        gl::glEnableVertexArrayAttrib(vertexArray, 0);

        return buffer;
    })();


    auto indexBuffer = ([]() {
        auto indexBufferData = std::to_array({0u, 1u, 2u, 2u, 3u, 1u});
        auto indexBuffer = 0u;
        gl::glGenBuffers(1, &indexBuffer);
        gl::glBindBuffer(gl::GLenum::GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        gl::glBufferData(
            gl::GLenum::GL_ELEMENT_ARRAY_BUFFER, sizeof(indexBufferData), indexBufferData.data(), gl::GLenum::GL_DYNAMIC_DRAW
        );
        return indexBuffer;
    })();

    const auto program = createProgram();
    gl::glUseProgram(program);

    while (!glfwWindowShouldClose(window)) {
        glClear(gl::GL_COLOR_BUFFER_BIT);

        static auto count = 0;
        count++;
        fmt::print("Frame x{}\r", count);

        // gl::glDrawElements(gl::GLenum::GL_TRIANGLES, 1, GLenum type, const void *indices)

        /**
         * glDrawArrays() seems == glDrawElements() with indexBuffer is [0,1,2,...,count-1]
         * `count` is how much indices in the indexBuffer are used,
         * e.g. 3 is one triangle, 6 is two, 5 is one and last 2 vertices are discarded
         *
         */
        gl::glDrawArrays(gl::GLenum::GL_TRIANGLES, count % 6, 6);
        /* works! */
        // gl::glDrawElements(gl::GLenum::GL_TRIANGLES, 6, gl::GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    fmt::println("");

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}