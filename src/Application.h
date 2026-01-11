#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class Application
{
public:
    Application(int width, int height, const char* title);
    virtual ~Application();

    // Prevent copying
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // Main entry point
    int run();

    // Lifecycle virtual functions (override in derived classes)
    virtual void onInit() {}
    virtual void onUpdate(float deltaTime) { (void)deltaTime; }
    virtual void onRender() {}
    virtual void onImGui() {}
    virtual void onShutdown() {}

    // Called when window is resized
    virtual void onResize(int width, int height) { (void)width; (void)height; }

    // Called when mouse scrolls
    virtual void onScroll(float yoffset) { (void)yoffset; }

    // Input queries
    bool isKeyPressed(int key) const;
    bool isKeyDown(int key) const;
    bool isMouseButtonPressed(int button) const;
    glm::vec2 getMousePosition() const { return m_mousePos; }
    glm::vec2 getMouseDelta() const { return m_mouseDelta; }

    // Window state
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    float getAspectRatio() const;
    float getDeltaTime() const { return m_deltaTime; }
    GLFWwindow* getWindow() const { return m_window; }

    // Cursor control
    void setCursorEnabled(bool enabled);
    bool isCursorEnabled() const { return m_cursorEnabled; }

    // Clear color
    void setClearColor(const glm::vec4& color) { m_clearColor = color; }
    glm::vec4 getClearColor() const { return m_clearColor; }

protected:
    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;
    float m_deltaTime;
    float m_lastFrame;
    bool m_cursorEnabled;
    glm::vec4 m_clearColor;

    // Mouse state
    glm::vec2 m_mousePos;
    glm::vec2 m_lastMousePos;
    glm::vec2 m_mouseDelta;
    bool m_firstMouse;

private:
    bool initWindow();
    bool initOpenGL();
    bool initImGui();
    void shutdownImGui();
    void updateMouseState();

    // GLFW callbacks
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};

#endif
