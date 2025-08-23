#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <ostream>
#include <thread>

#include "../log.h"

class VertexBuffer {
private:
	unsigned int m_RenderedID;
public:
	VertexBuffer()
	{
		//std::cout <<  std::this_thread::get_id() << " rendID: " << m_RenderedID << std::endl;
		glGenBuffers(1, &m_RenderedID);
	}

	~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RenderedID);
	}

	void AddBufferData(const void* data, unsigned int size)
	{
		LOGF("pid:%zu - render id: %d", std::this_thread::get_id(), m_RenderedID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RenderedID);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}

	void Bind() const
	{
		//std::cout << m_RenderedID << std::endl;
		glBindBuffer(GL_ARRAY_BUFFER, m_RenderedID);
	}

	void Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
};
