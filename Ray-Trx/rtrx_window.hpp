#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace rtrx {
	class rtrxWindow {

		public:
			rtrxWindow(int w, int h, std::string n);
			~rtrxWindow();

			rtrxWindow(const rtrxWindow &) = delete;
			rtrxWindow &operator=(const rtrxWindow &) = delete;

			bool shouldClose() { return glfwWindowShouldClose(window); }
			VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; };
			bool wasWindowResized() { return framebufferResized; }
			void resetWindowRezisedFlag() { framebufferResized = false; }

			void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		private:
			static void framebufferResizedCallback(GLFWwindow* window, int width, int height);
			void initWindow();

			int width;
			int height;

			bool framebufferResized = false;

			std::string windowName;
			GLFWwindow *window;
	};
}