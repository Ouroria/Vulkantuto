#pragma once
#include "Debug.h"
#include "Dependencies.h"
#include "VulkanContext.h"

class Renderer
{
public:
	explicit Renderer(VulkanContext& cxt)
		: ctxVulkan(cxt) {
	}

	void createInstance();
	
	void pickPhysicalDevice();

	void createLogicalDevice();

	void setWindow(GLFWwindow* _window);

	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);

	void SetupDebugCallback();

	std::vector<char const*> GetRequiredExtensions();

	std::vector <char const*> GetRequiredLayers();

	void setFramebuffer(bool x);

	bool getFramebuffer();
	
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

private:
	VulkanContext& ctxVulkan;


	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT, vk::DebugUtilsMessageTypeFlagsEXT, const vk::DebugUtilsMessengerCallbackDataEXT*, void*);

};
