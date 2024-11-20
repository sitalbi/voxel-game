#pragma once
#include "mesh.h"
#include <glm/glm.hpp>

namespace voxl {
	enum class BlockType {
		None = 0,
		Grass,
		Dirt,
		Stone,
		Sand,
		Wood,
		Water,
		Leaves
	};

	class Cube {
	public:
		Cube(BlockType type, glm::vec3 position);
		~Cube();

		BlockType getType();
		glm::vec3 getPosition();

	private:
		BlockType m_type;

		glm::vec3 m_position;
	};
}; // namespace voxl