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

Chunk::Chunk(int32_t x, int32_t z, World* world)
{
    m_x = x;
    m_z = z;
    chunk_x = x * m_horizontal_max;
    chunk_z = z * m_horizontal_max;
    chunk_x_f = (float)(x * m_horizontal_max);
    chunk_z_f = (float)(z * m_horizontal_max);

    m_world = world;
    m_changed = true;
    m_regen_mesh = true;
    m_vertex_ct = 0;
    m_water_vertex_ct = 0;
    
    //gen();
    std::thread t1(&Chunk::gen, this);
    t1.detach();
}

Chunk::~Chunk()
{
    /*
    for (int x = 0; x < m_horizontal_max; ++x)
    {
        for (int y = 0; y < m_y_max; ++y) 
        {
            delete[] chunk[x][y];
        }
    }
    */
}

void Chunk::update()
{
    if (m_changed)
    {
        m_VA.Unbind();
        m_VB.Unbind();
        m_water_VA.Unbind();
        m_water_VB.Unbind();

        genMesh();

        // Calls custom made opengl api
        // Most code courtesy of TheCherno
        m_VB.AddBufferData(&m_mesh[0], m_mesh.size() * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(2);
        layout.Push<float>(1);
        m_VA.AddBuffer(m_VB, layout);
        
        //std::cout << std::this_thread::get_id() << " :meshSize: " << m_mesh.size() << std::endl;
        m_water_VB.AddBufferData(&m_water_mesh[0], m_water_mesh.size() * sizeof(float));
        m_water_VA.AddBuffer(m_water_VB, layout);

        //std::cout << m_water_vertex_ct << std::endl;
        //std::cout << m_water_mesh.size() << std::endl;

        // update changed
        m_changed = false;
    }
}

void Chunk::genMesh()
{
    if (m_regen_mesh)
    {
        //std::cout << "adding mesh" << std::endl;
        // recreate mesh
        for (int i = 0; i < m_horizontal_max; ++i)
        {
            for (int j = 0; j < m_y_max; ++j)
            {
                for (int k = 0; k < m_horizontal_max; ++k)
                {
                    std::vector<std::vector<float>> temp = getVisibleFaces(i, j, k);
                    std::vector<std::vector<float>> water = getWaterFaces(i, j, k);

                    for (std::vector<float> v : temp)
                    {
                        m_vertex_ct += 6;
                        for (float f : v)
                        {
                            m_mesh.push_back(f);
                        }
                    }
                    for (std::vector<float> v : water)
                    {
                        m_water_vertex_ct += 6;
                        for (float f : v)
                        {
                            m_water_mesh.push_back(f);
                        }
                    }
                }
            }
        }

        //std::cout << std::this_thread::get_id() << " - mesh: " <<  m_vertex_ct << "-" << m_mesh.size() 
        //<< "\twater:" << m_water_vertex_ct << "-" << m_water_mesh.size() << std::endl;

        // update changed
        m_regen_mesh = false;
    }
}

void Chunk::render(Shader &shader)
{
    if (m_chunk_mutex.try_lock())
    {
        if (m_mesh.size() == 0) 
        {
            m_changed = true;
            m_regen_mesh = true;
        }

        update();
        //std::cout << m_mesh.size() << std::endl;
        //std::cout << std::this_thread::get_id() << " : " << m_vertex_ct << std::endl;

        shader.Bind();
        m_VB.Bind();
        m_VA.Bind();
        glDrawArrays(GL_TRIANGLES, 0, m_vertex_ct);

        shader.Bind();
        m_water_VB.Bind();
        m_water_VA.Bind();
        glDrawArrays(GL_TRIANGLES, 0, m_water_vertex_ct);

        m_chunk_mutex.unlock();
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

    m_chunk_mutex.lock();
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
                float x_coord = (x + (m_horizontal_max * m_x)) / scale * frequency;
                float z_coord = (z + (m_horizontal_max * m_z)) / scale * frequency;
                
                glm::vec2 coord(x_coord, z_coord);
                float perlinValue = glm::perlin(coord);
                //std::cout << x_coord << ":" << z_coord << " - "  << coord.x << ":" << coord.y << " - " << perlinValue << std::endl;
                noiseHeight += perlinValue * amplitude;

                amplitude *= persistence;
                frequency *= lacunarity;
            }

            // this functions as a really naive splin/step function
            // maps -1,1 to -.34,2, uses 0 as level 64 for water
            noiseHeight += .41420f;
            if (noiseHeight > 0.0f) {
                noiseHeight *= noiseHeight;
            } else {
                noiseHeight *= noiseHeight;
                noiseHeight = -noiseHeight;
            }

            //int y = noiseHeight * (m_y_max/4.0f);
            int y = (int)(noiseHeight * (m_y_max/4.0f) + (m_y_max/2.0f));

            if (y >= m_y_max)
                y = m_y_max - 1;
            if (y < 0)
                y = 0;
            
            // if else to add 3 deep top layers throughout the world
            if (y > m_y_max * 15.0f/16.0f)
            {
                chunk[x][y][z] = 5.0f;
                chunk[x][y-1][z] = 5.0f;
                chunk[x][y-2][z] = 5.0f;
            }
            else if (y > m_y_max * 14.0f/16.0f)
            {
                chunk[x][y][z] = 1.0f;
                chunk[x][y-1][z] = 1.0f;
                chunk[x][y-2][z] = 1.0f;
            }
            else if (y > m_y_max * 13.0f/16.0f)
            {
                chunk[x][y][z] = 2.0f;
                chunk[x][y-1][z] = 2.0f;
                chunk[x][y-2][z] = 2.0f;
            }
            else if (y > m_y_max * 8.0f/16.0f)
            {
                chunk[x][y][z] = 3.0f;
                chunk[x][y-1][z] = 2.0f;
                chunk[x][y-2][z] = 2.0f;
            }
            else if (y == 64 || y == 63 || y == 62)
            {
                chunk[x][y][z] = 6.0f;
                chunk[x][y-1][z] = 6.0f;
                chunk[x][y-2][z] = 6.0f;
            }
            else
            {
                chunk[x][y][z] = 7.0f;                
                chunk[x][y-1][z] = 7.0f;
                chunk[x][y-2][z] = 7.0f;
            }

            for (int i = 0; i < m_y_max; ++i)
            {
                // purposefully skip 3 blocks to account for top layers
                if (i > y) {
                    if (i < m_y_max/2.0f)
                    {
                        // add water below 64
                        chunk[x][i][z] = 4.0f;
                    } else {
                        chunk[x][i][z] = 0.0f;
                    }
                }
                if (i < y - 2) {
                    chunk[x][i][z] = 1.0f;
                }
            } 
            
            cavegen(x, y, z);
        }
    }
    genMesh();
    m_chunk_mutex.unlock();
}

void Chunk::cavegen(int x, int y_max, int z)
{
    // cave gen
    // to be used only by this class - should make private or synchronize properly
    for (int y = 1; y <= y_max - 3; ++y)
    {
        float scale = 0.025f;
        float x_coord = (x + (m_horizontal_max * m_x));
        float z_coord = (z + (m_horizontal_max * m_z));
        float y_coord = y;
        float inv = 1.0f / (y);

        glm::vec3 p(x_coord * scale, y_coord * scale, z_coord * scale);
        float value = glm::simplex(p);
        //std::cout << value << std::endl;

        if (value < 0.2f && value > 0.0f)
        {
            chunk[x][y][z] = 0.0f;
        }

    }

}

std::vector<std::vector<float>> Chunk::getVisibleFaces(int i, int j, int k)
{
    std::vector<std::vector<float>> ret;

    if (chunk[i][j][k] == 0 || chunk[i][j][k] == 4.0f)
        return ret;

    float x_value = (float)i + chunk_x_f;
    float y_value = (float)j;
    float z_value = (float)k + chunk_z_f;

    if ((i != m_horizontal_max - 1 && chunk[i + 1][j][k] == 0) || (i == m_horizontal_max - 1 && m_world->getBlockInWorld(m_x+1, m_z, 0, j, k) == 0)
        || (i != m_horizontal_max - 1 && chunk[i + 1][j][k] == 4.0f) || (i == m_horizontal_max - 1 && m_world->getBlockInWorld(m_x+1, m_z, 0, j, k) == 4.0f))
    {
        std::vector<float> temp = {
            //left  
           1.0f + x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
           1.0f + x_value,        y_value, 1.0f + z_value,  1.0f, 1.0f, chunk[i][j][k],
           1.0f + x_value,        y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k],
           1.0f + x_value,        y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k],
           1.0f + x_value, 1.0f + y_value,        z_value,  0.0f, 0.0f, chunk[i][j][k],
           1.0f + x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if ((i != 0 && chunk[i - 1][j][k] == 0) || (i == 0 && m_world->getBlockInWorld(m_x-1, m_z, m_horizontal_max -1, j, k) == 0)
        || (i != 0 && chunk[i - 1][j][k] == 4.0f) || (i == 0 && m_world->getBlockInWorld(m_x-1, m_z, m_horizontal_max -1, j, k) == 4.0f))
    {
        std::vector<float> temp = {
            //right
            x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
            x_value, 1.0f + y_value,        z_value,  1.0f, 1.0f, chunk[i][j][k],
            x_value,        y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k],
            x_value,        y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k],
            x_value,        y_value, 1.0f + z_value,  0.0f, 0.0f, chunk[i][j][k],
            x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if ((j != 0 && chunk[i][j - 1][k] == 0) || (j != 0 && chunk[i][j - 1][k] == 4.0f))
    {
        //This wasn't working properly but top(y-1) did
        //std::vector<float> temp = {
        //    //bottom
        //           (float)i + (float)(16 chunk_x_f,        y_value,        (float)k + (float)(16 chunk_z_f,  0.0f, 1.0f,
        //    1.0f + (float)i + (float)(16 chunk_x_f,        y_value,        (float)k + (float)(16 chunk_z_f,  1.0f, 1.0f,
        //    1.0f + (float)i + (float)(16 chunk_x_f,        y_value, 1.0f + (float)k + (float)(16 chunk_z_f,  1.0f, 0.0f,
        //    1.0f + (float)i + (float)(16 chunk_x_f,        y_value, 1.0f + (float)k + (float)(16 chunk_z_f,  1.0f, 0.0f,
        //           (float)i + (float)(16 chunk_x_f,        y_value, 1.0f + (float)k + (float)(16 chunk_z_f,  0.0f, 0.0f,
        //           (float)i + (float)(16 chunk_x_f,        y_value,        (float)k + (float)(16 chunk_z_f,  0.0f, 1.0f,
        //};
        std::vector<float> temp = {
            //top
                   x_value, 1.0f + y_value-1,        z_value,  0.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value-1,        z_value,  1.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value-1, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value-1, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
                   x_value, 1.0f + y_value-1, 1.0f + z_value,  0.0f, 0.0f, chunk[i][j][k],
                   x_value, 1.0f + y_value-1,        z_value,  0.0f, 1.0f, chunk[i][j][k]
        };
        ret.push_back(temp);

    }

    if ((j == m_y_max - 1 || chunk[i][j + 1][k] == 0) || (j == m_y_max - 1 || chunk[i][j + 1][k] == 4.0f))
    {
        std::vector<float> temp = {
            //top
                   x_value, 1.0f + y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k],
                   x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value,        z_value,  0.0f, 0.0f, chunk[i][j][k],
                   x_value, 1.0f + y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if ((k != 0 && chunk[i][j][k - 1] == 0) || (k == 0 && m_world->getBlockInWorld(m_x, m_z-1, i, j, m_horizontal_max -1) == 0)
        || (k != 0 && chunk[i][j][k - 1] == 4.0f) || (k == 0 && m_world->getBlockInWorld(m_x, m_z-1, i, j, m_horizontal_max -1) == 4.0f))
    {
        std::vector<float> temp = {
            //front
                   x_value,        y_value, z_value,  0.0f, 0.0f, chunk[i][j][k],
                   x_value, 1.0f + y_value, z_value,  0.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value, z_value,  1.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value, z_value,  1.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value,        y_value, z_value,  1.0f, 0.0f, chunk[i][j][k],
                   x_value,        y_value, z_value,  0.0f, 0.0f, chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    if ((k != m_horizontal_max - 1 && chunk[i][j][k + 1] == 0) || (k == m_horizontal_max - 1 && m_world->getBlockInWorld(m_x, m_z+1, i, j, 0) == 0)
        || (k != m_horizontal_max - 1 && chunk[i][j][k + 1] == 4.0f) || (k == m_horizontal_max - 1 && m_world->getBlockInWorld(m_x, m_z+1, i, j, 0) == 4.0f))
    {
        std::vector<float> temp = {
            //back
                   x_value,        y_value, 1.0f + z_value,  0.0f, 0.0f, chunk[i][j][k],
            1.0f + x_value,        y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value, 1.0f + y_value, 1.0f + z_value,  1.0f, 1.0f, chunk[i][j][k],
                   x_value, 1.0f + y_value, 1.0f + z_value,  0.0f, 1.0f, chunk[i][j][k],
                   x_value,        y_value, 1.0f + z_value,  0.0f, 0.0f, chunk[i][j][k]
        };
        ret.push_back(temp);
    }

    return ret;
}

std::vector<std::vector<float>> Chunk::getWaterFaces(int i, int j, int k)
{
    std::vector<std::vector<float>> ret;

    if (chunk[i][j][k] != 4.0f)
        return ret;
    
    float x_value = (float)i + chunk_x_f;
    float y_value = (float)j;
    float z_value = (float)k + chunk_z_f;
    
    if ((j == m_y_max - 1 || chunk[i][j + 1][k] == 0))
    {

        std::vector<float> temp = {
            //top
                   x_value,  0.8f + y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k],
                   x_value,  0.8f + y_value, 1.0f + z_value,  1.0f, 1.0f, chunk[i][j][k],
            1.0f + x_value,  0.8f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
            1.0f + x_value,  0.8f + y_value, 1.0f + z_value,  1.0f, 0.0f, chunk[i][j][k],
            1.0f + x_value,  0.8f + y_value,        z_value,  0.0f, 0.0f, chunk[i][j][k],
                   x_value,  0.8f + y_value,        z_value,  0.0f, 1.0f, chunk[i][j][k]
        };
        ret.push_back(temp);
    }
    return ret;
}
