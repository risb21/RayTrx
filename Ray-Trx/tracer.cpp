#include "tracer.hpp"

// std
#include <stdexcept>
#include <array>

namespace rtrx {
	
	Tracer::Tracer() {
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	Tracer::~Tracer() { vkDestroyPipelineLayout(rtrxDev.device(), pipelineLayout, nullptr); }

	void Tracer::run() {
		while (!rtrxWindow.shouldClose()) {
			glfwPollEvents(); // keystrokes, clicking X button etc
			drawFrame();
		}

		vkDeviceWaitIdle(rtrxDev.device());
	}

	void Tracer::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(rtrxDev.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void Tracer::createPipeline() {
		auto pipelineConfig = 
			rtrxPipeline::defaultPipelineConfigInfo(rtrxSwapChain.width(), rtrxSwapChain.height());
		pipelineConfig.renderpass = rtrxSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		RtrxPipeline = std::make_unique<rtrxPipeline> (
			rtrxDev,
			"simple_shader.vert.spv",
			"simple_shader.frag.spv",
			pipelineConfig );
	}

	void Tracer::createCommandBuffers() {
		commandBuffers.resize(rtrxSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = rtrxDev.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t> (commandBuffers.size());

		if (vkAllocateCommandBuffers(rtrxDev.device(), &allocInfo, commandBuffers.data()) != 
			VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!");
		}

		for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rtrxSwapChain.getRenderPass();
			renderPassInfo.framebuffer = rtrxSwapChain.getFrameBuffer(i);
			
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = rtrxSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 10.f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t> (clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			RtrxPipeline->bind(commandBuffers[i]);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to record command buffer");
			}
		}
	}
	void Tracer::drawFrame() {
		uint32_t imageIndex;
		auto result = rtrxSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		result = rtrxSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}
	}
}