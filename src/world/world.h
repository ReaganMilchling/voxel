#pragma once

#include "../engine/shader.h"
#include "chunk.h"

class World
{
public:
	World(int x, int y);
	~World();
	void Render(Shader& shader);
	float getBlockInWorld(int w_x, int w_z, int x, int y, int z);
	float getBlockInWorld(int x, int y, int z);
private:
	std::vector<Chunk*> m_chunks;
	void update();
};