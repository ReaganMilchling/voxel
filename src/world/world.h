#pragma once

#include "../engine/shader.h"
#include "chunk.h"
#include <mutex>

class World
{
public:
	int m_size;
	World(int s);
	~World();
	void generate();
	void generatechunk(int x, int y);
	void Render(Shader& shader);
	void renderNaive(Shader& shader);
	float getBlockInWorld(int w_x, int w_z, int x, int y, int z);
	float getBlockInWorld(int x, int y, int z);
private:
	std::vector<Chunk*> m_chunks;
	std::mutex m_chunks_mutex;
	void update();
};
