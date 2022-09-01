#pragma once

#include "rtrx_device.hpp"

// std
#include <string>
#include <vector>

namespace rtrx {

	struct PipelineConfigInfo {
		PipelineConfigInfo (const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo() = default;

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colourBlendAttachments;
		VkPipelineColorBlendStateCreateInfo colourBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
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
			rtrxPipeline& operator=(const rtrxPipeline&) = delete;
			rtrxPipeline() = default;

			void bind(VkCommandBuffer commandBuffer);
			static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
		
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