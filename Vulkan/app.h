#pragma once
#include "Renderer.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
class App
{
public:
	int width = 800;
	int height = 600;
	GLFWwindow* window = nullptr;
	Renderer* renderer = nullptr;

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

