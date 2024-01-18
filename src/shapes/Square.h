#pragma once
#include "Shape.h"

class Square : public Shape
{
private:
	float m_SideLength;
	bool m_Centered;
	bool m_HasTexture;
	float m_X;
	float m_Y;

public:
	/// <summary>
	/// Crear un cuadrado
	/// </summary>
	/// <param name="sideLength">Longitud del lado del cuadrado</param>
	/// <param name="centered">Se determina si el cuadrado está centrado en las coordenadas ingresadas. De no estarlo, su vértice inferior izquierdo tendrá la misma posición que la de las coordenadas.</param>
	/// <param name="x">Coordenada x</param>
	/// <param name="y">Coordenada y</param>
	Square(float sideLength, bool hasTexture = false, bool centered = true, float x = 0, float y = 0);

	void BuildShape() override;
};