#pragma once

#include "../engine/shader.h"
#include "../engine/VertexArray.hpp"
#include "../engine/VertexBuffer.hpp"

#include <mutex>
#include <vector>

class World;

const int xz = 32;
const int y_max = 128;

class Chunk
{
public:
	Chunk(int32_t, int32_t, World*);
	~Chunk();

	inline int32_t getX() { return m_x; };
	inline int32_t getZ() { return m_z; };
	inline float get(int x, int y, int z) { return chunk[x][y][z]; };
	
	void render(Shader&);
	void renderNaive(Shader& shader);
	void gen();
private:
	int32_t m_x, m_z;
	float chunk_x_f, chunk_z_f;
	int32_t chunk_x, chunk_z;
	
	float m_scale = 1.0f;
	
	int m_horizontal_max = xz;
	int m_y_max = y_max;
	
	World* m_world;
	std::mutex m_chunk_mutex;
	float chunk[xz][y_max][xz];
	bool m_changed;
	bool m_regen_mesh;

	VertexArray m_VA;
	VertexBuffer m_VB;
	uint32_t m_vertex_ct;
	std::vector<float> m_mesh;
	std::vector<std::vector<float>> getVisibleFaces(int x, int y, int z);

	VertexArray m_water_VA;
	VertexBuffer m_water_VB;
	uint32_t m_water_vertex_ct;
	std::vector<float> m_water_mesh;
	std::vector<std::vector<float>> getWaterFaces(int x, int y, int z);
	
	void update();
	void genMesh();
	void cavegen(int x, int y, int z);
};
