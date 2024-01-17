#pragma once
#include "Shape.h"

class Circle : public Shape
{
private:
	float m_Radio;

public:
	Circle(float radio);

	float GetRadio();

private:
	void BuildShape();
};