#pragma once

class Vertex
{
private:
	float m_X;
	float m_Y;

public:
	Vertex(float x, float y);

	Vertex(const Vertex& vertex);

	float GetX() const;
	float GetY() const;

	const Vertex& GetRotatedVertex(float angle) const;

	friend bool operator==(const Vertex& v1, const Vertex& v2);
	friend bool operator!=(const Vertex& v1, const Vertex& v2);
};