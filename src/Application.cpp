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
        - shader: id que especifica el gestor del objeto del shader cuyo código fuente será reeplazado
        - count: el número de códigos fuente que estamos especificando
        - string: especifica el array de punteros a strigns conteniendo el código fuente a ser cargado al shader
        - lenght: especifica una longitud para el array de strings. Si se pone un NULL aquí se asume a cada string como que termina con un caracter nulo
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
/// El propósito de esta función es proveer a OpenGL el código fuente del shader real,
/// y queremos que OpenGL compile ese programa y enlace los dos juntos en un único
/// programa que sería el shader completo.
/// </summary>
/// <param name="vertexShader"></param>
/// <param name="fragmentShader"></param>
/// <returns>Algún identificador único para ese shader creado para que podamos luego asociarlo y usarlo</returns>
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

    // Los primeros tres índices corresponden a los 3 vértices del primer triángulo
    // Los segundos tres índices corresponden a los 3 vértices del segundo triángulo
    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3
    };

    /* Creo un buffer para almacenar vértices */
    unsigned int vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    unsigned int index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    /*
        Describiré los atributos en orden:
            - index: especifica el índice del atributo del vértice genérico a ser modificado
                     Resulta que los vértices, recordemos, no son posiciones, sino todo un objeto
                     con diferentes propiedades y características. Por ejemplo: posición, textura, normal.
                     En este caso, consideramos a la posición como nuestra primera propiedad, así que el
                     índice será 0 en este caso.
            - size: especifica el número de componentes que tiene el atributo del vértice genérico. En este
                    caso, como estamos considerando una posición de un vértice bidimensional, tenemos que
                    son 2 las componentes que conforman la posición del vértice.
            - type: especifica el tipo de dato de cada componente en el arreglo. En nuestro caso, el tipo
                    de dato que tiene cada componente es un float.
            - normalized: se especifica si los datos deberían ser normalizados o no. En este caso no necesitamos
                          hacer esto porque float ya es un tipo de dato normalizado. Pero explico mejor esto:
                          Digamos que estamos especificando un byte cuyos valores van del 0 al 255, que suele ser
                          la manera de especificar el color. Eso necesita ser normalizado entre 0 y 1 como un float
                          en nuestro shader.
            - stride: especifica el offset de byte entre atributos consecutivos de un vértice genérico. En otras
                      palabras, es la cantidad de bytes que hay entre cada vértice. Volvamos al ejemplo en el que
                      tengo los atributos:
                       * "posición", que es un atributo de 3 componentes (supongamos que estamos en la 3ra dimensión);
                       * "textura", que es un atributo que tiene 2 componentes;
                       * y "normal", siendo un atributo de 3 componentes.
                      El primer atributo tiene 12 bytes (al ser 3 floats); el segundo, 8 bytes; y el tercer atributo,
                      12 bytes. Eso suma un total de 32 bytes. Y ese es nuestro stride. Es básicamente el tamaño de
                      cada vértice.
                      La razón por la que es importante esto es porque, si queremos saltar de un vértice al otro, es
                      importante saber cuánto debe ser el offset en bytes para esto.
            - pointer: es el puntero hacia el atributo real. Supongamos de nuevo que tengo los atributos mencionados
                       y nos centremos en solo UN vértice. ¿Cuál es el offset entonces de cada uno de esos atributos?
                       De "posición", será 0 claramente; de "textura", 12; y de "normal", 20. Eso representa este
                       atributo, y como te darás cuenta es un puntero lo que hay que escribir, así que haz un casteo
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

        /* Dibujo los triángulos. El segundo parámetro cuenta realmente los vértices, es decir, los pares (x,y) de cada vértice. */
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