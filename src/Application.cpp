#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();

    /*
        - shader: id que especifica el gestor del objeto del shader cuyo c�digo fuente ser� reeplazado
        - count: el n�mero de c�digos fuente que estamos especificando
        - string: especifica el array de punteros a strigns conteniendo el c�digo fuente a ser cargado al shader
        - lenght: especifica una longitud para el array de strings. Si se pone un NULL aqu� se asume a cada string como que termina con un caracter nulo
    */
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" <<
            std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

/// <summary>
/// El prop�sito de esta funci�n es proveer a OpenGL el c�digo fuente del shader real,
/// y queremos que OpenGL compile ese programa y enlace los dos juntos en un �nico
/// programa que ser�a el shader completo.
/// </summary>
/// <param name="vertexShader"></param>
/// <param name="fragmentShader"></param>
/// <returns>Alg�n identificador �nico para ese shader creado para que podamos luego asociarlo y usarlo</returns>
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    float positions[] = {
        -0.5f, -0.5f,
        -0.5f,  0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f
    };

    // Los primeros tres �ndices corresponden a los 3 v�rtices del primer tri�ngulo
    // Los segundos tres �ndices corresponden a los 3 v�rtices del segundo tri�ngulo
    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    /* Creo un buffer para almacenar v�rtices */
    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    unsigned int index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

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

    ShaderProgramSource shaders = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(shaders.VertexSource, shaders.FragmentSource);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Dibujo los tri�ngulos. El segundo par�metro cuenta realmente los v�rtices, es decir, los pares (x,y) de cada v�rtice. */
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}