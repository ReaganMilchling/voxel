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


World::World(Camera* camera, int renderDistance)
{
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
    update();
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
                if ((std::abs(i) == m_render_distance) && (std::abs(j) == m_render_distance))
                {
                }
                else 
                {
                    this->moveChunk(new_pos.x + i, new_pos.y + j);
                }
            }
        }        
        m_previous_pos = new_pos;
    }

}

float World::getBlockInWorld(int w_x, int w_z, int x, int y, int z)
{
    if (y >= y_max || y < 0) { return -1.0f; }

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
    if (y >= y_max || y < 0) { return -1.0f; }

    int w_x = x / xz;
    int w_z = z / xz;
    int t_x = x % xz;
    int t_z = z % xz;

    if (t_x < 0) 
    {
        if (w_x == 0) {
            w_x = -1;
        } else {
            t_x = xz + t_x;
        }
    }

    if (t_z < 0) 
    {
        if (w_z == 0) {
            w_z = -1;
        } else {
            t_z = xz + t_z;
        }
    }
    std::cout << w_x << ":" << w_z << " - " << t_x << ":" << y << ":" << t_z << std::endl;
    std::pair p(w_x, w_z);
    try 
    {
        Chunk* chunk = m_loaded_chunk_map.at(p);
        return chunk->get(t_x, y, t_z);
    } 
    catch (std::out_of_range) 
    {
        return -1.0f;
    }
}
