#pragma once
#include "Dependencies.h"
#include "VulkanConfig.h"
#include "VulkanContext.h"
#include "Renderer.h"

class Buffers
{
public:
	explicit Buffers(VulkanContext& ctx, Renderer* rend)
		: ctxVulkan(ctx), renderer(rend)
	{
	}
	void createCommandPool();

	void createCommandBuffers();

	void createVertexBuffer();

	void createIndexBuffer();

	void createUniformBuffers();

	void createDescriptorPool();

	void createDescriptorSets();

	void createDescriptorSetLayout();

	void createTextureImage();

	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);

	void recordCommandBuffer(uint32_t imageIndex);

	vk::raii::CommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer);

	void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

	void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);

	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);

	void transition_image_layout(
		vk::Image               image,
		vk::ImageLayout         old_layout,
		vk::ImageLayout         new_layout,
		vk::AccessFlags2        src_access_mask,
		vk::AccessFlags2        dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask,
		vk::ImageAspectFlags    image_aspect_flags);

private:
	VulkanContext& ctxVulkan;
	Renderer* renderer = nullptr;
};

