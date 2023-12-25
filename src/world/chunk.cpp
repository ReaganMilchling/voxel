#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <ostream>
#include <thread>

#include "chunk.h"
#include "world.h"

#include "../engine/VertexArray.h"
#include "../engine/VertexBuffer.h"

Chunk::Chunk(uint32_t x, uint32_t z, World* world)
{
    m_x = x;
    m_z = z;
    m_world = world;
    m_changed = true;
    m_vertexCount = 0;
    
    //gen();
    std::thread t1(&Chunk::gen, this);
    t1.detach();
}

Chunk::~Chunk()
{
    
}

void Chunk::update()
{
    if (m_changed)
    {
        m_VA.Unbind();
        m_VB.Unbind();

        //std::cout << "adding mesh" << std::endl;
        // recreate mesh
        for (int i = 0; i < m_horizontal_max; ++i)
        {
            for (int j = 0; j < m_y_max; ++j)
            {
                for (int k = 0; k < m_horizontal_max; ++k)
                {
                    std::vector<std::vector<float>> temp = getVisibleFaces(i, j, k);

                    for (std::vector<float> v : temp)
                    {
                        m_vertexCount += 6;
                        for (float f : v)
                        {
                            m_mesh.push_back(f);
                        }
                    }
                }
            }
        }

        // Calls custom made opengl api
        // Most code courtesy of TheCherno
        m_VB.AddBufferData(&m_mesh[0], m_mesh.size() * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(2);
        layout.Push<float>(1);
        m_VA.AddBuffer(m_VB, layout);
        //std::cout << std::this_thread::get_id() << " :meshSize: " << m_mesh.size() << std::endl;

        // update changed
        m_changed = false;
    }
}

void Chunk::Render(Shader &shader)
{
    if (this->m_chunk_mutex.try_lock())
    {
        if (this->m_mesh.size() == 0) 
            this->m_changed = true;
        update();
        //std::cout << m_mesh.size() << std::endl;
        //std::cout << std::this_thread::get_id() << " : " << m_vertexCount << std::endl;

        shader.Bind();
        m_VB.Bind();
        m_VA.Bind();

        glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
        this->m_chunk_mutex.unlock();
    } 
}

void Chunk::renderNaive(Shader &shader)
{
    update();

    shader.Bind();

}

void Chunk::gen()
{
    float scale = 50.0f;
    float persistence = 0.4f;
    float lacunarity = 1.5f;
    int octaves = 8;

    this->m_chunk_mutex.lock();
    // ground generation
    for (int x = 0; x < m_horizontal_max; ++x)
    {
        for (int z = 0; z < m_horizontal_max; ++z)
        {
            float amplitude = 1.1f;
            float frequency = 1.5f;
            float noiseHeight = 0.0f;

            for (int i = 0; i < octaves; ++i)
            {
                float x_coord = (x + (m_horizontal_max * this->m_x)) / scale * frequency;
                float z_coord = (z + (m_horizontal_max * this->m_z)) / scale * frequency;
                
                glm::vec2 coord(x_coord, z_coord);
                float perlinValue = glm::perlin(coord);
                //std::cout << x_coord << ":" << z_coord << " - "  << coord.x << ":" << coord.y << " - " << perlinValue << std::endl;
                noiseHeight += perlinValue * amplitude;

                amplitude *= persistence;
                frequency *= lacunarity;
            }

            int y = noiseHeight * (5.0f*this->m_y_max/16.0f) + (11.0f*this->m_y_max/16.0f);

            if (y >= m_y_max)
                y = m_y_max - 1;
            if (y < 0)
                y = 0;

            if (y > m_y_max * 15.0f/16.0f)
            {
                this->chunk[x][y][z] = 5.0f;
                this->chunk[x][y-1][z] = 5.0f;
                this->chunk[x][y-2][z] = 5.0f;
            }
            else if (y > m_y_max * 14.0f/16.0f)
            {
                this->chunk[x][y][z] = 1.0f;
                this->chunk[x][y-1][z] = 1.0f;
                this->chunk[x][y-2][z] = 1.0f;
            }
            else if (y > m_y_max * 13.0f/16.0f)
            {
                this->chunk[x][y][z] = 2.0f;
                this->chunk[x][y-1][z] = 2.0f;
                this->chunk[x][y-2][z] = 2.0f;
            }
            else if (y > m_y_max * 8.0f/16.0f)
            {
                this->chunk[x][y][z] = 3.0f;
                this->chunk[x][y-1][z] = 2.0f;
                this->chunk[x][y-2][z] = 2.0f;
            }
            else
            {
                this->chunk[x][y][z] = 2.0f;                
                this->chunk[x][y-1][z] = 2.0f;
                this->chunk[x][y-2][z] = 2.0f;
            }

            for (int i = 0; i < this->m_y_max; ++i)
            {
                if (i > y) {
                    this->chunk[x][i][z] = 0.0f;
                }
                if (i < y - 2) {
                    this->chunk[x][i][z] = 1.0f;
                }
            } 
            
            //cavegen(x, y, z);
        }
    }
    this->m_chunk_mutex.unlock();
}

void Chunk::cavegen(int x, int y_max, int z)
{
    // cave gen
    // to be used only by this class - should make private or synchronize properly
    for (int y = 0; y <= y_max; ++y)
    {
        float scale = 0.025f;
        float x_coord = (x + (m_horizontal_max * this->m_x));
        float z_coord = (z + (m_horizontal_max * this->m_z));
        float y_coord = y;
        float inv = 1.0f / (y);

        glm::vec3 p(x_coord * scale, y_coord * scale, z_coord * scale);
        float value = glm::simplex(p) + inv;
        //std::cout << value << std::endl;

        if (value < 0.0f)
        {
            this->chunk[x][y][z] = 0.0f;
        }

    }

}

std::vector<std::vector<float>> Chunk::getVisibleFaces(int i, int j, int k)
{
    std::vector<std::vector<float>> ret;

    if (chunk[i][j][k] == 0)
        return ret;

    if ((i != m_horizontal_max - 1 && chunk[i + 1][j][k] == 0) || (i == m_horizontal_max - 1 && m_world->getBlockInWorld(m_x+1, m_z, 0, j, k) == 0))
    {
        std::vector<float> temp = {
            //left  
            0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
            0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
            0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
            0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
            0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k],
            0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if ((i != 0 && chunk[i - 1][j][k] == 0) || (i == 0 && m_world->getBlockInWorld(m_x-1, m_z, m_horizontal_max -1, j, k) == 0))
    {
        std::vector<float> temp = {
            //right
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if (j != 0 && chunk[i][j - 1][k] == 0)
    {
        //This wasn't working properly but top(y-1) did
        //std::vector<float> temp = {
        //    //bottom
        //    -0.5f + (float)i + (float)(16 * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(16 * m_z),  0.0f, 1.0f,
        //     0.5f + (float)i + (float)(16 * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(16 * m_z),  1.0f, 1.0f,
        //     0.5f + (float)i + (float)(16 * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(16 * m_z),  1.0f, 0.0f,
        //     0.5f + (float)i + (float)(16 * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(16 * m_z),  1.0f, 0.0f,
        //    -0.5f + (float)i + (float)(16 * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(16 * m_z),  0.0f, 0.0f,
        //    -0.5f + (float)i + (float)(16 * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(16 * m_z),  0.0f, 1.0f,
        //};
        std::vector<float> temp = {
            //top
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j-1, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j-1, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j-1,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j-1,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j-1,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j-1, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k]
        };
        ret.push_back(temp);

    }

    if (j == m_y_max - 1 || chunk[i][j + 1][k] == 0)
    {
        std::vector<float> temp = {
            //top
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if ((k != 0 && chunk[i][j][k - 1] == 0) || (k == 0 && m_world->getBlockInWorld(m_x, m_z-1, i, j, m_horizontal_max -1) == 0))
    {
        std::vector<float> temp = {
            //front
            -0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j, -0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if ((k != m_horizontal_max - 1 && chunk[i][j][k + 1] == 0) || (k == m_horizontal_max - 1 && m_world->getBlockInWorld(m_x, m_z+1, i, j, 0) == 0))
    {
        std::vector<float> temp = {
            //back
            -0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 0.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
             0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  1.0f, 1.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x),  0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 1.0f, (float)chunk[i][j][k],
            -0.5f + (float)i + (float)(m_horizontal_max * m_x), -0.5f + (float)j,  0.5f + (float)k + (float)(m_horizontal_max * m_z),  0.0f, 0.0f, (float)chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    return ret;
}
