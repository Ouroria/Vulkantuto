#include "Texture.h"

void Texture::cleanupSwapChain()
{
	ctxVulkan.swapChainImageViews.clear();
	ctxVulkan.swapChain = nullptr;
}

void Texture::recreateSwapChain()
{
	int width = 0, height = 0;
	while ((width == 0 || height == 0) && !glfwWindowShouldClose(ctxVulkan.window))
	{
		glfwGetFramebufferSize(ctxVulkan.window, &width, &height);
		glfwWaitEvents();
	}
	if (glfwWindowShouldClose(ctxVulkan.window))
	{
		return;
	}

	ctxVulkan.device.waitIdle();

	cleanupSwapChain();
	createSwapChain();
	createImageViews();
	createDepthResources();
}

void Texture::createSurface() {
	VkSurfaceKHR  _surface;
	if (glfwCreateWindowSurface(*ctxVulkan.instance, ctxVulkan.window, nullptr, &_surface) != 0) {
		throw std::runtime_error("failed to create window surface!");
	}
	ctxVulkan.surface = vk::raii::SurfaceKHR(ctxVulkan.instance, _surface);
}

void Texture::createSwapChain()
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = ctxVulkan.physicalDevice.getSurfaceCapabilitiesKHR(*ctxVulkan.surface);
	ctxVulkan.swapChainExtent = chooseSwapExtent(surfaceCapabilities);
	uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

	std::vector<vk::SurfaceFormatKHR> availableFormats = ctxVulkan.physicalDevice.getSurfaceFormatsKHR(*ctxVulkan.surface);
	ctxVulkan.swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);

	std::vector<vk::PresentModeKHR> availablePresentModes = ctxVulkan.physicalDevice.getSurfacePresentModesKHR(*ctxVulkan.surface);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(availablePresentModes);

	vk::SwapchainCreateInfoKHR swapChainCreateInfo{ .surface = *ctxVulkan.surface,
											   .minImageCount = minImageCount,
											   .imageFormat = ctxVulkan.swapChainSurfaceFormat.format,
											   .imageColorSpace = ctxVulkan.swapChainSurfaceFormat.colorSpace,
											   .imageExtent = ctxVulkan.swapChainExtent,
											   .imageArrayLayers = 1,
											   .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
											   .imageSharingMode = vk::SharingMode::eExclusive,
											   .preTransform = surfaceCapabilities.currentTransform,
											   .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
											   .presentMode = chooseSwapPresentMode(availablePresentModes),
											   .clipped = true };

	ctxVulkan.swapChain = vk::raii::SwapchainKHR(ctxVulkan.device, swapChainCreateInfo);
	ctxVulkan.swapChainImages = ctxVulkan.swapChain.getImages();

}

void Texture::createImageViews()
{
	assert(ctxVulkan.swapChainImageViews.empty());

	ctxVulkan.swapChainImageViews.reserve(ctxVulkan.swapChainImages.size());
	for (auto& image : ctxVulkan.swapChainImages)
	{
		ctxVulkan.swapChainImageViews.emplace_back(createImageView(image, ctxVulkan.swapChainSurfaceFormat.format, vk::ImageAspectFlagBits::eColor));
	}
}

vk::raii::ImageView Texture::createImageView(vk::Image const& image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo{
			.image = image,
			.viewType = vk::ImageViewType::e2D,
			.format = format,
			.subresourceRange = {.aspectMask = aspectFlags, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1} };
	return vk::raii::ImageView(ctxVulkan.device, viewInfo);
}

void Texture::createDepthResources()
{
	vk::Format depthFormat = findDepthFormat();

	std::tie(ctxVulkan.depthImage, ctxVulkan.depthImageMemory) = createImage(ctxVulkan.swapChainExtent.width, ctxVulkan.swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal);
	ctxVulkan.depthImageView = createImageView(ctxVulkan.depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void Texture::createTextureImage()
{
	int            texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/texture.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels)
	{
		throw std::runtime_error("failed to load texture image!");
	}

	auto [stagingBuffer, stagingBufferMemory] =
		buffers->createBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data = stagingBufferMemory.mapMemory(0, imageSize);
	memcpy(data, pixels, imageSize);
	stagingBufferMemory.unmapMemory();

	stbi_image_free(pixels);

	std::tie(ctxVulkan.textureImage, ctxVulkan.textureImageMemory) = createImage(texWidth,
		texHeight,
		vk::Format::eR8G8B8A8Srgb,
		vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	vk::raii::CommandBuffer commandBuffer = buffers->beginSingleTimeCommands();
	buffers->transitionImageLayout(commandBuffer, ctxVulkan.textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	buffers->copyBufferToImage(commandBuffer, stagingBuffer, ctxVulkan.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	buffers->transitionImageLayout(commandBuffer, ctxVulkan.textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
	buffers->endSingleTimeCommands(std::move(commandBuffer));
}

void Texture::createTextureImageView()
{
	ctxVulkan.textureImageView = createImageView(*ctxVulkan.textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void Texture::createTextureSampler()
{
	vk::PhysicalDeviceProperties properties = ctxVulkan.physicalDevice.getProperties();
	vk::SamplerCreateInfo samplerInfo{ .magFilter = vk::Filter::eLinear,
									   .minFilter = vk::Filter::eLinear,
									   .mipmapMode = vk::SamplerMipmapMode::eLinear,
									   .addressModeU = vk::SamplerAddressMode::eRepeat,
									   .addressModeV = vk::SamplerAddressMode::eRepeat,
									   .addressModeW = vk::SamplerAddressMode::eRepeat,
									   .mipLodBias = 0.0f,
									   .anisotropyEnable = vk::True,
									   .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
									   .compareEnable = vk::False,
									   .compareOp = vk::CompareOp::eAlways };
	ctxVulkan.textureSampler = vk::raii::Sampler(ctxVulkan.device, samplerInfo);
}

vk::SurfaceFormatKHR Texture::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
	const auto formatIt = std::ranges::find_if(
		availableFormats,
		[](const auto& format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
	return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::Extent2D Texture::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	int width, height;
	glfwGetFramebufferSize(ctxVulkan.window, &width, &height);

	return {
		std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
	};
}

vk::PresentModeKHR Texture::chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
	assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
	return std::ranges::any_of(availablePresentModes,
		[](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ?
		vk::PresentModeKHR::eMailbox :
		vk::PresentModeKHR::eFifo;
}

uint32_t Texture::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
	auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
	if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
	{
		minImageCount = surfaceCapabilities.maxImageCount;
	}
	return minImageCount;
}

void Texture::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	UniformBufferObject ubo{};
	ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj =
		glm::perspective(glm::radians(45.0f), static_cast<float>(ctxVulkan.swapChainExtent.width) / static_cast<float>(ctxVulkan.swapChainExtent.height), 0.1f, 10.0f);

	memcpy(ctxVulkan.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

std::pair<vk::raii::Image, vk::raii::DeviceMemory> Texture::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	vk::ImageCreateInfo imageInfo{ .imageType = vk::ImageType::e2D,
								.format = format,
								.extent = {width, height, 1},
								.mipLevels = 1,
								.arrayLayers = 1,
								.samples = vk::SampleCountFlagBits::e1,
								.tiling = tiling,
								.usage = usage,
								.sharingMode = vk::SharingMode::eExclusive };

	vk::raii::Image image = vk::raii::Image(ctxVulkan.device, imageInfo);

	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size,
									 .memoryTypeIndex = renderer->findMemoryType(memRequirements.memoryTypeBits, properties) };
	vk::raii::DeviceMemory imageMemory = vk::raii::DeviceMemory(ctxVulkan.device, allocInfo);
	image.bindMemory(imageMemory, 0);

	return { std::move(image), std::move(imageMemory) };
}

vk::Format Texture::findDepthFormat()
{
	return renderer->findSupportedFormat({ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}
