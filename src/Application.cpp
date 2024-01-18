#include <iostream>
#include "GL/glew.h"

#include "Display.h"
#include "shapes/Square.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

#include "VertexArray.h"

#include "Shader.h"

#include "Renderer.h"

#include "Texture.h"

int main(void)
{
    Display window(500, 500, "Hello World!");

    window.setSwapInterval(1);

    GLenum err = glewInit();
    if (err != GLEW_OK)
        std::cerr << "Error! " << glewGetErrorString(err) << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    
    Square shape(1.0f, true);
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

    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    Texture texture("res/textures/TheCherno.png");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0); // En el segundo parámetro tiene que ir lo mismo que va de parámetro en texture.Bind()

    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();

    //std::cout << (unsigned char*)"There're " << triangles_qnty << " triangles to draw" << std::endl;

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
