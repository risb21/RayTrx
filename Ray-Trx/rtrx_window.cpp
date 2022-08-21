#include "rtrx_window.hpp"

namespace rtrx{

	rtrxWindow::rtrxWindow(int w, int h, std::string n) : width{ w }, height{ h }, windowName{ n } {
		initWindow();
	}

	rtrxWindow::~rtrxWindow() {  // Destructor method
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void rtrxWindow::initWindow() {  // Defining window initializer

		glfwInit();  // Initializes glfw
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// ^ Stops glfw from creating an OpenGL ctx, from GLFW_NO_API
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		// ^ Stops resizing of window

		// Window ptr defined in .hpp file of same name
		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		// nullptr for windowed mode, name of window needs c style string
	}
}
