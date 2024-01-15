#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <locale.h>
#include <Windows.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#define NUM_PI 3.14159265358979323846f
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

struct Shape
{
    double* positions;
    unsigned int* indices;

    unsigned int triangles_quantity;

    Shape(double* _positions, unsigned int* _indices, unsigned int tr_q) :
        positions(_positions),
        indices(_indices),
        triangles_quantity(tr_q)
    {}
};

struct Vertex
{
    double x;
    double y;

public:
    Vertex(double _x, double _y) : x(_x), y(_y) {}

    Vertex(const Vertex& vertex)
    {
        x = vertex.x;
        y = vertex.y;
    }

    friend bool operator==(const Vertex& v1, const Vertex& v2);
    friend bool operator!=(const Vertex& v1, const Vertex& v2);
};

inline Vertex RotateVertex(double x_component, double y_component, double angle)
{
    double new_x = x_component * cos(angle) - y_component * sin(angle);
    double new_y = x_component * sin(angle) + y_component * cos(angle);
    return Vertex(new_x, new_y);
}

static Shape BuildCircle(double radio, double rotation_angle)
{
    std::vector<double> positions;
    std::vector<unsigned int> indices;

    // Creación del vértice central y 1 vértice inicial desde el cual ir formando los triángulos.
    positions.insert(positions.end(), {  0.0 , 0.0 });
    positions.insert(positions.end(), {  0.0 , radio });

    unsigned int i_center_vertex = 0;
    unsigned int i_last_vertex = 1;

    Vertex vertex1(0, radio);

    unsigned int triangles_quantity = 0;

    while (true)
    {
        // Obtengo el último vértice para rotarlo
        Vertex last_vertex(positions[2 * i_last_vertex], positions[2 * i_last_vertex + 1]);

        // Creo el nuevo vértice rotado respecto al último
        Vertex new_vertex = RotateVertex(last_vertex.x, last_vertex.y, rotation_angle);

        triangles_quantity++;

        if (new_vertex == vertex1)
        {
            // Cargo el último índice necesario
            indices.insert(indices.end(), { i_center_vertex, i_last_vertex, 1 });
            break;
        }
        else
        {
            // Cargo el único vértice nuevo que necesito para el nuevo triángulo
            positions.insert(positions.end(), { new_vertex.x, new_vertex.y });

            // Cargo los índices de los vértices que conforman el nuevo triángulo
            indices.insert(indices.end(), { i_center_vertex, i_last_vertex, i_last_vertex++ + 1 });
        }
    }

    size_t p_size = positions.size();
    size_t i_size = indices.size();

    double* p_array = new double[p_size];
    unsigned int* i_array = new unsigned int[i_size];

    memcpy(p_array, &positions[0], p_size * sizeof(double));
    memcpy(i_array, &indices[0], i_size * sizeof(unsigned int));

    return Shape(p_array, i_array, triangles_quantity);
}

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
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();

    /*
        - shader: id que especifica el gestor del objeto del shader cuyo código fuente será reeplazado
        - count: el número de códigos fuente que estamos especificando
        - string: especifica el array de punteros a strigns conteniendo el código fuente a ser cargado al shader
        - lenght: especifica una longitud para el array de strings. Si se pone un NULL aquí se asume a cada string como que termina con un caracter nulo
    */
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" <<
            std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
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
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(500, 500, "Hello World", NULL, NULL);
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

    Shape circle = BuildCircle(0.5, NUM_PI/360);

    unsigned int triangles_qnty = circle.triangles_quantity;
    unsigned int vertices_qnty = triangles_qnty * 3;

    /* Creo un buffer para almacenar vértices */
    unsigned int vertexBuffer;
    GLCall(glGenBuffers(1, &vertexBuffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertices_qnty * 2 * sizeof(double), circle.positions, GL_STATIC_DRAW));

    unsigned int index_buffer;
    GLCall(glGenBuffers(1, &index_buffer));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_qnty * sizeof(unsigned int), circle.indices, GL_STATIC_DRAW));

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
                    de dato que tiene cada componente es un double.
            - normalized: se especifica si los datos deberían ser normalizados o no. En este caso no necesitamos
                          hacer esto porque double ya es un tipo de dato normalizado. Pero explico mejor esto:
                          Digamos que estamos especificando un byte cuyos valores van del 0 al 255, que suele ser
                          la manera de especificar el color. Eso necesita ser normalizado entre 0 y 1 como un double
                          en nuestro shader.
            - stride: especifica el offset de byte entre atributos consecutivos de un vértice genérico. En otras
                      palabras, es la cantidad de bytes que hay entre cada vértice. Volvamos al ejemplo en el que
                      tengo los atributos:
                       * "posición", que es un atributo de 3 componentes (supongamos que estamos en la 3ra dimensión);
                       * "textura", que es un atributo que tiene 2 componentes;
                       * y "normal", siendo un atributo de 3 componentes.
                      El primer atributo tiene 24 bytes (al ser 3 double); el segundo, 8 bytes; y el tercer atributo,
                      12 bytes. Eso suma un total de 44 bytes. Y ese es nuestro stride. Es básicamente el tamaño de
                      cada vértice.
                      La razón por la que es importante esto es porque, si queremos saltar de un vértice al otro, es
                      importante saber cuánto debe ser el offset en bytes para esto.
            - pointer: es el puntero hacia el atributo real. Supongamos de nuevo que tengo los atributos mencionados
                       y nos centremos en solo UN vértice. ¿Cuál es el offset entonces de cada uno de esos atributos?
                       De "posición", será 0 claramente; de "textura", 24; y de "normal", 32. Eso representa este
                       atributo, y como te darás cuenta es un puntero lo que hay que escribir, así que haz un casteo
                       si hace falta.
    */
    GLCall(glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, sizeof(double) * 2, 0));
    GLCall(glEnableVertexAttribArray(0));

    ShaderProgramSource shaders = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(shaders.VertexSource, shaders.FragmentSource);
    GLCall(glUseProgram(shader));

    std::cout << (unsigned char*)"There're " << triangles_qnty << " triangles to draw" << std::endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Dibujo los triángulos. El segundo parámetro cuenta realmente los vértices, es decir, los pares (x,y) de cada vértice. */
        GLCall(glDrawElements(GL_TRIANGLES, vertices_qnty, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    GLCall(glDeleteProgram(shader));

    glfwTerminate();
    return 0;
}

bool operator==(const Vertex& v1, const Vertex& v2)
{
    double tolerancia = 1e-6;

    bool xs_iguales = abs(v1.x - v2.x) < tolerancia ? true : false;
    bool ys_iguales = abs(v1.y - v2.y) < tolerancia ? true : false;

    return xs_iguales && ys_iguales;
}

bool operator!=(const Vertex& v1, const Vertex& v2)
{
    double tolerancia = 1e-6;

    bool xs_distintos = abs(v1.x - v2.x) < tolerancia ? false : true;
    bool ys_distintos = abs(v1.y - v2.y) < tolerancia ? false : true;

    return xs_distintos || ys_distintos;
}
