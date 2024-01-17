#pragma once

class IndexBuffer
{
private:
	// Identificador para todo tipo de objeto de OpenGL que asignamos a memoria
	unsigned int m_RendererID;
	unsigned int m_Count;

public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};