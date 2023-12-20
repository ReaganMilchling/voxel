#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>
#include <iostream>
#include <ostream>

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

    // terrain gen
    for (int i = 0; i < m_horizontal_max; ++i)
    {
        for (int k = 0; k < m_horizontal_max; ++k)
        {
            for (int y = 0; y < m_y_max; ++y)
            {
                float scale = 0.025f;
                float x_coord = (i + (m_horizontal_max * x));
                float z_coord = (k + (m_horizontal_max * z));
                float y_coord = y;
                float inv = 1.0f / (y);

                glm::vec3 p(x_coord * scale, y_coord * scale, z_coord * scale);
                float value = glm::simplex(p) + inv;
                //std::cout << value << std::endl;

                if (value >= 0.0f)
                {
                    if (y > m_y_max * 1.0f/3.0f) 
                    {
                        if (y > m_y_max * 2.0f/3.0f)
                        {
                            this->chunk[i][y][k] = 1.0f;
                        }
                        else 
                        {
                            this->chunk[i][y][k] = 2.0f;
                        }
                    }
                    else {
                        this->chunk[i][y][k] = 3.0f;
                    }
                }
                else 
                {
                    this->chunk[i][y][k] = 0.0f;
                }

            }
        }
    }
}

Chunk::~Chunk()
{
    
}

void Chunk::update()
{
    if (m_changed)
    {
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

        // update changed
        m_changed = false;
    }
}

void Chunk::Render(Shader &shader)
{
    update();

    shader.Bind();
    m_VB.Bind();
    m_VA.Bind();

    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
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
