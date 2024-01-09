#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK)
        std::cerr << "Error! " << glewGetErrorString(err) << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    /* Creo un buffer para almacenar v�rtices */
    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    /*
        Describir� los atributos en orden:
            - index: especifica el �ndice del atributo del v�rtice gen�rico a ser modificado
                     Resulta que los v�rtices, recordemos, no son posiciones, sino todo un objeto
                     con diferentes propiedades y caracter�sticas. Por ejemplo: posici�n, textura, normal.
                     En este caso, consideramos a la posici�n como nuestra primera propiedad, as� que el
                     �ndice ser� 0 en este caso.
            - size: especifica el n�mero de componentes que tiene el atributo del v�rtice gen�rico. En este
                    caso, como estamos considerando una posici�n de un v�rtice bidimensional, tenemos que
                    son 2 las componentes que conforman la posici�n del v�rtice.
            - type: especifica el tipo de dato de cada componente en el arreglo. En nuestro caso, el tipo
                    de dato que tiene cada componente es un float.
            - normalized: se especifica si los datos deber�an ser normalizados o no. En este caso no necesitamos
                          hacer esto porque float ya es un tipo de dato normalizado. Pero explico mejor esto:
                          Digamos que estamos especificando un byte cuyos valores van del 0 al 255, que suele ser
                          la manera de especificar el color. Eso necesita ser normalizado entre 0 y 1 como un float
                          en nuestro shader.
            - stride: especifica el offset de byte entre atributos consecutivos de un v�rtice gen�rico. En otras
                      palabras, es la cantidad de bytes que hay entre cada v�rtice. Volvamos al ejemplo en el que
                      tengo los atributos:
                       * "posici�n", que es un atributo de 3 componentes (supongamos que estamos en la 3ra dimensi�n);
                       * "textura", que es un atributo que tiene 2 componentes;
                       * y "normal", siendo un atributo de 3 componentes.
                      El primer atributo tiene 12 bytes (al ser 3 floats); el segundo, 8 bytes; y el tercer atributo,
                      12 bytes. Eso suma un total de 32 bytes. Y ese es nuestro stride. Es b�sicamente el tama�o de
                      cada v�rtice.
                      La raz�n por la que es importante esto es porque, si queremos saltar de un v�rtice al otro, es
                      importante saber cu�nto debe ser el offset en bytes para esto.
            - pointer: es el puntero hacia el atributo real. Supongamos de nuevo que tengo los atributos mencionados
                       y nos centremos en solo UN v�rtice. �Cu�l es el offset entonces de cada uno de esos atributos?
                       De "posici�n", ser� 0 claramente; de "textura", 12; y de "normal", 20. Eso representa este
                       atributo, y como te dar�s cuenta es un puntero lo que hay que escribir, as� que haz un casteo
                       si hace falta.
    */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Dibujo los tri�ngulos. El tercer par�metro cuenta realmente los v�rtices, es decir, los pares (x,y) de cada v�rtice. */
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}