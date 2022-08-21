#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace rtrx {
	class rtrxWindow {

		public:
			rtrxWindow(int w, int h, std::string n);
			~rtrxWindow();

			bool shouldClose() { return glfwWindowShouldClose(window); }

		private:
			void initWindow();

			const int width;
			const int height;

			std::string windowName;
			GLFWwindow *window;
	};
}