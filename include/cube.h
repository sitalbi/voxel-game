#pragma once

namespace voxl {
	enum class BlockType {
		Grass,
		Dirt,
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