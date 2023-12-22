#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <thread>

#include "VertexArray.h"


VertexArray::VertexArray()
{
	//std::cout <<  std::this_thread::get_id() << " : " << m_RendererID << std::endl;
	glGenVertexArrays(1, &m_RendererID);
	//GLCall(glBindVertexArray(m_RendererID));
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	//std::cout <<  std::this_thread::get_id() << " : " << m_RendererID << std::endl;
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
		glEnableVertexAttribArray(i);
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::Unbind() const
{
	glBindVertexArray(0);
}
