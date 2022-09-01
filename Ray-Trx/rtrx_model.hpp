#pragma once

#include "rtrx_device.hpp"

// libs
#define	GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>

namespace rtrx {
	class rtrxModel {
		public:

			struct Vertex {
				glm::vec2 position;
				glm::vec3 colour; // interleaving colour bind in buffer
				static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
				static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
			};

			rtrxModel(rtrxDevice& device, const std::vector<Vertex>& vertices);
			~rtrxModel();

			rtrxModel(const rtrxModel&) = delete;
			rtrxModel& operator = (const rtrxModel&) = delete;

			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);

		private:
			void createVertexBuffers(const std::vector<Vertex>& vertices);

			rtrxDevice& RtrxDevice;
			VkBuffer vertexBuffer;
			VkDeviceMemory vertexBufferMemory;
			uint32_t vertexCount;
	};
}