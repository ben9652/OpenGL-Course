#include <iostream>
#include "GL/glew.h"

#include "Display.h"
#include "shapes/Square.h"

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"

int main(void)
{
    Display window(960, 540, "Hello World!");

    window.setSwapInterval(1);

    GLenum err = glewInit();
    if (err != GLEW_OK)
        std::cerr << "Error! " << glewGetErrorString(err) << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    
    Square shape(100.0f, true, true);
    unsigned int triangles_qnty = shape.GetTriangles();
    unsigned int vertices_qnty = shape.GetVertices();

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));

    VertexArray va;
    VertexBuffer vb(shape.GetPositions(), 2 * vertices_qnty * 2 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(shape.GetIndexes(), 3 * triangles_qnty);

    glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 0.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,0));

    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    Texture texture("res/textures/TheCherno.png");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0); // En el segundo parámetro tiene que ir lo mismo que va de parámetro en texture.Bind()

    //std::cout << (unsigned char*)"There're " << triangles_qnty << " triangles to draw" << std::endl;

    Renderer renderer;

    glm::vec3 translation(200, 200, 0);

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!window.windowShouldClose())
    {
        renderer.Clear();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 mvp = proj * view * model;

        window.ImGui_NewFrame();

        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
        shader.SetUniformMat4f("u_MVP", mvp);

        renderer.Draw(va, ib, shader);
        
        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;

        r += increment;
        
        {
            static float f = 0.0f;

            ImGui::Begin("Hello, world!");

            ImGui::SliderFloat("Translation X", &translation.x, 0.0f, 960.0f);
            ImGui::SliderFloat("Translation Y", &translation.y, 0.0f, 540.0f);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        window.swapBuffers();

        /* Poll for and process events */
        window.pollEvents();
    }

    return 0;
}
