#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <ostream>
#include <thread>

#include "VertexBuffer.h"


VertexBuffer::VertexBuffer()
{
	std::cout <<  std::this_thread::get_id() << " rendID: " << m_RenderedID << std::endl;
	glGenBuffers(1, &m_RenderedID);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_RenderedID);
}

void VertexBuffer::AddBufferData(const void* data, unsigned int size)
{
	std::cout <<  std::this_thread::get_id() << " rendID: " << m_RenderedID << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderedID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
	//std::cout << m_RenderedID << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderedID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
