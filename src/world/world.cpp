#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <ostream>
#include <thread>

#include "world.h"
#include "chunk.h"


World::World(Camera* camera, int s)
{
    m_size = s;
    m_camera = camera;
    m_camera_pos = &camera->Position;
    m_previous_pos = glm::vec2((int)(m_camera_pos->x/xz), (int)(m_camera_pos->z/xz));
}

World::~World()
{
    for (unsigned int i = 0; i < m_chunks.size(); ++i)
    {
        m_chunks.at(i)->~Chunk();
    }
}

void World::generate()
{
    for (int32_t i = 0; i < m_size; ++i) 
    {
        for (int32_t j = 0; j < m_size; ++j) {
            Chunk *togen = new Chunk(i, j, this);
            
            std::pair<int, int> pair(i, j);

            m_chunks_mutex.lock();
            m_chunks.push_back(togen);
            m_loaded_chunk_map[pair] = togen;
            m_viewable_chunk_map[pair] = togen;
            m_chunks_mutex.unlock();
            std::cout << std::this_thread::get_id() << " :coords: " << i << " : " << j << std::endl;
        }
    }
}

void World::generatechunk(int x, int y)
{
    Chunk *togen = new Chunk(x, y, this);
    m_chunks.push_back(togen);
    //std::cout << std::this_thread::get_id() << " :coords: " << x << " : " << y << std::endl;
}

void World::Render(Shader& shader)
{
    //std::cout << m_viewable_chunk_map.size() << std::endl;
    m_chunks_mutex.lock();
    for (auto const& [pos, chunk] : m_viewable_chunk_map)
    {
        //std::cout << std::this_thread::get_id() << " :coords: " << pos.first << " : " << pos.second << std::endl;
        chunk->render(shader);
    }
    m_chunks_mutex.unlock();
}

void World::moveChunk(int x, int y)
{
    Chunk* chunk = m_loaded_chunk_map.at(std::pair(x, y));
    if (chunk != nullptr)
    {

    }
    else 
    {
    
    }
}

void World::update()
{
    glm::vec2 new_pos((int)(m_camera_pos->x/xz), (int)(m_camera_pos->z/xz));
    if (new_pos != m_previous_pos)
    {
        //std::cout << new_pos.x << "-" << new_pos.y << std::endl;
        m_viewable_chunk_map.clear();
        m_viewable_chunk_map[std::pair(new_pos.x, new_pos.y)] = m_loaded_chunk_map[std::pair(new_pos.x, new_pos.y)];
        m_viewable_chunk_map[std::pair(new_pos.x, new_pos.y-1)] = m_loaded_chunk_map[std::pair(new_pos.x, new_pos.y-1)];
        m_viewable_chunk_map[std::pair(new_pos.x, new_pos.y+1)] = m_loaded_chunk_map[std::pair(new_pos.x, new_pos.y+1)];
        m_viewable_chunk_map[std::pair(new_pos.x-1, new_pos.y)] = m_loaded_chunk_map[std::pair(new_pos.x-1, new_pos.y)];
        m_viewable_chunk_map[std::pair(new_pos.x-1, new_pos.y-1)] = m_loaded_chunk_map[std::pair(new_pos.x-1, new_pos.y-1)];
        m_viewable_chunk_map[std::pair(new_pos.x-1, new_pos.y+1)] = m_loaded_chunk_map[std::pair(new_pos.x-1, new_pos.y+1)];
        m_viewable_chunk_map[std::pair(new_pos.x+1, new_pos.y-1)] = m_loaded_chunk_map[std::pair(new_pos.x+1, new_pos.y-1)];
        m_viewable_chunk_map[std::pair(new_pos.x+1, new_pos.y+1)] = m_loaded_chunk_map[std::pair(new_pos.x+1, new_pos.y+1)];
        m_viewable_chunk_map[std::pair(new_pos.x+1, new_pos.y)] = m_loaded_chunk_map[std::pair(new_pos.x+1, new_pos.y)];
        m_previous_pos = new_pos;
    }

}

float World::getBlockInWorld(int w_x, int w_z, int x, int y, int z)
{
    for (Chunk* c : m_chunks) {
        if (c->getX() == w_x && c->getZ() == w_z)
        {
            return c->get(x, y, z);
        }
    }
    return 0.0f;
}

float World::getBlockInWorld(int x, int y, int z)
{
    int w_x = x / 16;
    int w_z = z / 16;
    int t_x = x % 16;
    int t_z = z % 16;
    for (Chunk* c : m_chunks) {
        if (c->getX() == w_x && c->getZ() == w_z)
        {
            return c->get(t_x, y, t_z);
        }
    }
    return -1.0f;
}
