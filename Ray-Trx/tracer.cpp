#include "tracer.hpp"

namespace rtrx {
	void Tracer::run() {
		while (!rtrxWindow.shouldClose()) {
			glfwPollEvents(); // keystrokes, clicking X button etc
		}
	}
}