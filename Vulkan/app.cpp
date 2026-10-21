#include "app.h"

void app::Init()
{
    InitWindow();
    InitVulkan();
}

void app::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        renderer->drawFrame();
    }
}

void app::Destroy()
{
    DestroyVulkan();
    DestroyWindow();
}

void app::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(800, 600, "Vulkan Triangle", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void app::InitVulkan()
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
    renderer->createCommandBuffers();
    renderer->createSyncObjects();

}

void app::DestroyWindow()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

void app::DestroyVulkan()
{
}