#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "world.h"
#include "chunk.h"


World::World(int x, int y)
{
    for (int32_t i = -x; i < x; ++i) 
    {
        for (int32_t j = -y; j < y; ++j) {
            m_chunks.push_back(new Chunk(i, j, this));
        }
    }
}

World::~World()
{
    for (unsigned int i = 0; i < m_chunks.size(); ++i)
    {
        m_chunks.at(i)->~Chunk();
    }
}

void World::Render(Shader& shader)
{
    for (unsigned int i = 0; i < m_chunks.size(); ++i)
    {
        m_chunks.at(i)->Render(shader);
    }
}

void World::update()
{

}

float World::getBlockInWorld(int w_x, int w_z, int x, int y, int z)
{
    for (Chunk* c : m_chunks) {
        if (c->getX() == w_x && c->getZ() == w_z)
        {
            return c->get(x, y, z);
        }
    }
    return -1.0f;
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
