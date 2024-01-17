#include <iostream>
#include "GL/glew.h"

#include "Display.h"
#include "shapes/Circle.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

#include "VertexArray.h"

#include "Shader.h"

#include "Renderer.h"

int main(void)
{
    Display window(500, 500, "Hello World!");

    window.setSwapInterval(1);

    GLenum err = glewInit();
    if (err != GLEW_OK)
        std::cerr << "Error! " << glewGetErrorString(err) << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    
    Circle circle(0.5f);
    unsigned int triangles_qnty = circle.GetTriangles();
    unsigned int vertices_qnty = circle.GetVertices();

    VertexArray va;
    VertexBuffer vb(circle.GetPositions(), 2 * vertices_qnty * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(circle.GetIndexes(), 3 * triangles_qnty);

    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();

    std::cout << (unsigned char*)"There're " << triangles_qnty << " triangles to draw" << std::endl;

    Renderer renderer;

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!window.windowShouldClose())
    {
        renderer.Clear();

        shader.Bind();
        shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

        renderer.Draw(va, ib, shader);
        
        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;

        r += increment;

        /* Swap front and back buffers */
        window.swapBuffers();

        /* Poll for and process events */
        window.pollEvents();
    }

    return 0;
}
