#pragma once

#include <string>
#include <vector>

namespace rtrx {
	class rtrxPipeline {

		public:
			rtrxPipeline(const std::string& vertfilepath, const std::string& fragfilepath);
		
		private:
			static std::vector<char>  readFile(const std::string& filepath);
			void createGraphicsPipeline(const std::string& vertfilepath, const std::string& fragfilepath);
	};
}