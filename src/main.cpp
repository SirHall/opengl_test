#include "OpenGLExtensions.hpp"
#include "Shader.hpp"
#include "ShaderSource.hpp"

// OpenGL Start
#include <GL/glew.h>
// glew must be imported before glfw3
#include <GLFW/glfw3.h>
// OpenGL End

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

int main(int argc, char *argv[])
{

#pragma region Setup GLEW and GLFW
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW, quitting." << std::endl;
        GLCall(glfwTerminate());
        return -1;
    }

    // Tell OpenGL to use GLSL 3.30
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3));
    GLCall(glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3));
    GLCall(glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE));
    GLCall(glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE));

    // Create a windowed mode window and its OpenGL context
    GLCall(GLFWwindow *window =
               glfwCreateWindow(640, 640, "Hello World", NULL, NULL));

    if (!window)
    {
        std::cerr << "Failed to create window" << std::endl;
        GLCall(glfwTerminate());
        return -1;
    }

    int bufferWidth = 0, bufferHeight = 0;
    // Get window buffer size
    GLCall(glfwGetFramebufferSize(window, &bufferWidth, &bufferHeight));

    // Make the window's context current
    GLCall(glfwMakeContextCurrent(window));

    // Allow experimental features
    glewExperimental = GL_TRUE;

    // Turn VSync on
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    { /* Problem: glewInit failed, something is seriously wrong. */
        GLCall(fprintf(stderr, "Error: %s\n", glewGetErrorString(err)));
        GLCall(glfwDestroyWindow(window));
        GLCall(glfwTerminate());
        return 1;
    }

    // Depth buffer
    glEnable(GL_DEPTH_TEST);

    // Setup viewport
    glViewport(0, 0, bufferWidth, bufferHeight);

    GLCall(fprintf(stdout, "Status: Using GLEW %s\n",
                   glewGetString(GLEW_VERSION)));

#pragma endregion

    //--- OpenGL Code starts here ---//

    GLCall(std::cout << "OpenGL Version: " << glGetString(GL_VERSION)
                     << std::endl);

    // Setup buffer
    std::vector<float> positions = {
        -0.5f, -0.5f, // 0
        0.5f,  -0.5f, // 1
        0.5f,  0.5f,  // 2
        -0.5f, 0.5f   // 3
    };

    std::vector<unsigned short> indices = {0, 1, 2, 0, 2, 3};

    // Vertex buffer
    unsigned int buffer = 0;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
                        positions.data(), GL_STATIC_DRAW));

    // Setup vertex array
    GLuint vertexArrayObject = 0;
    GLCall(glGenVertexArrays(1, &vertexArrayObject));
    GLCall(glBindVertexArray(vertexArrayObject));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

    // Index buffer
    GLuint indexBufferObject = 0;
    GLCall(glGenBuffers(1, &indexBufferObject));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        indices.size() * sizeof(unsigned short), indices.data(),
                        GL_STATIC_DRAW));

#pragma region Compile Shaders

    auto shaderSources       = ShaderSource::ReadShaderSources("res/");
    auto shaderCompileErrors = std::make_unique<std::vector<std::string>>();
    auto shaders             = Shader::CompileShaders(
        shaderSources, shaderCompileErrors); // Compile(shaderSources);

    if (shaderCompileErrors->size() > 0)
    {
        for (auto error : *shaderCompileErrors)
            std::cout << error << std::endl;
        return 1;
    }
    else
    {
        std::cout << "All shaders compiled successfully" << std::endl;
    }

#pragma endregion

    // Unbind all
    GLCall(glBindVertexArray(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // Probably not needed
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GLCall(glUseProgram(0));

#pragma region Render Data

    // Projection matrix
    glm::mat4 projection =
        glm::ortho(0.0f, (GLfloat)1, (GLfloat)1, 0.0f, 0.1f, 100.0f);
    //  glm::perspective(
    //     45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f,
    //     100.0f);

#pragma endregion

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events
        GLCall(glfwPollEvents());

        // Render here
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        //--- Binding ---//
        shaders->at(0).SetInUse();

        GLCall(glBindVertexArray(vertexArrayObject));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer)); // Probably not needed
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));

        //--- Setup for draw ---//
        GLint colorUniformLoc = shaders->at(0).GetUniformLocation("u_Color");
        if (colorUniformLoc >= 0)
        {
            GLCall(glUniform4f(colorUniformLoc, 0.8f, 0.3f, 0.2f, 1.0f));
        }
        // Model matrix (Where the object's position is defined)
        glm::mat4 model =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));

        // Assign the object model to the uniform (Not good for more than
        // one object)
        GLint modelUniformLoc = shaders->at(0).GetUniformLocation("model");
        if (modelUniformLoc >= 0)
        {
            GLCall(glUniformMatrix4fv(modelUniformLoc, 1, GL_FALSE,
                                      glm::value_ptr(model)));
        }
        // Assign the projection matrix uniform its value
        GLint projectionUniformLoc =
            shaders->at(0).GetUniformLocation("projection");
        if (projectionUniformLoc >= 0)
        {
            GLCall(glUniformMatrix4fv(projectionUniformLoc, 1, GL_FALSE,
                                      glm::value_ptr(projection)));
        }

        //--- Drawing ---//
        // The actual draw call
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr));

        // Swap front and back buffers
        GLCall(glfwSwapBuffers(window));
    }

    GLCall(glfwTerminate());
    return 0;
}