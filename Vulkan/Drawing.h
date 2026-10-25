#pragma once
#include "Dependencies.h"
#include "VulkanConfig.h"
#include "VulkanContext.h"
#include "Texture.h"
#include "Buffers.h"

class Drawing
{
public:
	explicit Drawing(VulkanContext& ctx, Texture* tex, Buffers* buf)
		: ctxVulkan(ctx), texture(tex), buffers(buf)
	{
	}
	void drawFrame();

	void createSyncObjects();

private:
	VulkanContext& ctxVulkan;
	Texture* texture = nullptr;
	Buffers* buffers = nullptr;
};

