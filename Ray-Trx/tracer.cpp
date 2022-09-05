#include "tracer.hpp"

// libs
#define	GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <iostream>
#include <stdexcept>
#include <array>
#include <cmath>

namespace rtrx {

	struct SimplePushConstantData {
		glm::vec2 offset;
		alignas(16) glm::vec3 colour;
	};

	
	Tracer::Tracer() {
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
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

	void Tracer::Sierpinski(std::vector<rtrxModel::Vertex>& verts,
		int depth,
		glm::vec2 left,
		glm::vec2 right,
		glm::vec2 top) {
		if (depth <= 0) {
			verts.push_back({ top });
			verts.push_back({ right });
			verts.push_back({ left });
		} else {
			auto leftTop = 0.5f * (left + top);
			auto rightTop = 0.5f * (right + top);
			auto leftRight = 0.5f * (left + right);
			Sierpinski(verts, depth - 1, left, leftRight, leftTop); // Bottom left triangle recursively
			Sierpinski(verts, depth - 1, leftRight, right, rightTop); // Bottom right triangle recursively
			Sierpinski(verts, depth - 1, leftTop, rightTop, top); // Top triangle recursively
		}
	}

	void Tracer::loadModels() {

		 // std::vector<rtrxModel::Vertex> vertices{};
		    
		 // Sierpinski(vertices, 7, { 0.0f, -0.5f }, { 0.5f, 0.5f }, { -0.5f, 0.5f });

		std::vector<rtrxModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };

		RtrxModel = std::make_unique<rtrxModel>(rtrxDev, vertices);
	}

	void Tracer::createPipelineLayout() {

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(rtrxDev.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout");
		}
	}

	void Tracer::createPipeline() {
		assert(RtrxSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
		PipelineConfigInfo pipelineConfig{};
		rtrxPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderpass = RtrxSwapChain -> getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		RtrxPipeline = std::make_unique<rtrxPipeline> (
			rtrxDev,
			"simple_shader.vert.spv",
			"simple_shader.frag.spv",
			pipelineConfig );
	}

	void Tracer::recreateSwapChain() {
		auto extent = rtrxWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = rtrxWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(rtrxDev.device());

		if (RtrxSwapChain == nullptr) {
			RtrxSwapChain = std::make_unique<rtrxSwapChain>(rtrxDev, extent);
		} else {
			RtrxSwapChain = std::make_unique<rtrxSwapChain>(rtrxDev, extent, std::move(RtrxSwapChain));
			if (RtrxSwapChain -> imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		createPipeline();
	}

	void Tracer::freeCommandBuffers() {
		vkFreeCommandBuffers(
			rtrxDev.device(),
			rtrxDev.getCommandPool(),
			static_cast<uint32_t> (commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void Tracer::createCommandBuffers() {
		commandBuffers.resize(RtrxSwapChain -> imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = rtrxDev.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t> (commandBuffers.size());

		if (vkAllocateCommandBuffers(rtrxDev.device(), &allocInfo, commandBuffers.data()) != 
			VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers!");
		}

		/*for (int i = 0; i < commandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("Failed to begin recording command buffer");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = rtrxSwapChain -> getRenderPass();
			renderPassInfo.framebuffer = rtrxSwapChain -> getFrameBuffer(i);
			
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = rtrxSwapChain -> getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 10.f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t> (clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			RtrxPipeline->bind(commandBuffers[imageIndex]);
			RtrxModel->bind(commandBuffers[imageIndex]);
			RtrxModel->draw(commandBuffers[imageIndex]);

			vkCmdEndRenderPass(commandBuffers[imageIndex]);
			if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to record command buffer");
			}
		}*/
	}

	void Tracer::recordCommandBuffer(int imageIndex) {

		static int frame = 0;
		static int len = 360;
		frame = (frame + 1) % len;

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = RtrxSwapChain->getRenderPass();
		renderPassInfo.framebuffer = RtrxSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = RtrxSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t> (clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float> (RtrxSwapChain -> getSwapChainExtent().width);
		viewport.height = static_cast<float> (RtrxSwapChain -> getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, RtrxSwapChain -> getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		RtrxPipeline->bind(commandBuffers[imageIndex]);
		RtrxModel->bind(commandBuffers[imageIndex]);

		SimplePushConstantData push{};
		push.offset = { 0.0f, 0.0f };
		float pi = 3.141592;
		float radians = static_cast<float> ((frame % 121) * 2 * pi / 360);
		if (frame < len / 3 + 1) {
			push.colour = { 0.0f + sin(3*radians/4), 0.0f, 1.0f - sin(3*radians/4)};
		} else if (frame < len * 2 / 3 + 1) {
			push.colour = { 1.0f - sin(3*radians/4), 0.0f + sin(3*radians/4), 0.0f };
		} else {
			push.colour = { 0.0f, 1.0f - sin(3*radians/4), 0.0f + sin(3*radians/4) };
		}

		vkCmdPushConstants(commandBuffers[imageIndex],
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0, sizeof(SimplePushConstantData),
			&push);
		RtrxModel->draw(commandBuffers[imageIndex]);

		//for (int j = 0; j < 4; j++) {
		//	SimplePushConstantData push{};
		//	push.offset = { -0.5f + frame * 0.008f, -0.4f + j * 0.25f };
		//	push.colour = { 0.0f + frame * 0.009f, 0.1f + frame * 0.008f, 0.2f + 0.2f * j - frame * 0.002f};

		//	vkCmdPushConstants(commandBuffers[imageIndex],
		//		pipelineLayout,
		//		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		//		0, sizeof(SimplePushConstantData),
		//		&push);
		//	RtrxModel->draw(commandBuffers[imageIndex]);
		//}


		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer");
		}
	}

	void Tracer::drawFrame() {
		uint32_t imageIndex;
		auto result = RtrxSwapChain -> acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swap chain image");
		}

		recordCommandBuffer(imageIndex);
		result = RtrxSwapChain -> submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || 
			rtrxWindow.wasWindowResized()) {
			rtrxWindow.resetWindowRezisedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swap chain image");
		}
	}
}