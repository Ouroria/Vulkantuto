#pragma once
#include "Dependencies.h"
#include "VulkanContext.h"
#include "Texture.h"

class GraphicPipeline
{
public:
	explicit GraphicPipeline(VulkanContext& ctx, Texture* tex)
		: ctxVulkan(ctx), texture(tex)
	{
	}

	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;
	void createGraphicsPipeline();


	static std::vector<char> readFile(const std::string& filename);

private:
	VulkanContext& ctxVulkan;
	Texture* texture = nullptr;

};

