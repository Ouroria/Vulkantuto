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
}

void app::InitVulkan()
{

    renderer = new Renderer();
    renderer->SetWindow(window);

    renderer->CreateInstance();
    renderer->SetupDebugCallback();
    renderer->createSurface();
    renderer->pickPhysicalDevice();
    renderer->createLogicalDevice();
    renderer->createSwapChain();
    renderer->createImageViews();
    renderer->createGraphicsPipeline();

}

void app::DestroyWindow()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

void app::DestroyVulkan()
{
}