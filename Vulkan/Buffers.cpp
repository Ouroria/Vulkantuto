#include "Buffers.h"

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f,  0.5f}, {1, 0, 0}, {0, 0}},
	{{ 0.5f, -0.5f,  0.5f}, {0, 1, 0}, {1, 0}},
	{{ 0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
	{{-0.5f,  0.5f,  0.5f}, {1, 1, 1}, {0, 1}},

	{{-0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0}},
	{{ 0.5f, -0.5f, -0.5f}, {0, 1, 0}, {1, 0}},
	{{ 0.5f,  0.5f, -0.5f}, {0, 0, 1}, {1, 1}},
	{{-0.5f,  0.5f, -0.5f}, {1, 1, 1}, {0, 1}},

	{{-0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0}},
	{{ 0.5f, -0.5f, -0.5f}, {0, 1, 0}, {1, 0}},
	{{ 0.5f, -0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
	{{-0.5f, -0.5f,  0.5f}, {1, 0, 0}, {0, 1}},

	{{-0.5f,  0.5f,  0.5f}, {1, 0, 0}, {0, 0}},
	{{ 0.5f,  0.5f,  0.5f}, {0, 1, 0}, {1, 0}},
	{{ 0.5f,  0.5f, -0.5f}, {0, 0, 1}, {1, 1}},
	{{-0.5f,  0.5f, -0.5f}, {1, 1, 1}, {0, 1}},

	{{-0.5f, -0.5f, -0.5f}, {1, 0, 0}, {0, 0}},
	{{-0.5f, -0.5f,  0.5f}, {0, 1, 0}, {1, 0}},
	{{-0.5f,  0.5f,  0.5f}, {0, 0, 1}, {1, 1}},
	{{-0.5f,  0.5f, -0.5f}, {1, 1, 1}, {0, 1}},

	{{ 0.5f, -0.5f,  0.5f}, {1, 0, 0}, {0, 0}},
	{{ 0.5f, -0.5f, -0.5f}, {0, 1, 0}, {1, 0}},
	{{ 0.5f,  0.5f, -0.5f}, {0, 0, 1}, {1, 1}},
	{{ 0.5f,  0.5f,  0.5f}, {1, 1, 1}, {0, 1}} };

const std::vector<uint16_t> indices = {
	 0, 1, 2, 2, 3, 0,

	 5, 4, 7, 7, 6, 5,

	 8, 9, 10, 10, 11, 8,

	 12, 13, 14, 14, 15, 12,

	 16, 17, 18, 18, 19, 16,

	 20, 21, 22, 22, 23, 20
};

void Buffers::recordCommandBuffer(uint32_t imageIndex)
{
	auto& commandBuffer = ctxVulkan.commandBuffers[ctxVulkan.frameIndex];
	commandBuffer.begin({});

	transition_image_layout(
		ctxVulkan.swapChainImages[imageIndex],
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::ImageAspectFlagBits::eColor);

	transition_image_layout(
		*ctxVulkan.depthImage,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthAttachmentOptimal,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::ImageAspectFlagBits::eDepth);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

	vk::RenderingAttachmentInfo colorAttachmentInfo = {
			.imageView = ctxVulkan.swapChainImageViews[imageIndex],
			.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = clearColor };

	vk::RenderingAttachmentInfo depthAttachmentInfo = {
		.imageView = ctxVulkan.depthImageView,
		.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eDontCare,
		.clearValue = clearDepth };

	vk::RenderingInfo renderingInfo = {
		.renderArea = {.offset = {0, 0}, .extent = ctxVulkan.swapChainExtent},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentInfo,
		.pDepthAttachment = &depthAttachmentInfo };

	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *ctxVulkan.graphicsPipeline);
	commandBuffer.setViewport(0, vk::Viewport(0.0f, static_cast<float>(ctxVulkan.swapChainExtent.height), static_cast<float>(ctxVulkan.swapChainExtent.width), -static_cast<float>(ctxVulkan.swapChainExtent.height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), ctxVulkan.swapChainExtent));
	ctxVulkan.commandBuffers[ctxVulkan.frameIndex].bindVertexBuffers(0, *ctxVulkan.vertexBuffer, { 0 });
	ctxVulkan.commandBuffers[ctxVulkan.frameIndex].bindIndexBuffer(*ctxVulkan.indexBuffer, 0, vk::IndexType::eUint16);
	ctxVulkan.commandBuffers[ctxVulkan.frameIndex].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, ctxVulkan.pipelineLayout, 0, *ctxVulkan.descriptorSets[ctxVulkan.frameIndex], nullptr);
	commandBuffer.drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	commandBuffer.endRendering();

	transition_image_layout(
		ctxVulkan.swapChainImages[imageIndex],
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		{},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe,
		vk::ImageAspectFlagBits::eColor);
	commandBuffer.end();
}

void Buffers::transition_image_layout(
	vk::Image               image,
	vk::ImageLayout         old_layout,
	vk::ImageLayout         new_layout,
	vk::AccessFlags2        src_access_mask,
	vk::AccessFlags2        dst_access_mask,
	vk::PipelineStageFlags2 src_stage_mask,
	vk::PipelineStageFlags2 dst_stage_mask,
	vk::ImageAspectFlags    image_aspect_flags)
{
	vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = src_stage_mask,
			.srcAccessMask = src_access_mask,
			.dstStageMask = dst_stage_mask,
			.dstAccessMask = dst_access_mask,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = image,
			.subresourceRange = {
				.aspectMask = image_aspect_flags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1} };
	vk::DependencyInfo dependency_info = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier };
	ctxVulkan.commandBuffers[ctxVulkan.frameIndex].pipelineBarrier2(dependency_info);
}

void Buffers::createCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo{ .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
										   .queueFamilyIndex = ctxVulkan.graphicsQueueIndex };

	ctxVulkan.commandPool = vk::raii::CommandPool(ctxVulkan.device, poolInfo);
}

void Buffers::createCommandBuffers()
{
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = ctxVulkan.commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = MAX_FRAMES_IN_FLIGHT };

	ctxVulkan.commandBuffers = vk::raii::CommandBuffers(ctxVulkan.device, allocInfo);

}

void Buffers::createVertexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	auto [stagingBuffer, stagingBufferMemory] =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(dataStaging, vertices.data(), bufferSize);
	stagingBufferMemory.unmapMemory();

	std::tie(ctxVulkan.vertexBuffer, ctxVulkan.vertexBufferMemory) =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

	copyBuffer(stagingBuffer, ctxVulkan.vertexBuffer, bufferSize);
}

void Buffers::createIndexBuffer()
{
	vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	auto [stagingBuffer, stagingBufferMemory] =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data = stagingBufferMemory.mapMemory(0, bufferSize);
	memcpy(data, indices.data(), (size_t)bufferSize);
	stagingBufferMemory.unmapMemory();

	std::tie(ctxVulkan.indexBuffer, ctxVulkan.indexBufferMemory) =
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

	copyBuffer(stagingBuffer, ctxVulkan.indexBuffer, bufferSize);
}

void Buffers::createUniformBuffers()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
		auto [buffer, bufferMem] = createBuffer(
			bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
		ctxVulkan.uniformBuffers.emplace_back(std::move(buffer));
		ctxVulkan.uniformBuffersMemory.emplace_back(std::move(bufferMem));
		ctxVulkan.uniformBuffersMapped.emplace_back(ctxVulkan.uniformBuffersMemory.back().mapMemory(0, bufferSize));
	}
}

void Buffers::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 2> poolSize{ {{.type = vk::DescriptorType::eUniformBuffer, .descriptorCount = MAX_FRAMES_IN_FLIGHT},
												{.type = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = MAX_FRAMES_IN_FLIGHT}} };
	vk::DescriptorPoolCreateInfo          poolInfo{ .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
												   .maxSets = MAX_FRAMES_IN_FLIGHT,
												   .poolSizeCount = static_cast<uint32_t>(poolSize.size()),
												   .pPoolSizes = poolSize.data() };

	ctxVulkan.descriptorPool = vk::raii::DescriptorPool(ctxVulkan.device, poolInfo);
}

void Buffers::createDescriptorSets()
{
	std::vector<vk::DescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *ctxVulkan.descriptorSetLayout);
	vk::DescriptorSetAllocateInfo        allocInfo{ .descriptorPool = ctxVulkan.descriptorPool,
												   .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
												   .pSetLayouts = layouts.data() };

	ctxVulkan.descriptorSets = ctxVulkan.device.allocateDescriptorSets(allocInfo);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::DescriptorBufferInfo bufferInfo{ .buffer = ctxVulkan.uniformBuffers[i], .offset = 0, .range = sizeof(UniformBufferObject) };
		vk::DescriptorImageInfo  imageInfo{ .sampler = ctxVulkan.textureSampler, .imageView = ctxVulkan.textureImageView, .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal };
		std::array<vk::WriteDescriptorSet, 2> descriptorWrites{ {{.dstSet = ctxVulkan.descriptorSets[i],
																	 .dstBinding = 0,
																	 .dstArrayElement = 0,
																	 .descriptorCount = 1,
																	 .descriptorType = vk::DescriptorType::eUniformBuffer,
																	 .pBufferInfo = &bufferInfo},
																	{.dstSet = ctxVulkan.descriptorSets[i],
																	 .dstBinding = 1,
																	 .dstArrayElement = 0,
																	 .descriptorCount = 1,
																	 .descriptorType = vk::DescriptorType::eCombinedImageSampler,
																	 .pImageInfo = &imageInfo}} };
		ctxVulkan.device.updateDescriptorSets(descriptorWrites, {});
	}
}

void Buffers::createDescriptorSetLayout()
{
	std::array<vk::DescriptorSetLayoutBinding, 2> bindings{
			{{.binding = 0, .descriptorType = vk::DescriptorType::eUniformBuffer, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eVertex},
			 {.binding = 1, .descriptorType = vk::DescriptorType::eCombinedImageSampler, .descriptorCount = 1, .stageFlags = vk::ShaderStageFlagBits::eFragment}} };

	vk::DescriptorSetLayoutCreateInfo layoutInfo{ .bindingCount = static_cast<uint32_t>(bindings.size()), .pBindings = bindings.data() };
	ctxVulkan.descriptorSetLayout = vk::raii::DescriptorSetLayout(ctxVulkan.device, layoutInfo);
}

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> Buffers::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties)
{
	vk::BufferCreateInfo bufferInfo{ .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };
	vk::raii::Buffer buffer = vk::raii::Buffer(ctxVulkan.device, bufferInfo);
	vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size, .memoryTypeIndex = renderer->findMemoryType(memRequirements.memoryTypeBits, properties) };
	vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(ctxVulkan.device, allocInfo);
	buffer.bindMemory(*bufferMemory, 0);
	return { std::move(buffer), std::move(bufferMemory) };
}
vk::raii::CommandBuffer Buffers::beginSingleTimeCommands()
{
	vk::CommandBufferAllocateInfo allocInfo{ .commandPool = ctxVulkan.commandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
	vk::raii::CommandBuffer       commandBuffer = std::move(vk::raii::CommandBuffers(ctxVulkan.device, allocInfo).front());

	vk::CommandBufferBeginInfo beginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
	commandBuffer.begin(beginInfo);

	return std::move(commandBuffer);
}

void Buffers::endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
	ctxVulkan.graphicsQueue.submit(submitInfo, nullptr);
	ctxVulkan.graphicsQueue.waitIdle();
}

void Buffers::transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	vk::ImageMemoryBarrier barrier{ .oldLayout = oldLayout,
								   .newLayout = newLayout,
								   .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
								   .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
								   .image = image,
								   .subresourceRange = {.aspectMask = vk::ImageAspectFlagBits::eColor, .levelCount = 1, .layerCount = 1} };

	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}
	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, {}, barrier);
}

void Buffers::copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height)
{
	vk::BufferImageCopy region{ .bufferOffset = 0,
								   .bufferRowLength = 0,
								   .bufferImageHeight = 0,
								   .imageSubresource = {.aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
								   .imageOffset = {0, 0, 0},
								   .imageExtent = {width, height, 1} };
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, region);
}

void Buffers::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size)
{
	vk::raii::CommandBuffer commandCopyBuffer = beginSingleTimeCommands();
	commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy{ .size = size });
	endSingleTimeCommands(std::move(commandCopyBuffer));
}