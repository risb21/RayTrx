#include "rtrx_pipeline.hpp"
#include "rtrx_model.hpp"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace rtrx {

	rtrxPipeline::rtrxPipeline(rtrxDevice& device,
							   const std::string& vertfilepath,
							   const std::string& fragfilepath,
							   const PipelineConfigInfo& configInfo)
							   : rtrxDev{ device } {
		createGraphicsPipeline(vertfilepath, fragfilepath, configInfo);
	}

	rtrxPipeline::~rtrxPipeline() {
		vkDestroyShaderModule(rtrxDev.device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(rtrxDev.device(), fragShaderModule, nullptr);
		vkDestroyPipeline(rtrxDev.device(), graphicsPipeline, nullptr);
	}

	std::vector<char> rtrxPipeline::readFile(const std::string& filepath) {
		// input filestream variable, seek at the end, read as binary
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void rtrxPipeline::createGraphicsPipeline(
		const std::string& vertfilepath,
		const std::string& fragfilepath,
		const PipelineConfigInfo& configInfo) {

		assert(
			configInfo.pipelineLayout != VK_NULL_HANDLE &&
			"Cannot create graphics Pipeline:: no pipelinelayout provided in configInfo");
		assert(
			configInfo.renderpass != VK_NULL_HANDLE &&
			"Cannot create graphics Pipeline:: no renderpass provided in configInfo");
		auto vertCode = readFile(vertfilepath);
		auto fragCode = readFile(fragfilepath);

		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		auto bindingDescriptions = rtrxModel::Vertex::getBindingDescriptions();
		auto attributeDescriptions = rtrxModel::Vertex::getAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t> (attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t> (bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &configInfo.viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colourBlendInfo;
		// pipelineInfo.pColorBlendState = &colourBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = &configInfo.dynamicStateInfo;
		
		pipelineInfo.layout = configInfo.pipelineLayout;
		pipelineInfo.renderPass = configInfo.renderpass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(rtrxDev.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create graphics pipeline");
		}
	}

	void rtrxPipeline::createShaderModule(const std::vector<char> code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(rtrxDev.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create shader module");
		}
	}

	void rtrxPipeline::bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	void rtrxPipeline::defaultPipelineConfigInfo(PipelineConfigInfo& configInfo) {

		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		// Assigning info on viewport
		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1;
		configInfo.viewportInfo.pViewports = nullptr;
		configInfo.viewportInfo.scissorCount = 1;
		configInfo.viewportInfo.pScissors = nullptr;

		// Rasterization stage
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;		  // Decides which face is being shown
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // Iterate through vertices and form triangle CW
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;			  // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		// Multisampling for Anti-Aliasing
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.minSampleShading = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;			  // Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;			  // Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;		  // Optional
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		// Colour blending settings
		configInfo.colourBlendAttachments.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		configInfo.colourBlendAttachments.blendEnable = VK_FALSE;
		configInfo.colourBlendAttachments.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;     // Optional  
		configInfo.colourBlendAttachments.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;	// Optional
		configInfo.colourBlendAttachments.colorBlendOp = VK_BLEND_OP_ADD;                // Optional
		configInfo.colourBlendAttachments.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;		// Optional
		configInfo.colourBlendAttachments.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;	// Optional
		configInfo.colourBlendAttachments.alphaBlendOp = VK_BLEND_OP_ADD;				// Optional
		
		// Assigning blending settings attachment
		configInfo.colourBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colourBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colourBlendInfo.logicOp = VK_LOGIC_OP_COPY;	  // Optional
		configInfo.colourBlendInfo.attachmentCount = 1;
		configInfo.colourBlendInfo.pAttachments = &configInfo.colourBlendAttachments;
		configInfo.colourBlendInfo.blendConstants[0] = 0.0f;	  // Optional
		configInfo.colourBlendInfo.blendConstants[1] = 0.0f;	  // Optional
		configInfo.colourBlendInfo.blendConstants[2] = 0.0f;	  // Optional
		configInfo.colourBlendInfo.blendConstants[3] = 0.0f;	  // Optional

		// Depth buffer info
		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;		  // Optional
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;		  // Optional
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};					  // Optional
		configInfo.depthStencilInfo.back = {};					  // Optional

		configInfo.dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamicStateInfo.pDynamicStates = configInfo.dynamicStateEnables.data();
		configInfo.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t> (configInfo.dynamicStateEnables.size());
		configInfo.dynamicStateInfo.flags = 0;
	}
}