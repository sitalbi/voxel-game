#include "cube.h"

namespace voxl {

Cube::Cube(BlockType type, glm::vec3 position)
{
	m_type = type;
	m_position = position;
}

Cube::~Cube()
{
}

BlockType Cube::getType()
{
	return m_type;
}

glm::vec3 Cube::getPosition()
{
	return m_position;
}

} // namespace voxl