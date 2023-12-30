#pragma once

#include "../engine/camera.h"
#include "../engine/shader.h"
#include "chunk.h"
#include <map>
#include <mutex>

class World
{
public:
	int m_size;
	Camera* m_camera;
	glm::vec3* m_camera_pos;
	
	World(Camera*, int s);
	~World();
	void generate();
	void generatechunk(int x, int y);
	void Render(Shader& shader);
	void renderNaive(Shader& shader);
	void update();
	float getBlockInWorld(int w_x, int w_z, int x, int y, int z);
	float getBlockInWorld(int x, int y, int z);
private:
	glm::vec2 m_previous_pos;
	std::vector<Chunk*> m_chunks;
	std::map<std::pair<int, int>, Chunk*> m_viewable_chunk_map; //indexed relative to player
	std::map<std::pair<int, int>, Chunk*> m_loaded_chunk_map; //indexed absolutely
	std::mutex m_chunks_mutex;
	void moveChunk(int x, int y);
};
