#pragma once

class VertexBuffer
{
private:
	unsigned int m_RenderedID;
public:
	VertexBuffer();
	~VertexBuffer();

	void AddBufferData(const void* data, unsigned int size);

	void Bind() const;
	void Unbind() const;
};