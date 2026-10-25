#pragma once
#include "Dependencies.h"
#include "UniformBufferObject.h"
#include "Vertex.h"

class VulkanContext
{
public:

	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SwapchainKHR swapChain = nullptr;
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;
	vk::raii::Buffer indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr;
	vk::raii::DescriptorPool descriptorPool = nullptr;
	vk::raii::Image textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
	vk::raii::ImageView    textureImageView = nullptr;
	vk::raii::Sampler      textureSampler = nullptr;
	vk::raii::Image        depthImage = nullptr;
	vk::raii::DeviceMemory depthImageMemory = nullptr;
	vk::raii::ImageView    depthImageView = nullptr;
	GLFWwindow* window = nullptr;

	vk::SurfaceFormatKHR swapChainSurfaceFormat;

	vk::Extent2D swapChainExtent;

	std::vector<vk::Image> swapChainImages;

	std::vector<vk::raii::ImageView> swapChainImageViews;

	UniformBufferObject ubo{};

	Vertex vertex;

	std::vector<vk::raii::Buffer> uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	std::vector<vk::raii::DescriptorSet> descriptorSets;

	std::vector<vk::raii::CommandBuffer> commandBuffers;

	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;

	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;

	std::vector<vk::raii::Fence> inFlightFences;

	uint32_t frameIndex = 0;

	uint32_t graphicsQueueIndex = ~0;

	std::vector<const char*> requiredDeviceExtension = { vk::KHRSwapchainExtensionName };

	bool framebufferResized = false;
};

