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


World::World(int s)
{
    this->m_size = s;
}

World::~World()
{
    for (unsigned int i = 0; i < this->m_chunks.size(); ++i)
    {
        this->m_chunks.at(i)->~Chunk();
    }
}

void World::generate()
{
    for (int32_t i = 0; i < this->m_size; ++i) 
    {
        for (int32_t j = 0; j < this->m_size; ++j) {
            Chunk *togen = new Chunk(i, j, this);
            
            this->m_chunks_mutex.lock();
            this->m_chunks.push_back(togen);
            this->m_chunks_mutex.unlock();
            //std::cout << std::this_thread::get_id() << " :coords: " << i << " : " << j << std::endl;
        }
    }
}

void World::generatechunk(int x, int y)
{
    Chunk *togen = new Chunk(x, y, this);
    this->m_chunks.push_back(togen);
    //std::cout << std::this_thread::get_id() << " :coords: " << x << " : " << y << std::endl;
}

void World::Render(Shader& shader)
{
    //std::cout << m_chunks.size() << std::endl;
    this->m_chunks_mutex.lock();
    for (unsigned int i = 0; i < this->m_chunks.size(); ++i)
    {
        this->m_chunks.at(i)->Render(shader);
    }
    this->m_chunks_mutex.unlock();
}

void World::update()
{

}

float World::getBlockInWorld(int w_x, int w_z, int x, int y, int z)
{
    for (Chunk* c : this->m_chunks) {
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
    for (Chunk* c : this->m_chunks) {
        if (c->getX() == w_x && c->getZ() == w_z)
        {
            return c->get(t_x, y, t_z);
        }
    }
    return -1.0f;
}
