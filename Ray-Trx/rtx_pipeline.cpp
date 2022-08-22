#include "rtrx_pipeline.hpp"

//std
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace rtrx {

	rtrxPipeline::rtrxPipeline(const std::string& vertfilepath, const std::string& fragfilepath) {
		createGraphicsPipeline(vertfilepath, fragfilepath);
	}

	std::vector<char> rtrxPipeline::readFile(const std::string& filepath) {
		// input filestream variable, seek at the end, read as binary
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("Failed to open file: " + filepath);
		}

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();
		return buffer;
	}

	void rtrxPipeline::createGraphicsPipeline(const std::string& vertfilepath, const std::string& fragfilepath) {
		auto vertCode = readFile(vertfilepath);
		auto fragCode = readFile(fragfilepath);

		std::cout << "Vertex shader code size: " << vertCode.size() << "\n";
		std::cout << "Fragment shader code size: " << fragCode.size() << "\n";
	}
}