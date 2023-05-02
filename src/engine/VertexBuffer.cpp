#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VertexBuffer.h"


VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &m_RenderedID);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_RenderedID);
}

void VertexBuffer::AddBufferData(const void* data, unsigned int size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderedID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RenderedID);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}