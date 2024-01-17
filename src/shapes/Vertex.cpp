#include "Vertex.h"
#include <math.h>

#define TOLERANCE_ERROR 1e-4f

Vertex::Vertex(float x, float y) : m_X(x), m_Y(y) {}

Vertex::Vertex(const Vertex& vertex) : m_X(vertex.m_X), m_Y(vertex.m_Y) {}

bool operator==(const Vertex& v1, const Vertex& v2)
{
    bool xs_iguales = fabsf(v1.GetX() - v2.GetX()) < TOLERANCE_ERROR ? true : false;
    bool ys_iguales = fabsf(v1.GetY() - v2.GetY()) < TOLERANCE_ERROR ? true : false;

    return xs_iguales && ys_iguales;
}

bool operator!=(const Vertex& v1, const Vertex& v2)
{
    bool xs_distintos = fabsf(v1.GetX() - v2.GetY()) < TOLERANCE_ERROR ? false : true;
    bool ys_distintos = fabsf(v1.GetY() - v2.GetY()) < TOLERANCE_ERROR ? false : true;

    return xs_distintos || ys_distintos;
}

float Vertex::GetX() const
{
    return m_X;
}

float Vertex::GetY() const
{
    return m_Y;
}

const Vertex& Vertex::GetRotatedVertex(float angle) const
{
    float new_x = m_X * cosf(angle) - m_Y * sinf(angle);
    float new_y = m_X * sinf(angle) + m_Y * cosf(angle);

    return Vertex(new_x, new_y);
}
