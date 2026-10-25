#include "Renderer.h"
#include "Debug.h"

void Renderer::setWindow(GLFWwindow* _window)
{
	ctxVulkan.window = _window;
}

void Renderer::setFramebuffer(bool x) {
	ctxVulkan.framebufferResized = x;
}

bool Renderer::getFramebuffer() {
	return ctxVulkan.framebufferResized;
}


void Renderer::createInstance()
{
	vk::ApplicationInfo application_info
	{
		.pApplicationName = "Triangle",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Gaming Campus Vulkan",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = vk::ApiVersion14
	};

	std::vector RequiredExtension = GetRequiredExtensions();

	auto extensionProperties = ctxVulkan.context.enumerateInstanceExtensionProperties();

	for (uint32_t i = 0; i < RequiredExtension.size(); i++) {

		auto compareExtensions = [extension = RequiredExtension[i]] (auto const& extensionsProperty)
			{
				return strcmp(extensionsProperty.extensionName, extension) == 0;
			}
		;
		if (std::ranges::none_of(extensionProperties, compareExtensions))
		{
			throw std::runtime_error("Required GLFW extension not supported: " + std::string(RequiredExtension[i]));
		};
	};

	std::vector<char const*> requiredLayers = GetRequiredLayers();

	auto layerProperties = ctxVulkan.context.enumerateInstanceLayerProperties();

	auto unsupportedLayerIt = std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer)
		{
			return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) 

				{
					return strcmp(layerProperty.layerName, requiredLayer) == 0;
				});
		});

	if (unsupportedLayerIt != requiredLayers.end())
	{
		throw std::runtime_error("Required layer not supported: " + std::string(*unsupportedLayerIt));
	}

	vk::InstanceCreateInfo create_info{
		.pApplicationInfo = &application_info,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(RequiredExtension.size()),
		.ppEnabledExtensionNames = RequiredExtension.data()
		
	};

	ctxVulkan.instance = vk::raii::Instance(ctxVulkan.context, create_info);

	SetupDebugCallback();
}

bool Renderer::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
	bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

	auto queueFamilies = physicalDevice.getQueueFamilyProperties();
	bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp) { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

	auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
	bool supportsAllRequiredExtensions =
		std::ranges::all_of(ctxVulkan.requiredDeviceExtension,
			[&availableDeviceExtensions](auto const& requiredDeviceExtension)
			{
				return std::ranges::any_of(availableDeviceExtensions,
					[requiredDeviceExtension](auto const& availableDeviceExtension)
					{ return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0; });
			});

	auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

	bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceFeatures2>().features.samplerAnisotropy &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
		features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
		features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

	return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

void Renderer::pickPhysicalDevice()
{
	std::vector<vk::raii::PhysicalDevice> physicalDevices = ctxVulkan.instance.enumeratePhysicalDevices();
	auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) { return isDeviceSuitable(physicalDevice); });
	if (devIter == physicalDevices.end())
	{
		throw std::runtime_error("failed to find a suitable GPU!");
	}
	ctxVulkan.physicalDevice = *devIter;

}

void Renderer::createLogicalDevice() {
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = ctxVulkan.physicalDevice.getQueueFamilyProperties();

	for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
	{
		if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
			ctxVulkan.physicalDevice.getSurfaceSupportKHR(qfpIndex, *ctxVulkan.surface))
		{
			ctxVulkan.graphicsQueueIndex = qfpIndex;
			break;
		}
	}
	if (ctxVulkan.graphicsQueueIndex == ~0)
	{
		throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
	}

	auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const& qfp) { return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0); });

	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
	{.features = {.samplerAnisotropy = true } },
	{.synchronization2 = true, .dynamicRendering = true },
	{.extendedDynamicState = true }
	};

	float                     queuePriority = 0.5f;
	vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = ctxVulkan.graphicsQueueIndex , .queueCount = 1, .pQueuePriorities = &queuePriority };
	vk::DeviceCreateInfo      deviceCreateInfo{ .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
											   .queueCreateInfoCount = 1,
											   .pQueueCreateInfos = &deviceQueueCreateInfo,
											   .enabledExtensionCount = static_cast<uint32_t>(ctxVulkan.requiredDeviceExtension.size()),
											   .ppEnabledExtensionNames = ctxVulkan.requiredDeviceExtension.data() };

	ctxVulkan.device = vk::raii::Device(ctxVulkan.physicalDevice, deviceCreateInfo);

	ctxVulkan.graphicsQueue = vk::raii::Queue(ctxVulkan.device, ctxVulkan.graphicsQueueIndex, 0);
}

std::vector<char const*> Renderer::GetRequiredExtensions()
{
	uint32_t glfw_extensions_count = 0;
	const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfw_extensions_count);
	
	std::vector required_extensions = { vk::EXTDebugUtilsExtensionName };
	
	std::vector extensions(glfwExtensions, glfwExtensions + glfw_extensions_count);

	extensions.append_range(required_extensions);

	return extensions;
}

std::vector<char const*> Renderer::GetRequiredLayers()
{
	const std::vector<char const*> validationLayer = { "VK_LAYER_KHRONOS_validation" };

	std::vector<char const*> requiredLayers = {};

	//if (Debug::enableDebug)
		//requiredLayers.append_range(validationLayer);

	return requiredLayers;
}

void Renderer::SetupDebugCallback()
{
	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | 
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);

	vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

	vk::DebugUtilsMessengerCreateInfoEXT debugInfo
	{
		.messageSeverity = severityFlags,
		.messageType = messageTypeFlags,
		.pfnUserCallback = &DebugCallBack
	};

	ctxVulkan.debugMessenger = ctxVulkan.instance.createDebugUtilsMessengerEXT(debugInfo);
}

uint32_t Renderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties = ctxVulkan.physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

vk::Format Renderer::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for (const auto format : candidates)
	{
		vk::FormatProperties props = ctxVulkan.physicalDevice.getFormatProperties(format);
		if (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
			((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)))
		{
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL Renderer::DebugCallBack(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallBackData, void* pUserData)
{

	std::println(std::cout, "Validatin layer: Type[{}]", to_string(type));
	std::println(std::cout, "Message : {}", pCallBackData->pMessage);

	return vk::False;
}


