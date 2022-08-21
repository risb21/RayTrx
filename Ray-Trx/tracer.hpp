#pragma once

#include "rtrx_window.hpp"

namespace rtrx {
	class Tracer {
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HEIGHT = 600;

			void run();

		private:
			rtrxWindow rtrxWindow{ WIDTH, HEIGHT, "Hello how r u i am under d wotor it is too much raining" };
	};
}