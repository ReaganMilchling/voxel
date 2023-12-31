#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <thread>

#include "world.h"
#include "chunk.h"


World::World(Camera* camera, int s, int renderDistance)
{
    m_size = s;
    m_render_distance = renderDistance;
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
            //std::cout << std::this_thread::get_id() << " :coords: " << i << " : " << j << std::endl;
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
    std::pair p(x, y);

    try 
    {
        Chunk* chunk = m_loaded_chunk_map.at(p);
        m_viewable_chunk_map[p] = chunk;
    } 
    catch (std::out_of_range) 
    {
        if (m_loaded_chunk_map.size() > 250) {return;}
        Chunk *togen = new Chunk(x, y, this);

        m_chunks_mutex.lock();
        m_loaded_chunk_map[p] = togen;
        m_viewable_chunk_map[p] = togen;
        m_chunks_mutex.unlock();
    }
}

void World::update()
{
    glm::vec2 new_pos((int)(m_camera_pos->x/xz), (int)(m_camera_pos->z/xz));
    if (new_pos != m_previous_pos)
    {
        //std::cout << new_pos.x << "-" << new_pos.y << std::endl;
        m_viewable_chunk_map.clear();
        
        int neg_dist = m_render_distance * -1;
        for (int i = neg_dist; i <= m_render_distance; ++i)
        {
            for (int j = neg_dist; j <= m_render_distance; ++j)
            {
                this->moveChunk(new_pos.x + i, new_pos.y + j);
            }
        }        
        m_previous_pos = new_pos;
    }

}

float World::getBlockInWorld(int w_x, int w_z, int x, int y, int z)
{
    std::pair p(w_x, w_z);
    try 
    {
        Chunk* chunk = m_loaded_chunk_map.at(p);
        return chunk->get(x, y, z);
    } 
    catch (std::out_of_range) 
    {
        return -1.0f;
    }

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
