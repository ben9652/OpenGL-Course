#include "Shape.h"

float* Shape::GetPositions() const
{
    return m_Positions;
}

unsigned int* Shape::GetIndexes() const
{
    return m_Indexes;
}

unsigned int Shape::GetTriangles() const
{
    return m_Triangles_Qnty;
}

unsigned int Shape::GetVertices() const
{
    return m_Vertices_Qnty;
}
