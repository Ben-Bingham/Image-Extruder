#include <iostream>

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include <glm/ext/matrix_clip_space.hpp>

#include <imgui.h>
#include <implot.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <utility/OpenGl/Shader.h>
#include <utility/OpenGl/VertexAttributeObject.h>
#include <utility/OpenGl/Buffer.h>
#include <utility/OpenGl/GLDebug.h>
#include <utility/OpenGl/RenderTarget.h>

#include <utility/Camera.h>
#include <utility/TimeScope.h>
#include <utility/Transform.h>

#include "OrthoCamera.h"
#include "Image.h"
#include "Mesh.h"
#include "ImageExtruder.h"

using namespace RenderingUtilities;

void glfwErrorCallback(int error, const char* description) {
    std::cout << "ERROR: GLFW has thrown an error: " << std::endl;
    std::cout << description << std::endl;
}

glm::ivec2 mousePosition{ };
void mouseMoveCallback(GLFWwindow* window, double x, double y) {
    mousePosition.x = static_cast<int>(x);
    mousePosition.y = static_cast<int>(y);
}

void MoveCamera(Camera& camera, GLFWwindow* window, float dt, const glm::ivec2& mousePositionWRTViewport, const glm::ivec2& viewportSize, bool mouseOverViewport) {
    static bool mouseDown{ false };
    static bool hasMoved{ false };
    static glm::ivec2 lastMousePosition{ };

    if (!hasMoved) {
        lastMousePosition = mousePositionWRTViewport;
        hasMoved = true;
    }

    bool positionChange{ false };
    bool directionChange{ false };
    const float velocity = camera.speed * dt;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.frontVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.frontVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.rightVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.rightVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.upVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.upVector * velocity;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (mouseDown == false) {
            lastMousePosition.x = mousePositionWRTViewport.x;
            lastMousePosition.y = mousePositionWRTViewport.y;
        }

        mouseDown = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        mouseDown = false;
    }

    if (!mouseOverViewport) {
        hasMoved = false;
    }

    if (mouseDown && mouseOverViewport) {
        const float xDelta = (float)mousePositionWRTViewport.x - (float)lastMousePosition.x;
        const float yDelta = (float)lastMousePosition.y - (float)mousePositionWRTViewport.y;

        camera.yaw += xDelta * camera.lookSensitivity;
        camera.pitch += yDelta * camera.lookSensitivity;

        if (camera.pitch > 89.9f) {
            camera.pitch = 89.9f;
        }
        else if (camera.pitch < -89.9f) {
            camera.pitch = -89.9f;
        }

        directionChange = true;
    }

    if (mouseDown && mouseOverViewport) {
        camera.frontVector.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.frontVector.y = sin(glm::radians(camera.pitch));
        camera.frontVector.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.frontVector = glm::normalize(camera.frontVector);

        camera.rightVector = glm::normalize(glm::cross(camera.frontVector, camera.upVector));

        lastMousePosition.x = mousePositionWRTViewport.x;
        lastMousePosition.y = mousePositionWRTViewport.y;
    }
}

void MoveCamera2D(OrthoCamera& camera, GLFWwindow* window, float dt, const glm::ivec2& mousePositionWRTViewport, const glm::ivec2& viewportSize, bool mouseOverViewport) {
    static bool mouseDown{ false };
    static bool hasMoved{ false };
    static glm::ivec2 lastMousePosition{ };

    if (!hasMoved) {
        lastMousePosition = mousePositionWRTViewport;
        hasMoved = true;
    }

    bool positionChange{ false };
    bool directionChange{ false };
    const float velocity = camera.speed * dt;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.upVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.upVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        positionChange = true;
        camera.position += camera.rightVector * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        positionChange = true;
        camera.position -= camera.rightVector * velocity;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (mouseDown == false) {
            lastMousePosition.x = mousePositionWRTViewport.x;
            lastMousePosition.y = mousePositionWRTViewport.y;
        }

        mouseDown = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE) {
        mouseDown = false;
    }

    if (!mouseOverViewport) {
        hasMoved = false;
    }
}

int main() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit()) {
        std::cout << "ERROR: Failed to initialize GLFW." << std::endl;
    }

    glm::ivec2 windowSize{ 1600, 900 };
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, "Rutile", nullptr, nullptr);

    if (!window) {
        std::cout << "ERROR: Failed to create window." << std::endl;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR: Failed to initialize GLEW." << std::endl;
    }

    glfwSetCursorPosCallback(window, mouseMoveCallback);

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.FontGlobalScale = 2.0f;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glm::ivec2 defaultFramebufferSize{ 800, 600 };
    glm::ivec2 lastFrame3DViewportSize{ defaultFramebufferSize };
    glm::ivec2 lastFrame2DViewportSize{ defaultFramebufferSize };

    RenderTarget rendererTarget3D{ defaultFramebufferSize };
    RenderTarget renderTarget2D{ defaultFramebufferSize };

    Shader solidShader{
        "assets\\shaders\\solid.vert",
        "assets\\shaders\\solid.frag"
    };

    Shader imageShader{
        "assets\\shaders\\texture.vert",
        "assets\\shaders\\texture.frag"
    };

    Camera camera{ };
    OrthoCamera camera2D{ };

    VertexAttributeObject imageVAO{ };

    VertexBufferObject imageVBO{ std::vector<float>{
        -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f,       0.0f, 1.0f,
         0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f,       1.0f, 0.0f
    } };

    ElementBufferObject imageEBO{ std::vector<unsigned int>{
        2, 1, 0,
        3, 2, 0
    } };

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    imageVAO.Unbind();
    imageVBO.Unbind();
    imageEBO.Unbind();

    // TODO image aspect ratio
    Transform imageTransform{ };
    imageTransform.position = glm::vec3{ 0.0f, 0.0f, 5.0f };
    imageTransform.scale *= 10.0f;

    TextureParameters parameters{ };
    parameters.magFilter = TextureFilteringMode::NEAREST;
    parameters.minFilter = TextureFilteringMode::NEAREST;

    Image image{ "assets\\blackWhite.png" };
    Texture2D imageTexture{ "assets\\blackWhite.png", parameters };

    Mesh mesh = ExtrudeImage(image);

    std::cout << "Index count: " << mesh.indices.size() << std::endl;

    VertexAttributeObject VAO3D{ };

    VertexBufferObject VBO3D{ mesh.vertices };

    ElementBufferObject EBO3D{ mesh.indices };

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    VAO3D.Unbind();
    VBO3D.Unbind();
    EBO3D.Unbind();

    Transform transform{ };
    transform.position = glm::vec3{ 0.0f, 0.0f, 5.0f };

    std::chrono::duration<double> frameTime{ };
    std::chrono::duration<double> renderTime3D{ };
    std::chrono::duration<double> renderTime2D{ };

    bool mouseOver3DViewPort{ false };
    glm::ivec2 viewportOffset3D{ 0, 0 };

    bool mouseOver2DViewPort{ false };
    glm::ivec2 viewportOffset2D{ 0, 0 };

    while (!glfwWindowShouldClose(window)) {
        TimeScope frameTimeScope{ &frameTime };

        glfwPollEvents();

        glm::ivec2 mousePositionWRT3DViewport{ mousePosition.x - viewportOffset3D.x, lastFrame3DViewportSize.y - (viewportOffset3D.y - mousePosition.y) };
        glm::ivec2 mousePositionWRT2DViewport{ mousePosition.x - viewportOffset2D.x, lastFrame2DViewportSize.y - (viewportOffset2D.y - mousePosition.y) };

        if (mouseOver3DViewPort) {
            MoveCamera(camera, window, static_cast<float>(frameTime.count()), mousePositionWRT3DViewport, lastFrame3DViewportSize, mouseOver3DViewPort);
        }

        if (mouseOver2DViewPort) {
            MoveCamera2D(camera2D, window, static_cast<float>(frameTime.count()), mousePositionWRT3DViewport, lastFrame3DViewportSize, mouseOver3DViewPort);
        }

        { // 3D rendering
            TimeScope renderingTimeScope{ &renderTime3D };

            rendererTarget3D.Bind();

            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            solidShader.Bind();
            solidShader.SetVec3("color", glm::vec3{ 1.0f, 0.0f, 0.0f });

            glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)rendererTarget3D.GetSize().x / (float)rendererTarget3D.GetSize().y, camera.nearPlane, camera.farPlane);
            transform.CalculateMatrix();
            glm::mat4 mvp = projection * camera.View() * transform.matrix;

            solidShader.SetMat4("mvp", mvp);

            VAO3D.Bind();
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);

            rendererTarget3D.Unbind();
        }

        { // 2D rendering
            TimeScope renderingTimeScope{ &renderTime2D };

            renderTarget2D.Bind();

            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            imageShader.Bind();
            glActiveTexture(GL_TEXTURE0);
            imageTexture.Bind();

            imageShader.SetInt("image", 0);

            glm::mat4 projection = glm::ortho(camera2D.left, camera2D.right, camera2D.bottom, camera2D.top, camera2D.nearPlane, camera2D.farPlane);
            imageTransform.CalculateMatrix();
            glm::mat4 mvp = projection * camera2D.View() * imageTransform.matrix;

            imageShader.SetMat4("mvp", mvp);

            imageVAO.Bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            renderTarget2D.Unbind();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //ImGui::ShowDemoWindow();
        //ImGui::ShowMetricsWindow();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // Keep track of this so that we can make these changes after the imgui frame is finished
        size_t changedPointLightIndex{ 0 };
        bool pointLightChanged{ false };

        glm::ivec2 new3DViewportSize{ };

        { ImGui::Begin("3D-Viewport");
            // Needs to be the first call after "Begin"
            new3DViewportSize = glm::ivec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

            // Display the frame with the last frames viewport size (The same size it was rendered with)
            ImGui::Image((ImTextureID)rendererTarget3D.GetTexture().Get(), ImVec2{ (float)lastFrame3DViewportSize.x, (float)lastFrame3DViewportSize.y }, ImVec2{ 0.0f, 1.0f }, ImVec2{ 1.0f, 0.0f });

            mouseOver3DViewPort = ImGui::IsItemHovered();

            viewportOffset3D = glm::ivec2{ (int)ImGui::GetCursorPos().x, (int)ImGui::GetCursorPos().y };

        } ImGui::End(); // Viewport

        glm::ivec2 new2DViewportSize{ };

        { ImGui::Begin("2D-Viewport");
            new2DViewportSize = glm::ivec2{ ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

            ImGui::Image((ImTextureID)renderTarget2D.GetTexture().Get(), ImVec2{(float)lastFrame2DViewportSize.x, (float)lastFrame2DViewportSize.y}, ImVec2{0.0f, 1.0f}, ImVec2{1.0f, 0.0f});
        
            mouseOver2DViewPort = ImGui::IsItemHovered();

            viewportOffset2D = glm::ivec2{ (int)ImGui::GetCursorPos().x, (int)ImGui::GetCursorPos().y };
        } ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        const ImGuiIO& io = ImGui::GetIO();

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* currentContextBackup = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(currentContextBackup);
        }

        // After ImGui has rendered its frame, we resize the framebuffer if needed for next frame
        if (new3DViewportSize != lastFrame3DViewportSize) {
            rendererTarget3D.Resize(new3DViewportSize);
        }

        if (new2DViewportSize != lastFrame2DViewportSize) {
            renderTarget2D.Resize(new2DViewportSize);
        }

        lastFrame3DViewportSize = new3DViewportSize;
        lastFrame2DViewportSize = new2DViewportSize;

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwTerminate();
}