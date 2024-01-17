#include "Circle.h"
#include "Vertex.h"
#include <vector>

#define NUM_PI 3.14159265358979323846f
#define ROTATION_ANGLE NUM_PI/360.0f

Circle::Circle(float radio) : m_Radio(radio)
{
	BuildShape();
}

float Circle::GetRadio()
{
	return m_Radio;
}

void Circle::BuildShape()
{
    std::vector<float> positions;
    std::vector<unsigned int> indices;

    // Creación del vértice central y 1 vértice inicial desde el cual ir formando los triángulos.
    positions.insert(positions.end(), { 0.0f , 0.0f });
    positions.insert(positions.end(), { 0.0f , m_Radio });

    unsigned int i_center_vertex = 0;
    unsigned int i_last_vertex = 1;

    Vertex vertex1(0.0f, m_Radio);

    m_Triangles_Qnty = 0;
    m_Vertices_Qnty = 2;

    while (true)
    {
        // Obtengo el último vértice para rotarlo
        Vertex last_vertex(positions[2 * i_last_vertex], positions[2 * i_last_vertex + 1]);

        // Creo el nuevo vértice rotado respecto al último
        Vertex new_vertex = last_vertex.GetRotatedVertex(ROTATION_ANGLE);

        m_Triangles_Qnty++;

        if (new_vertex == vertex1)
        {
            // Cargo el último índice necesario
            indices.insert(indices.end(), { i_center_vertex, i_last_vertex, 1 });
            break;
        }
        else
        {
            // Cargo el único vértice nuevo que necesito para el nuevo triángulo
            positions.insert(positions.end(), { new_vertex.GetX(), new_vertex.GetY() });
            m_Vertices_Qnty++;

            // Cargo los índices de los vértices que conforman el nuevo triángulo
            indices.insert(indices.end(), { i_center_vertex, i_last_vertex, i_last_vertex++ + 1 });
        }
    }

    size_t p_size = positions.size();
    size_t i_size = indices.size();

    m_Positions = new float[p_size];
    m_Indexes = new unsigned int[i_size];

    memcpy(m_Positions, &positions[0], p_size * sizeof(float));
    memcpy(m_Indexes, &indices[0], i_size * sizeof(unsigned int));
}