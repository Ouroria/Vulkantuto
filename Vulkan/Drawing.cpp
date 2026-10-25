#include "Drawing.h"

void Drawing::drawFrame()
{
	auto fenceResult = ctxVulkan.device.waitForFences(*ctxVulkan.inFlightFences[ctxVulkan.frameIndex], vk::True, UINT64_MAX);
	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}

	auto [result, imageIndex] = ctxVulkan.swapChain.acquireNextImage(UINT64_MAX, *ctxVulkan.presentCompleteSemaphores[ctxVulkan.frameIndex], nullptr);

	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		texture->recreateSwapChain();
		return;
	}
	if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	texture->updateUniformBuffer(ctxVulkan.frameIndex);

	ctxVulkan.device.resetFences(*ctxVulkan.inFlightFences[ctxVulkan.frameIndex]);

	ctxVulkan.commandBuffers[ctxVulkan.frameIndex].reset();
	buffers->recordCommandBuffer(imageIndex);

	ctxVulkan.graphicsQueue.waitIdle();

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1,
						  .pWaitSemaphores = &*ctxVulkan.presentCompleteSemaphores[ctxVulkan.frameIndex],
						  .pWaitDstStageMask = &waitDestinationStageMask,
						  .commandBufferCount = 1,
						  .pCommandBuffers = &*ctxVulkan.commandBuffers[ctxVulkan.frameIndex],
						  .signalSemaphoreCount = 1,
						  .pSignalSemaphores = &*ctxVulkan.renderFinishedSemaphores[imageIndex] };
	ctxVulkan.graphicsQueue.submit(submitInfo, *ctxVulkan.inFlightFences[ctxVulkan.frameIndex]);

	const vk::PresentInfoKHR presentInfoKHR{ .waitSemaphoreCount = 1,
											.pWaitSemaphores = &*ctxVulkan.renderFinishedSemaphores[imageIndex],
											.swapchainCount = 1,
											.pSwapchains = &*ctxVulkan.swapChain,
											.pImageIndices = &imageIndex };
	result = ctxVulkan.graphicsQueue.presentKHR(presentInfoKHR);
	if ((result == vk::Result::eSuboptimalKHR) || (result == vk::Result::eErrorOutOfDateKHR))
	{
		ctxVulkan.framebufferResized = false;
		texture->recreateSwapChain();
	}
	else
	{
		assert(result == vk::Result::eSuccess);
	}
	switch (result)
	{
	case vk::Result::eSuccess:
		break;
	case vk::Result::eSuboptimalKHR:
		std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
		break;
	default:
		break;
	}
}

void Drawing::createSyncObjects()
{
	assert(ctxVulkan.presentCompleteSemaphores.empty() && ctxVulkan.renderFinishedSemaphores.empty() && ctxVulkan.inFlightFences.empty());

	for (size_t i = 0; i < ctxVulkan.swapChainImages.size(); i++)
	{
		ctxVulkan.renderFinishedSemaphores.emplace_back(ctxVulkan.device, vk::SemaphoreCreateInfo());
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		ctxVulkan.presentCompleteSemaphores.emplace_back(ctxVulkan.device, vk::SemaphoreCreateInfo());
		ctxVulkan.inFlightFences.emplace_back(ctxVulkan.device, vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
	}
}