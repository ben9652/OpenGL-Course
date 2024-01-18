#include "Square.h"
#include "Vertex.h"

Square::Square(float sideLength, bool hasTexture, bool centered, float x, float y) :
	m_SideLength(sideLength), m_Centered(centered), m_HasTexture(hasTexture), m_X(x), m_Y(y)
{
	m_Triangles_Qnty = 2;
	m_Vertices_Qnty = 4;
	BuildShape();
}

void Square::BuildShape()
{
	m_Positions = m_HasTexture ? new float[2 * m_Vertices_Qnty * 2] : new float[2 * m_Vertices_Qnty];
	m_Indexes = new unsigned int[3 * m_Triangles_Qnty];

	unsigned int i = 0;
	
	if (m_HasTexture)
	{
		if (m_Centered)
		{
			// Coordenada X e Y de vértice inferior izquierdo
			m_Positions[0] = m_X - m_SideLength / 2.0f;
			m_Positions[1] = m_Y - m_SideLength / 2.0f;

			// Coordenada X e Y de vértice superior izquierdo
			m_Positions[4] = m_X - m_SideLength / 2.0f;
			m_Positions[5] = m_Y + m_SideLength / 2.0f;

			// Coordenada X e Y de vértice superior derecho
			m_Positions[8] = m_X + m_SideLength / 2.0f;
			m_Positions[9] = m_Y + m_SideLength / 2.0f;

			// Coordenada X e Y de vértice inferior derecho
			m_Positions[12] = m_X + m_SideLength / 2.0f;
			m_Positions[13] = m_Y - m_SideLength / 2.0f;

		}
		else
		{
			// Coordenada X e Y de vértice inferior izquierdo
			m_Positions[0] = m_X;
			m_Positions[1] = m_Y;

			// Coordenada X e Y de vértice superior izquierdo
			m_Positions[4] = m_X;
			m_Positions[5] = m_Y + m_SideLength;

			// Coordenada X e Y de vértice superior derecho
			m_Positions[8] = m_X + m_SideLength;
			m_Positions[9] = m_Y + m_SideLength;

			// Coordenada X e Y de vértice inferior derecho
			m_Positions[12] = m_X + m_SideLength;
			m_Positions[13] = m_Y;
		}

		m_Positions[2] = 0.0f;			m_Positions[3] = 0.0f;
		m_Positions[6] = 0.0f;			m_Positions[7] = m_SideLength;
		m_Positions[10] = m_SideLength; m_Positions[11] = m_SideLength;
		m_Positions[14] = m_SideLength; m_Positions[15] = 0.0f;
	}
	else
	{
		if (m_Centered)
		{
			// Coordenada X e Y de vértice inferior izquierdo
			m_Positions[i++] = m_X - m_SideLength / 2.0f;
			m_Positions[i++] = m_Y - m_SideLength / 2.0f;

			// Coordenada X e Y de vértice superior izquierdo
			m_Positions[i++] = m_X - m_SideLength / 2.0f;
			m_Positions[i++] = m_Y + m_SideLength / 2.0f;

			// Coordenada X e Y de vértice superior derecho
			m_Positions[i++] = m_X + m_SideLength / 2.0f;
			m_Positions[i++] = m_Y + m_SideLength / 2.0f;

			// Coordenada X e Y de vértice inferior derecho
			m_Positions[i++] = m_X + m_SideLength / 2.0f;
			m_Positions[i++] = m_Y - m_SideLength / 2.0f;
		}
		else
		{
			// Coordenada X e Y de vértice inferior izquierdo
			m_Positions[i++] = m_X;
			m_Positions[i++] = m_Y;

			// Coordenada X e Y de vértice superior izquierdo
			m_Positions[i++] = m_X;
			m_Positions[i++] = m_Y + m_SideLength;

			// Coordenada X e Y de vértice superior derecho
			m_Positions[i++] = m_X + m_SideLength;
			m_Positions[i++] = m_Y + m_SideLength;

			// Coordenada X e Y de vértice inferior derecho
			m_Positions[i++] = m_X + m_SideLength;
			m_Positions[i++] = m_Y;
		}
	}

	m_Indexes[0] = 0;
	m_Indexes[1] = 1;
	m_Indexes[2] = 3;
	m_Indexes[3] = 1;
	m_Indexes[4] = 2;
	m_Indexes[5] = 3;
}
