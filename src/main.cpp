#include "Mesh.hpp"
#include "OpenGLExtensions.hpp"
#include "Shader.hpp"
#include "ShaderSource.hpp"
#include "extern/stb_image.hpp"

// OpenGL Start
#include <GL/glew.h>
// glew must be imported before glfw3
#include <GLFW/glfw3.h>
// OpenGL End

#include <glm/glm.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std::chrono;

float  deltaTime  = 0.1f;
float  mouseXDiff = 0.0f, mouseYDiff = 0.0f;
double lastMouseX = 0.0, lastMouseY = 0.0;
bool   firstUpdate = true;

float GetTime(steady_clock::time_point start, steady_clock::time_point now)
{
    steady_clock::duration duration = now - start;
    return float(duration.count()) * steady_clock::period::num /
           steady_clock::period::den;
}

float GetTime(steady_clock::time_point start)
{
    return GetTime(start, steady_clock::now());
}

static void MousePosEvent(GLFWwindow *window, double xpos, double ypos)
{
    if (firstUpdate)
    {
        firstUpdate = false;
        lastMouseX  = xpos;
        lastMouseY  = ypos;
    }
    mouseXDiff = xpos - lastMouseX;
    mouseYDiff = ypos - lastMouseY;
    lastMouseX = xpos;
    lastMouseY = ypos;
}

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
    // GLCall(glBindVertexArray(0));
    // GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // Probably not needed
    // GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    GLCall(glUseProgram(0));

#pragma region Render Data

    // Projection matrix
    // glm::mat4 projection =
    //     glm::ortho(0.0f, (GLfloat)1, (GLfloat)1, 0.0f, 0.1f, 100.0f);
    glm::mat4 projection = glm::perspective(
        45.0f, (GLfloat)bufferWidth / (GLfloat)bufferHeight, 0.1f, 100.0f);

    std::vector<Mesh> meshes = std::vector<Mesh>();

    Mesh cubeMesh = Mesh();
    cubeMesh.CreateMesh({Vertex(1.f, 1.f, 1.f, 0.0f, 0.0f),    // 0
                         Vertex(-1.f, 1.f, 1.f, 0.0f, 0.0f),   // 1
                         Vertex(-1.f, 1.f, -1.f, 0.0f, 0.0f),  // 2
                         Vertex(1.f, 1.f, -1.f, 0.0f, 0.0f),   // 3
                         Vertex(1.f, -1.f, 1.f, 0.0f, 0.0f),   // 4
                         Vertex(-1.f, -1.f, 1.f, 0.0f, 0.0f),  // 5
                         Vertex(-1.f, -1.f, -1.f, 0.0f, 0.0f), // 6
                         Vertex(1.f, -1.f, -1.f, 0.0f, 0.0f)}, // 7
                        {0, 1, 3, 3, 1, 2, 2, 6, 7, 7, 3, 2, 7, 6, 5, 5, 4, 7,
                         5, 1, 4, 4, 1, 0, 4, 3, 7, 3, 4, 0, 5, 6, 2, 5, 1, 2});

    meshes.push_back(std::move(cubeMesh));

#pragma endregion

    // Model matrix (Where the object's position is defined)
    glm::mat4 model =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    glm::mat4 view =
        glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 camPitch = glm::mat4(1.0f);
    glm::mat4 camYaw   = glm::mat4(1.0f);

    auto lastTimePoint = steady_clock::now();

    // Hide and lock the cursor, and prep it for motion-based detection
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Use raw mouse motion input
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetCursorPosCallback(window, MousePosEvent);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events
        GLCall(glfwPollEvents());

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_TRUE)
            break;

        // Calculate the delta-time for this frame
        auto now      = steady_clock::now();
        deltaTime     = GetTime(lastTimePoint, now);
        lastTimePoint = now;

        // Render here
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        //--- Binding ---//
        shaders->at(0).SetInUse();

        //--- Setup for draw ---//
        GLint colorUniformLoc = shaders->at(0).GetUniformLocation("u_Color");
        if (colorUniformLoc >= 0)
        {
            GLCall(glUniform4f(colorUniformLoc, 0.8f, 0.3f, 0.2f, 1.0f));
        }

        // Rotate the model
        // model = glm::rotate(model, 0.005f, glm::vec3(1.0f, 1.0f, 1.0f));

        // Update view matrix based on user input
        // View matrix position
        float cameraSpeed = 8.0f, cameraSensitivity = 0.005f;

        // View matrix rotation
        // camPitch *= glm::rotate(glm::mat4(1.0f), mouseYDiff *
        // cameraSensitivity,
        //                         glm::vec3(1.0f, 0.0f, 0.0f));
        // camYaw *= glm::rotate(glm::mat4(1.0f), mouseXDiff *
        // cameraSensitivity,
        //                       glm::vec3(0.0f, 1.0f, 0.0f));

        auto camPitchDiff =
            glm::rotate(glm::mat4(1.0f), mouseYDiff * cameraSensitivity,
                        glm::vec3(1.0f, 0.0f, 0.0f));
        auto camYawDiff =
            glm::rotate(glm::mat4(1.0f), mouseXDiff * cameraSensitivity,
                        glm::vec3(0.0f, 1.0f, 0.0f));

        // View matrix position
        glm::vec3 moveVec = glm::vec3(0.0f, 0.0f, 0.0f);
        // Forward/Backward
        if (glfwGetKey(window, GLFW_KEY_W) == GL_TRUE)
            moveVec.z = 1.0f;
        else if (glfwGetKey(window, GLFW_KEY_S) == GL_TRUE)
            moveVec.z = -1.0f;

        // Right/Left
        if (glfwGetKey(window, GLFW_KEY_D) == GL_TRUE)
            moveVec.x = -1.0f;
        else if (glfwGetKey(window, GLFW_KEY_A) == GL_TRUE)
            moveVec.x = 1.0f;

        // Up/Down
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GL_TRUE)
            moveVec.y = -1.0f;
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GL_TRUE)
            moveVec.y = 1.0f;

        glm::vec3 viewScale;
        glm::quat viewRotation;
        glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 viewSkew;
        glm::vec4 viewPerspective;

        glm::decompose(view, viewScale, viewRotation, viewPos, viewSkew,
                       viewPerspective);

        glm::mat4 viewTransform = glm::translate(glm::mat4(1.0f), -viewPos);
        viewTransform           = viewTransform * glm::toMat4(viewRotation);
        glm::mat4 inverseViewTransform = glm::inverse(viewTransform);

        view = (viewTransform * camYawDiff * inverseViewTransform) * view;
        view = camPitchDiff * view;
        view =
            glm::translate(glm::mat4(1.0f), moveVec * cameraSpeed * deltaTime) *
            view;
        // view = glm::translate(view, moveVec * cameraSpeed * deltaTime);

        // Reset the diffs
        mouseXDiff = mouseYDiff = 0.0f;

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

        // Assign the view matrix
        GLint viewUniformLoc = shaders->at(0).GetUniformLocation("view");
        if (viewUniformLoc >= 0)
        {
            GLCall(
                glUniformMatrix4fv(viewUniformLoc, 1, GL_FALSE,
                                   glm::value_ptr(camYaw * camPitch * view)));
        }

        //--- Drawing ---//
        // The actual draw call
        for (std::size_t i = 0; i < meshes.size(); i++)
            meshes[i].RenderMesh();

        // Swap front and back buffers
        GLCall(glfwSwapBuffers(window));
    }

    for (std::size_t i = 0; i < meshes.size(); i++)
        meshes[i].ClearMesh();

    GLCall(glfwTerminate());
    return 0;
}