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

		return configInfo;
	}
}