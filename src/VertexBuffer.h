#pragma once

class VertexBuffer
{
private:
	// Identificador para todo tipo de objeto de OpenGL que asignamos a memoria
	unsigned int m_RendererID;

public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};