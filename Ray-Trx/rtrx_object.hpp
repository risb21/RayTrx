#pragma once

#include "rtrx_model.hpp"

// std
#include <memory>

namespace rtrx {

	struct Trasnform2dComponent {
		glm::vec2 translation{}; // position offset
		glm::vec2 scale{ 1.f, 1.f };
		float rotation;
		glm::mat2 mat2() {
			const float s = sin(rotation);
			const float c = cos(rotation);

			glm::mat2 rotMat{ {c, s}, {-s, c} };
			glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} }; // matrix, column wise
			return rotMat * scaleMat;
		}
	};

	class rtrxObject {
		public:
			using id_t = unsigned int;

			static rtrxObject createObject() {
				static id_t currentId = 0;
				return rtrxObject(currentId++);
			}

			rtrxObject(const rtrxObject&) = delete;
			rtrxObject& operator = (const rtrxObject&) = delete;
			rtrxObject(rtrxObject&&) = default;
			rtrxObject& operator = (rtrxObject&&) = default;

			const id_t getId() { return id; }

			std::shared_ptr<rtrxModel> model{};
			glm::vec3 colour{};
			Trasnform2dComponent transform2d{};

		private:
			rtrxObject(id_t objId) : id(objId) {}
			id_t id;
	};
}