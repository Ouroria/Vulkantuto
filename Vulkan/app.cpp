#include "app.h"

void App::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<Renderer*>(glfwGetWindowUserPointer(window));
    app->setFramebuffer(true);
}

void App::Init()
{
    InitWindow();
    InitVulkan();
}

void App::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        renderer->drawFrame();
    }
}

void App::Destroy()
{
    DestroyVulkan();
    DestroyWindow();
}

void App::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(800, 600, "Vulkan Triangle", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void App::InitVulkan()
{

    renderer = new Renderer();
    renderer->setWindow(window);

    renderer->CreateInstance();
    renderer->SetupDebugCallback();
    renderer->createSurface();
    renderer->pickPhysicalDevice();
    renderer->createLogicalDevice();
    renderer->createSwapChain();
    renderer->createImageViews();
    renderer->createGraphicsPipeline();
    renderer->createCommandPool();
    renderer->createVertexBuffer();
    renderer->createIndexBuffer();
    renderer->createCommandBuffers();
    renderer->createSyncObjects();

}

void App::DestroyWindow()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

void App::DestroyVulkan()
{
}