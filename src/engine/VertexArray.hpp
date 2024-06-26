#pragma once

#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

class VertexArray {
private:
	unsigned int m_RendererID;
public:
	VertexArray()
	{
		//std::cout <<  std::this_thread::get_id() << " : " << m_RendererID << std::endl;
		glGenVertexArrays(1, &m_RendererID);
		//GLCall(glBindVertexArray(m_RendererID));
	}

	~VertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
	{
		//std::cout <<  std::this_thread::get_id() << " : " << m_RendererID << std::endl;
		Bind();
		vb.Bind();
		const auto& elements = layout.GetElements();
		size_t offset = 0;

		for (unsigned int i = 0; i < elements.size(); i++)
		{
			const auto& element = elements[i];
			glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
			glEnableVertexAttribArray(i);
			offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
		}
	}

	void Bind() const
	{

		glBindVertexArray(m_RendererID);
	}

	void Unbind() const
	{

		glBindVertexArray(0);
	}
};
