#include "cube.h"

voxl::Cube::Cube(voxl::BlockType type)
{
	this->type = type;
}

voxl::Cube::~Cube()
{
}

voxl::BlockType voxl::Cube::getType()
{
	return type;
}
