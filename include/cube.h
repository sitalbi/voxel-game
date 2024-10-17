#pragma once

namespace voxl {
	enum class BlockType {
		Default,
		Grass,
		Dirt,
		Water,
		Stone,
		Sand,
		Wood
	};

	class Cube {
	public:
		Cube(BlockType type);
		~Cube();

		BlockType getType();

	private:
		BlockType type;
	};
}; // namespace voxl