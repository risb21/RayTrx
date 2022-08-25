#include "tracer.hpp"

// std
#include <stdexcept>

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
		}
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

	void Tracer::createCommandBuffers() {}
	void Tracer::drawFrame() {}
}