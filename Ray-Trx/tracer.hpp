#pragma once

#include "rtrx_window.hpp"
#include "rtrx_pipeline.hpp"
#include "rtrx_swap_chain.hpp"
#include "rtrx_device.hpp"

// std
#include <memory>
#include <vector>

namespace rtrx {
	class Tracer {
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HEIGHT = 600;

			Tracer();
			~Tracer();

			Tracer(const rtrxWindow&) = delete;
			Tracer& operator=(const rtrxWindow&) = delete;

			void run();

		private:
			void createPipelineLayout();
			void createPipeline();
			void createCommandBuffers();
			void drawFrame();


			rtrxWindow rtrxWindow{ WIDTH, HEIGHT, "Hello how r u i am under d wotor it is too much raining" };
			rtrxDevice rtrxDev{ rtrxWindow };
			rtrxSwapChain rtrxSwapChain{ rtrxDev, rtrxWindow.getExtent() };
			/*rtrxPipeline rtrxPipeline{ rtrxDev, 
									   "simple_shader.vert.spv", 
									   "simple_shader.frag.spv",
									   rtrxPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};*/
			std::unique_ptr<rtrxPipeline> RtrxPipeline;
			VkPipelineLayout pipelineLayout;
			std::vector<VkCommandBuffer> commandBuffers;
	};
}