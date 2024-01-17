#pragma once

#include <GLFW/glfw3.h>

#include "GLFWException.h"

class Display
{
private:
	GLFWwindow* window;
	int m_Width;
	int m_Height;
	const char* m_Title;

public:
	Display(int width, int height, const char* title) :
		m_Width(width),
		m_Height(height),
		m_Title(title)
	{
		if (!glfwInit())
			throw new GLFWException("GLFW couldn't be initialized!");

		window = glfwCreateWindow(width, height, title, NULL, NULL);

		if (!window)
		{
			glfwTerminate();
			throw new GLFWException("GLFW window creation failed!");
		}

		glfwMakeContextCurrent(window);
	}

	~Display()
	{
		glfwTerminate();
	}

	int windowShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	void setSwapInterval(int interval)
	{
		glfwSwapInterval(interval);
	}

	void swapBuffers()
	{
		glfwSwapBuffers(window);
	}

	void pollEvents()
	{
		glfwPollEvents();
	}
};