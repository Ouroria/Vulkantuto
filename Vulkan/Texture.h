#pragma once
#include "Dependencies.h"
#include "VulkanContext.h"
#include "Renderer.h"
#include "Buffers.h"

#include "stb_image.h"

class Texture
{
public:
	explicit Texture(VulkanContext& cxt, Renderer* rend, Buffers* buf)
		: ctxVulkan(cxt), renderer(rend), buffers(buf) {
	}
	void cleanupSwapChain();
	void recreateSwapChain();
	void createSurface();
	void createSwapChain();
	void createImageViews();
	void createDepthResources();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& avaivleFormats);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);
	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::raii::ImageView createImageView(vk::Image const& image, vk::Format format, vk::ImageAspectFlags aspectFlags);

	void updateUniformBuffer(uint32_t currentImage);

	vk::Format findDepthFormat();

private:
	VulkanContext& ctxVulkan;
	Renderer* renderer = nullptr;
	Buffers* buffers = nullptr;

	std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(
		uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
};

