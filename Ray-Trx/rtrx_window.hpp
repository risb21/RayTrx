#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace rtrx {
	class rtrxWindow {

		public:
			rtrxWindow(int w, int h, std::string n);
			~rtrxWindow();

			// rtrxWindow(const rtrxWindow &) = delete;
			// rtrxWindow &operator=(const rtrxWindow &) = delete;

			bool shouldClose() { return glfwWindowShouldClose(window); }
			void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		private:
			void initWindow();

			const int width;
			const int height;

			std::string windowName;
			GLFWwindow *window;
	};
}