#include "rtrx_pipeline.hpp"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace rtrx {

	rtrxPipeline::rtrxPipeline(rtrxDevice& device,
							   const std::string& vertfilepath,
							   const std::string& fragfilepath,
							   const PipelineConfigInfo& configInfo)
							   : rtrxDev{ device } {
		createGraphicsPipeline(vertfilepath, fragfilepath, configInfo);
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

	void rtrxPipeline::createGraphicsPipeline(const std::string& vertfilepath,
											  const std::string& fragfilepath,
											  const PipelineConfigInfo& configInfo) {
		auto vertCode = readFile(vertfilepath);
		auto fragCode = readFile(fragfilepath);

		std::cout << "Vertex shader code size: " << vertCode.size() << "\n";
		std::cout << "Fragment shader code size: " << fragCode.size() << "\n";
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

	PipelineConfigInfo rtrxPipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
		PipelineConfigInfo configInfo{};

		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		// Viewport effects can also squish display
		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;

		// Scissor clips off part of the viewport displaying to the window
		configInfo.scissor.offset = { 0,0 };
		configInfo.scissor.extent = { width, height };

		// Assigning info on viewport defined before
		configInfo.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewportInfo.viewportCount = 1;
		configInfo.viewportInfo.pViewports = &configInfo.viewport;
		configInfo.viewportInfo.scissorCount = 1;
		configInfo.viewportInfo.pScissors = &configInfo.scissor;

		// Rasterization stage
		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;			  // Optional
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

		// Multisampling for Anti-Aliasing
		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.minSampleShading = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;			  //Optional
		configInfo.multisampleInfo.pSampleMask = nullptr;			  //Optional
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  //Optional
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;		  //Optional




		return configInfo;
	}
}