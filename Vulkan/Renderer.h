#pragma once
#include "Debug.h"
#include "Dependencies.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <cstdint>
#include <fstream>
#include <glm/glm.hpp>
#include <array>

class Renderer
{
public:

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;
	
	void cleanupSwapChain();
	void recreateSwapChain();

	void CreateInstance();
	
	void pickPhysicalDevice();

	void createLogicalDevice();

	void createSurface();

	void createSwapChain();

	void createImageViews();

	void createGraphicsPipeline();

	void createCommandPool();

	void createCommandBuffers();

	void createVertexBuffer();

	void createIndexBuffer();

	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

	void setWindow(GLFWwindow* _window);

	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);

	void SetupDebugCallback();

	void recordCommandBuffer(uint32_t imageIndex);

	void transition_image_layout(
		uint32_t                imageIndex,
		vk::ImageLayout         old_layout,
		vk::ImageLayout         new_layout,
		vk::AccessFlags2        src_access_mask,
		vk::AccessFlags2        dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask);

	void createSyncObjects();

	void drawFrame();

	static std::vector<char> readFile(const std::string& filename);

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& avaivleFormats);

	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);

	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);

	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);

	std::vector<char const*> GetRequiredExtensions();

	std::vector <char const*> GetRequiredLayers();

	void setFramebuffer(bool x);

	bool getFramebuffer();
	

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::SwapchainKHR swapChain = nullptr;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;
	vk::raii::Buffer       indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr;
	GLFWwindow* window = nullptr;

	std::vector<vk::raii::CommandBuffer> commandBuffers;

	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;

	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;

	std::vector<vk::raii::Fence> inFlightFences;

	uint32_t frameIndex = 0;

	uint32_t graphicsQueueIndex = ~0;

	vk::SurfaceFormatKHR swapChainSurfaceFormat;

	vk::Extent2D swapChainExtent;

	std::vector<vk::Image> swapChainImages;

	std::vector<vk::raii::ImageView> swapChainImageViews;

	std::vector<const char*> requiredDeviceExtension = {vk::KHRSwapchainExtensionName };

	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);

	bool framebufferResized = false;

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);

};
