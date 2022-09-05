#pragma once

#include "rtrx_window.hpp"
#include "rtrx_pipeline.hpp"
#include "rtrx_swap_chain.hpp"
#include "rtrx_device.hpp"
#include "rtrx_model.hpp"
#include "rtrx_object.hpp"

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
			void Sierpinski(std::vector<rtrxModel::Vertex> &verts,
				int depth,
				glm::vec2 left,
				glm::vec2 right,
				glm::vec2 top);
			void loadObjects();
			void createPipelineLayout();
			void createPipeline();
			void createCommandBuffers();
			void freeCommandBuffers();
			void drawFrame();
			void recreateSwapChain();
			void recordCommandBuffer(int imageIndex);
			void renderObjects(VkCommandBuffer commandBuffer);


			rtrxWindow rtrxWindow{ WIDTH, HEIGHT, "Hello how r u i am under d wotor it is too much raining" };
			rtrxDevice rtrxDev{ rtrxWindow };
			std::unique_ptr<rtrxSwapChain> RtrxSwapChain;
			std::unique_ptr<rtrxPipeline> RtrxPipeline;
			VkPipelineLayout pipelineLayout;
			std::vector<VkCommandBuffer> commandBuffers;
			std::vector<rtrxObject> Objects;
	};
}