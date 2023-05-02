#pragma once

#include "../engine/shader.h"
#include "../engine/VertexArray.h"
#include "../engine/VertexBuffer.h"

#include "block.h"
#include <vector>

class World;

const int xz = 16;
const int y = 64;

class Chunk
{
public:
	Chunk(uint32_t, uint32_t, World*);
	~Chunk();
	inline int32_t getX() { return m_x; };
	inline int32_t getZ() { return m_z; };
	inline float get(int x, int y, int z) { return chunk[x][y][z]; };
	void Render(Shader&);
private:
	int32_t m_x, m_z;
	int m_horizontal_max = xz;
	int m_y_max = y;
	float chunk[xz][y][xz];
	bool m_changed;

	VertexArray m_VA;
	VertexBuffer m_VB;

	World* m_world;
	uint32_t m_vertexCount;
	std::vector<float> m_mesh;
	std::vector<std::vector<float>> getVisibleFaces(int x, int y, int z);
	void update();
};