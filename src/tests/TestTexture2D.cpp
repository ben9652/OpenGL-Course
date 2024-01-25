#include "TestTexture2D.h"

#include "Renderer.h"

#include "imgui/imgui.h"

#include "shapes/Square.h"

namespace test {

	TestTexture2D::TestTexture2D() :
        m_TranslationA(200, 200, 0),
        m_TranslationB(400, 400, 0),
        m_Proj(glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 0.0f)),
        m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)))
	{
        m_Shape = std::make_unique<Square>(100.0f, true, true);
        unsigned int triangles_qnty = m_Shape->GetTriangles();
        unsigned int vertices_qnty = m_Shape->GetVertices();

        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        GLCall(glEnable(GL_BLEND));

        m_VAO = std::make_unique<VertexArray>();

        m_VertexBuffer = std::make_unique<VertexBuffer>(m_Shape->GetPositions(), 2 * vertices_qnty * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_VAO->AddBuffer(*m_VertexBuffer, layout);

        m_IndexBuffer = std::make_unique<IndexBuffer>(m_Shape->GetIndexes(), 3 * triangles_qnty);

        m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
        m_Shader->Bind();
        m_Shader->SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        m_Texture = std::make_unique<Texture>("res/textures/TheCherno.png");
        m_Shader->SetUniform1i("u_Texture", 0);
	}

	TestTexture2D::~TestTexture2D()
	{
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture2D::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

        Renderer renderer;

        m_Texture->Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            glm::mat4 mvp = m_Proj * m_View * model;
            m_Shader->Bind();
            m_Shader->SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
        }
	}

	void TestTexture2D::OnImGuiRender()
	{
        ImGui::SliderFloat("TranslationA X", &m_TranslationA.x, 0.0f, 960.0f);
        ImGui::SliderFloat("TranslationA Y", &m_TranslationA.y, 0.0f, 540.0f);

        ImGui::SliderFloat("TranslationB X", &m_TranslationB.x, 0.0f, 960.0f);
        ImGui::SliderFloat("TranslationB Y", &m_TranslationB.y, 0.0f, 540.0f);
	}

}
