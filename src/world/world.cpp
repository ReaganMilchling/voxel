#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <ostream>
#include <stdexcept>

#include "world.h"
#include "chunk.h"
#include "../log.h"


World::World(Camera* camera, int renderDistance, ThreadPool* pool)
{
    m_render_distance = renderDistance;
    m_camera = camera;
    m_camera_pos = &camera->Position;
    m_previous_pos = glm::vec2((int)(m_camera_pos->x/xz), (int)(m_camera_pos->z/xz));
    m_pool = pool;

    this->generate();
    //std::thread t1(&World::generate, &world);
    //std::cout << std::this_thread::get_id() << std::endl;
    //std::thread t1(&World::generatechunk, &world, 1, 0);
    LOG("world generated");
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
    int neg_dist = m_render_distance * -1;
    for (int i = neg_dist; i <= m_render_distance; ++i)
    {
        for (int j = neg_dist; j <= m_render_distance; ++j)
        {
            Chunk *togen = new Chunk(i, j, this);
            std::pair p(i, j);
            //m_chunks_mutex.lock();
            m_loaded_chunk_map[p] = togen;
            m_viewable_chunk_map[p] = togen;
            //m_chunks_mutex.unlock();
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
    //m_chunks_mutex.lock();
    for (auto& [pos, chunk] : m_viewable_chunk_map)
    {
        //LOGF("coords: %d:%d", pos.first, pos.second);
        chunk->render(shader);
    }
    //m_chunks_mutex.unlock();
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
        if (m_loaded_chunk_map.size() > 500) {return;}
        Chunk *togen = new Chunk(x, y, this);

        //m_chunks_mutex.lock();
        m_loaded_chunk_map[p] = togen;
        m_viewable_chunk_map[p] = togen;
        //m_chunks_mutex.unlock();
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
                    //std::cout << new_pos.x + i << "=x - z="<<new_pos.y+j << "\n";
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

float World::getBlockInWorld(float x, float y, float z)
{
    if (y >= y_max || y < 0) { return -1.0f; }

    int w_x = (int)x / xz;
    int t_x = (int)x % xz;

    int w_z = (int)z / xz;
    int t_z = (int)z % xz;

    // this adjusts negative values since w_z,w_x is bottom left and goes toward 0,0
    // decrementing world x,y prevents needing -0
    if (x < 0.0l)
    {
        w_x -= 1;
        t_x += (xz - 1);
    }

    if (z < 0.0l)
    {
        w_z -= 1;
        t_z += (xz - 1);
    } 

    //std::cout << w_x << ":" << w_z << " - " << t_x << ":" << t_z<< " ------ " << x << ":" << y << ":" << z << std::endl;
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
