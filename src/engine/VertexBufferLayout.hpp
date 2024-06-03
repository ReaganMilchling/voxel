#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0) {}
/**
 * gcc doesn't like this and I don't know enough about templates to fix it
 * upon updating to llvm17, clang uses libstdc++ not libc++ and I can't figure out how to fix that
	template<typename T>
	void Push(unsigned int count)
	{
		//static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		VertexBufferElement element = { GL_FLOAT, count, GL_FALSE };
		m_Elements.push_back(element);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		VertexBufferElement element = { GL_UNSIGNED_INT, count, GL_FALSE };
		m_Elements.push_back(element);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		VertexBufferElement element = { GL_UNSIGNED_BYTE, count, GL_TRUE };
		m_Elements.push_back(element);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}
**/
	void Push(unsigned int count)
	{
		VertexBufferElement element = { GL_FLOAT, count, GL_FALSE };
		m_Elements.push_back(element);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};
