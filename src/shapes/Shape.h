#pragma once

class Shape
{
protected:
    // Arreglo de números que, si se los toma por tuplas, son las coordenadas de un vértice
    float* m_Positions;

    // Índices para apuntar a cada vértice, es decir, a cada tupla
    unsigned int* m_Indexes;

    // Cantidad total de triángulos que forman la figura
    unsigned int m_Triangles_Qnty;

    // Cantidad total de vértices que forman a figura
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