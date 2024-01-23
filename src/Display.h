#pragma once

#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

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

		ImGui::CreateContext();
		const char* glsl_version = "#version 100";
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		ImGui::StyleColorsDark();
	}

	~Display()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
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

	void ImGui_NewFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
};