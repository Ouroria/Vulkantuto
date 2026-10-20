#pragma once
#include "Renderer.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
class app
{
public:
	int width = 800;
	int height = 600;
	GLFWwindow* window = nullptr;
	Renderer* renderer = nullptr;

	void Init();
	void Run();
	void Destroy();

private:
	void InitWindow();
	void InitVulkan();
	void DestroyVulkan();
	void DestroyWindow();

};

