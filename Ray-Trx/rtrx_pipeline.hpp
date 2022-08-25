#pragma once

#include "rtrx_device.hpp"

// std
#include <string>
#include <vector>

namespace rtrx {

	struct PipelineConfigInfo {
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colourBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colourBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderpass = nullptr;
		uint32_t subpass = 0;
	};

	class rtrxPipeline {

		public:
			rtrxPipeline(rtrxDevice& device,
						 const std::string& vertfilepath,
						 const std::string& fragfilepath,
						 const PipelineConfigInfo& configInfo);
			~rtrxPipeline();

			rtrxPipeline(const rtrxPipeline&) = delete;
			void operator=(const rtrxPipeline&) = delete;

			static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
		
		private:
			static std::vector<char>  readFile(const std::string& filepath);
			void createGraphicsPipeline(const std::string& vertfilepath,
										const std::string& fragfilepath,
										const PipelineConfigInfo& configInfo);
			void createShaderModule(const std::vector<char> code, VkShaderModule* shaderModule);
			
			rtrxDevice& rtrxDev;
			VkPipeline graphicsPipeline;
			VkShaderModule vertShaderModule;
			VkShaderModule fragShaderModule;
	};
}