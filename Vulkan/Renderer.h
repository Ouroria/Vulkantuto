#pragma once
#include "Debug.h"
#include "Dependencies.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <cstdint>
#include <fstream>

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
	

private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue queue = nullptr;
	vk::raii::SwapchainKHR swapChain = nullptr;
	vk::raii::PipelineLayout pipelineLayout = nullptr;
	vk::raii::Pipeline graphicsPipeline = nullptr;
	vk::raii::CommandPool commandPool = nullptr;
	GLFWwindow* window = nullptr;

	std::vector<vk::raii::CommandBuffer> commandBuffers;

	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;

	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;

	std::vector<vk::raii::Fence> inFlightFences;

	uint32_t frameIndex = 0;

	uint32_t queueIndex = ~0;

	vk::SurfaceFormatKHR swapChainSurfaceFormat;

	vk::Extent2D swapChainExtent;

	std::vector<vk::Image> swapChainImages;

	std::vector<vk::raii::ImageView> swapChainImageViews;

	std::vector<const char*> requiredDeviceExtension = {vk::KHRSwapchainExtensionName };

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);

};
