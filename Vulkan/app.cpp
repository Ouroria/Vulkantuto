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
        drawing->drawFrame();
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

    window = glfwCreateWindow(800, 600, "Vulkan cube", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void App::InitVulkan()
{

    renderer = new Renderer(context);
    buffers = new Buffers(context, renderer);
    texture = new Texture(context, renderer, buffers);
    graphicpipeline = new GraphicPipeline(context, texture);
    drawing = new Drawing(context, texture, buffers);
    renderer->setWindow(window);
    renderer->createInstance();
    renderer->SetupDebugCallback();
    texture->createSurface();
    renderer->pickPhysicalDevice();
    renderer->createLogicalDevice();
    texture->createSwapChain();
    texture->createImageViews();
    buffers->createDescriptorSetLayout();
    graphicpipeline->createGraphicsPipeline();
    buffers->createCommandPool();
    texture->createDepthResources();
    texture->createTextureImage();
    texture->createTextureImageView();
    texture->createTextureSampler();
    buffers->createVertexBuffer();
    buffers->createIndexBuffer();
    buffers->createUniformBuffers();
    buffers->createDescriptorPool();
    buffers->createDescriptorSets();
    buffers->createCommandBuffers();
    drawing->createSyncObjects();

}

void App::DestroyWindow()
{
    glfwDestroyWindow(window);

    glfwTerminate();
}

void App::DestroyVulkan()
{
}