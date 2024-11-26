#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "file_manager/file_manager.h"
#include "shader/shader.h"

void errorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

void createTriangle(unsigned int &vao, unsigned int &vbo, unsigned int &ebo)
{
    // Define the vertices and their associated colors for the triangle
    // Each vertex has a position (x, y, z) and a color (r, g, b)
    float triangleVertices[] = {
        0.0f, 1.0f, 0.0f,  // Position of vertex 1
        1.0f, 0.0f, 0.0f,  // Color of vertex 1 (Red)
        -1.f, -0.5f, 0.0f, // Position of vertex 2
        0.0f, 1.0f, 0.0f,  // Color of vertex 2 (Green)
        1.0f, -0.5f, 0.0f, // Position of vertex 3
        0.0f, 0.0f, 1.0f   // Color of vertex 3 (Blue)
    };

    // Define the indices that represent how to draw the triangle using the vertices
    unsigned int triangleIndices[] = {
        0, 1, 2 // The triangle consists of vertices 0, 1, and 2
    };

    // Generate a Vertex Array Object (VAO), Vertex Buffer Object (VBO),
    // and Element Buffer Object (EBO) to manage and store OpenGL state
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Bind the VAO to start recording the OpenGL state for this triangle
    glBindVertexArray(vao);

    // Bind the VBO to the GL_ARRAY_BUFFER target to store vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // Upload the vertex data to the GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    // Bind the EBO to the GL_ELEMENT_ARRAY_BUFFER target to store index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    // Upload the index data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices), triangleIndices, GL_STATIC_DRAW);

    // Specify the layout of the vertex data:
    // Attribute 0: Position (x, y, z) - 3 floats, starting at offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0); // Enable attribute 0

    // Attribute 1: Color (r, g, b) - 3 floats, starting at offset 3 * sizeof(float)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1); // Enable attribute 1

    // Unbind the VBO (optional, to prevent accidental modification of VBO data)
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Unbind the VAO (optional, to prevent accidental modification of VAO state)
    glBindVertexArray(0);
}

int main()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW error callback
    glfwSetErrorCallback(errorCallback);

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // OpenGL 4.1
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1); // OpenGL 4.1
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac

    // Create window
    GLFWwindow *window = glfwCreateWindow(1200, 900, "Virtual Universe!", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Create triangle
    unsigned int vao, vbo, ebo;
    createTriangle(vao, vbo, ebo);

    // Initialize shader
    Shader simpleShader;
    simpleShader.init(
        FileManager::read("shaders/simple.vs"), // Vertex shader source
        FileManager::read("shaders/simple.fs")  // Fragment shader source
    );

    // Get screen dimensions
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    // Camera setup
    glm::vec3 cameraPosition(0.f, 0.f, 2.f);       // Camera is placed at (0, 0, 2)
    glm::vec3 cameraViewDirection(0.f, 0.f, -1.f); // Camera looks towards the negative Z-axis

    // Model matrix
    glm::mat4 model(1.f); // Identity matrix, meaning no transformation on the model

    // View matrix
    glm::mat4 view = glm::lookAt(
        cameraPosition,                       // Camera position
        cameraPosition + cameraViewDirection, // Target position (camera looks here)
        glm::vec3(0.f, 1.f, 0.f)              // Up vector (positive Y-axis)
    );

    // Projection matrix
    glm::mat4 projection = glm::perspective(
        (float)M_PI_2,                            // Field of view (90 degrees in radians)
        (float)screenWidth / (float)screenHeight, // Aspect ratio (width / height)
        0.01f,                                    // Near clipping plane
        100.0f                                    // Far clipping plane
    );

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Get elapsed time since program launch
        float time = (float)glfwGetTime();

        // Vary color based on time
        float red = (std::sin(time * 0.5f) + 1.0f) / 4.0f;
        float green = (std::sin(time * 0.3f) + 1.0f) / 4.0f;
        float blue = (std::sin(time * 0.7f) + 1.0f) / 4.0f;

        // Clear color buffer
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(red, green, blue, 1.0f); // Use oscillating colors

        // Render the triangle
        {
            // Activate the shader to use it for rendering
            simpleShader.use();

            // Update model matrix for rotating the triangle on Z-axis
            model = glm::rotate(
                glm::mat4(1.f),
                std::sin(time * 0.8f) / 4.f,
                glm::vec3(0.f, 0.f, -1.f));

            // Set shader uniforms
            simpleShader.setMat4("u_model", model);
            simpleShader.setMat4("u_view", view);
            simpleShader.setMat4("u_projection", projection);

            // Bind the VAO that stores the triangle's vertex data and settings
            glBindVertexArray(vao);

            // Issue the draw command
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

            // Unbind the VAO
            glBindVertexArray(0);
        }

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup triangle
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    // Cleanup GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
