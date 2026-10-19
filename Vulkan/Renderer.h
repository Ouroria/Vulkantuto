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
	void CreateInstance();
	
	void pickPhysicalDevice();

	void createLogicalDevice();

	void createSurface();

	void createSwapChain();

	void createImageViews();

	void createGraphicsPipeline();

	static std::vector<char> readFile(const std::string& filename);

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;

	void SetWindow(GLFWwindow* _window);

	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& avaivleFormats);

	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);

	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);

	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);

	std::vector<char const*> GetRequiredExtensions();

	std::vector <char const*> GetRequiredLayers();

	void SetupDebugCallback();
	

private:
	vk::raii::Context context;

	vk::raii::Instance instance = nullptr;

	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

	vk::raii::PhysicalDevice physicalDevice = nullptr;

	vk::raii::Device device = nullptr;

	vk::raii::Queue queue = nullptr;

	vk::SurfaceFormatKHR swapChainSurfaceFormat;

	vk::Extent2D swapChainExtent;

	vk::raii::SwapchainKHR swapChain = nullptr;

	std::vector<vk::Image> swapChainImages;

	std::vector<vk::raii::ImageView> swapChainImageViews;

	std::vector<const char*> requiredDeviceExtension = {vk::KHRSwapchainExtensionName };

	GLFWwindow* window = nullptr;

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);

	vk::raii::SurfaceKHR surface = nullptr;

};
