#pragma once

class Shape
{
protected:
    // Arreglo de n�meros que, si se los toma por tuplas, son las coordenadas de un v�rtice
    float* m_Positions;

    // �ndices para apuntar a cada v�rtice, es decir, a cada tupla
    unsigned int* m_Indexes;

    // Cantidad total de tri�ngulos que forman la figura
    unsigned int m_Triangles_Qnty;

    // Cantidad total de v�rtices que forman a figura
    unsigned int m_Vertices_Qnty;

public:
    float* GetPositions() const;
    unsigned int* GetIndexes() const;
    unsigned int GetTriangles() const;
    unsigned int GetVertices() const;

protected:
    Shape() = default;

    virtual void BuildShape() = 0;
};