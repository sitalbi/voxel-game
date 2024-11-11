#include "cube.h"

voxl::Cube::Cube(voxl::BlockType type, glm::vec3 position)
{
	m_type = type;
	m_position = position;
}

voxl::Cube::~Cube()
{
}

voxl::BlockType voxl::Cube::getType()
{
	return m_type;
}

glm::vec3 voxl::Cube::getPosition()
{
	return m_position;
}
