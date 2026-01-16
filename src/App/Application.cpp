#include "Application.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

Application::Application(int width, int height, const char* title)
    : m_window(nullptr)
    , m_width(width)
    , m_height(height)
    , m_title(title)
    , m_deltaTime(0.0f)
    , m_lastFrame(0.0f)
    , m_cursorEnabled(true)
    , m_clearColor(0.1f, 0.1f, 0.1f, 1.0f)
    , m_mousePos(0.0f)
    , m_lastMousePos(0.0f)
    , m_mouseDelta(0.0f)
    , m_firstMouse(true)
{
}

Application::~Application()
{
}

int Application::run()
{
    // Initialize systems
    if (!initWindow())
    {
        std::cerr << "ERROR::APPLICATION::FAILED_TO_INIT_WINDOW" << std::endl;
        return -1;
    }

    if (!initOpenGL())
    {
        std::cerr << "ERROR::APPLICATION::FAILED_TO_INIT_OPENGL" << std::endl;
        glfwTerminate();
        return -1;
    }

    if (!initImGui())
    {
        std::cerr << "ERROR::APPLICATION::FAILED_TO_INIT_IMGUI" << std::endl;
        glfwTerminate();
        return -1;
    }

    // User initialization
    onInit();

    // Main loop
    while (!glfwWindowShouldClose(m_window))
    {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        m_deltaTime = currentFrame - m_lastFrame;
        m_lastFrame = currentFrame;

        // Poll events
        glfwPollEvents();

        // Update mouse state
        updateMouseState();

        // User update
        onUpdate(m_deltaTime);

        // Get current framebuffer size
        int displayW, displayH;
        glfwGetFramebufferSize(m_window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);

        // Clear screen
        glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // User render
        onRender();

        // ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // User ImGui
        onImGui();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(m_window);
    }

    // User shutdown
    onShutdown();

    // Cleanup
    shutdownImGui();
    glfwDestroyWindow(m_window);
    glfwTerminate();

    return 0;
}

bool Application::initWindow()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "ERROR::GLFW::INIT_FAILED" << std::endl;
        return false;
    }

    // Configure GLFW for OpenGL 4.5 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window)
    {
        std::cerr << "ERROR::GLFW::WINDOW_CREATION_FAILED" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    // Store this pointer for callbacks
    glfwSetWindowUserPointer(m_window, this);

    // Set callbacks
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);

    return true;
}

bool Application::initOpenGL()
{
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "ERROR::GLAD::INIT_FAILED" << std::endl;
        return false;
    }

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    return true;
}

bool Application::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true))
    {
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init("#version 450"))
    {
        return false;
    }

    return true;
}

void Application::shutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::updateMouseState()
{
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    m_mousePos = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));

    if (m_firstMouse)
    {
        m_lastMousePos = m_mousePos;
        m_firstMouse = false;
    }

    m_mouseDelta = m_mousePos - m_lastMousePos;
    m_lastMousePos = m_mousePos;
}

bool Application::isKeyPressed(int key) const
{
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Application::isKeyDown(int key) const
{
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Application::isMouseButtonPressed(int button) const
{
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

float Application::getAspectRatio() const
{
    if (m_height == 0) return 1.0f;
    return static_cast<float>(m_width) / static_cast<float>(m_height);
}

void Application::setCursorEnabled(bool enabled)
{
    m_cursorEnabled = enabled;
    if (enabled)
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_firstMouse = true;
    }
}

// Static callbacks
void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app)
    {
        app->m_width = width;
        app->m_height = height;
        glViewport(0, 0, width, height);
        app->onResize(width, height);
    }
}

void Application::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app)
    {
        app->m_mousePos = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
    }
}

void Application::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)xoffset;
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app)
    {
        app->onScroll(static_cast<float>(yoffset));
    }
}
