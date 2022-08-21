#include "tracer.hpp"

// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main() {

	rtrx::Tracer app{};

	try {
		app.run();
	} catch (const std::exception &e) {
		std::cerr << e.what() << " Oh god pls help me" << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}