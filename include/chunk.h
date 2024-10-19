#include "cube.h"
#include "vector"

namespace voxl
{

class Chunk {
public:
	Chunk(int x, int y, int z);
	~Chunk();

	void generate();
	void update();

	static const int CHUNK_SIZE = 16;

	std::vector<Cube> cubes;

private:
	int m_x, m_y, m_z;

	std::vector<float> m_vertices; // Chunk's mesh vertices

};

} // namespace voxl