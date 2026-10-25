#pragma once
#include "Dependencies.h"
#include "VulkanContext.h"
#include "Renderer.h"
#include "Texture.h"
#include "GraphicPipeline.h"
#include "Buffers.h"
#include "Drawing.h"

class App
{
public:
	int width = 800;
	int height = 600;
	VulkanContext context;
	GLFWwindow* window = nullptr;
	Renderer* renderer = nullptr;
	Texture* texture = nullptr;
	GraphicPipeline* graphicpipeline = nullptr;
	Buffers* buffers = nullptr;
	Drawing* drawing = nullptr;


	void Init();
	void Run();
	void Destroy();

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
	void InitWindow();
	void InitVulkan();
	void DestroyVulkan();
	void DestroyWindow();

};

