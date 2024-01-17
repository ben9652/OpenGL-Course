#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <math.h>
#include <locale.h>
#include <Windows.h>
#include "GL/glew.h"

#include "Display.h"

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "VertexArray.h"

#define NUM_PI 3.14159265358979323846f
#define TOLERANCE_ERROR 1e-4f


struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

struct Shape
{
    float* positions;
    unsigned int* indices;

    unsigned int triangles_qnty;
    unsigned int vertices_qnty;

    Shape(float* _positions, unsigned int* _indices, unsigned int tr_q, unsigned int v_q) :
        positions(_positions),
        indices(_indices),
        triangles_qnty(tr_q),
        vertices_qnty(v_q)
    {}
};

struct Vertex
{
    float x;
    float y;

public:
    Vertex(float _x, float _y) : x(_x), y(_y) {}

    Vertex(const Vertex& vertex)
    {
        x = vertex.x;
        y = vertex.y;
    }

    friend bool operator==(const Vertex& v1, const Vertex& v2);
    friend bool operator!=(const Vertex& v1, const Vertex& v2);
};

inline Vertex RotateVertex(float x_component, float y_component, float angle)
{
    float new_x = x_component * cosf(angle) - y_component * sinf(angle);
    float new_y = x_component * sinf(angle) + y_component * cosf(angle);
    return Vertex(new_x, new_y);
}

static Shape BuildCircle(float radio, float rotation_angle)
{
    std::vector<float> positions;
    std::vector<unsigned int> indices;

    // Creaci�n del v�rtice central y 1 v�rtice inicial desde el cual ir formando los tri�ngulos.
    positions.insert(positions.end(), {  0.0f , 0.0f });
    positions.insert(positions.end(), {  0.0f , radio });

    unsigned int i_center_vertex = 0;
    unsigned int i_last_vertex = 1;

    Vertex vertex1(0.0f, radio);

    unsigned int triangles_quantity = 0;
    unsigned int vertices_quantity = 2;

    while (true)
    {
        // Obtengo el �ltimo v�rtice para rotarlo
        Vertex last_vertex(positions[2 * i_last_vertex], positions[2 * i_last_vertex + 1]);

        // Creo el nuevo v�rtice rotado respecto al �ltimo
        Vertex new_vertex = RotateVertex(last_vertex.x, last_vertex.y, rotation_angle);

        triangles_quantity++;

        if (new_vertex == vertex1)
        {
            // Cargo el �ltimo �ndice necesario
            indices.insert(indices.end(), { i_center_vertex, i_last_vertex, 1 });
            break;
        }
        else
        {
            // Cargo el �nico v�rtice nuevo que necesito para el nuevo tri�ngulo
            positions.insert(positions.end(), { new_vertex.x, new_vertex.y });
            vertices_quantity++;

            // Cargo los �ndices de los v�rtices que conforman el nuevo tri�ngulo
            indices.insert(indices.end(), { i_center_vertex, i_last_vertex, i_last_vertex++ + 1 });
        }
    }

    size_t p_size = positions.size();
    size_t i_size = indices.size();

    float* p_array = new float[p_size];
    unsigned int* i_array = new unsigned int[i_size];

    memcpy(p_array, &positions[0], p_size * sizeof(float));
    memcpy(i_array, &indices[0], i_size * sizeof(unsigned int));

    return Shape(p_array, i_array, triangles_quantity, vertices_quantity);
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
        - shader: id que especifica el gestor del objeto del shader cuyo c�digo fuente ser� reeplazado
        - count: el n�mero de c�digos fuente que estamos especificando
        - string: especifica el array de punteros a strigns conteniendo el c�digo fuente a ser cargado al shader
        - lenght: especifica una longitud para el array de strings. Si se pone un NULL aqu� se asume a cada string como que termina con un caracter nulo
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
/// El prop�sito de esta funci�n es proveer a OpenGL el c�digo fuente del shader real,
/// y queremos que OpenGL compile ese programa y enlace los dos juntos en un �nico
/// programa que ser�a el shader completo.
/// </summary>
/// <param name="vertexShader"></param>
/// <param name="fragmentShader"></param>
/// <returns>Alg�n identificador �nico para ese shader creado para que podamos luego asociarlo y usarlo</returns>
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
    Display window(500, 500, "Hello World!");

    window.setSwapInterval(1);

    GLenum err = glewInit();
    if (err != GLEW_OK)
        std::cerr << "Error! " << glewGetErrorString(err) << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    Shape circle = BuildCircle(0.5, NUM_PI/2);

    unsigned int triangles_qnty = circle.triangles_qnty;
    unsigned int vertices_qnty = circle.vertices_qnty;

    VertexArray va;
    VertexBuffer vb(circle.positions, 2 * vertices_qnty * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(circle.indices, 3 * triangles_qnty);

    ShaderProgramSource shaders = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(shaders.VertexSource, shaders.FragmentSource);
    GLCall(glUseProgram(shader));

    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    std::cout << (unsigned char*)"There're " << triangles_qnty << " triangles to draw" << std::endl;

    float r = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!window.windowShouldClose())
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        va.Bind();
        ib.Bind();

        /* Dibujo los tri�ngulos. El segundo par�metro cuenta realmente los v�rtices, es decir, los pares (x,y) de cada v�rtice. */
        GLCall(glDrawElements(GL_TRIANGLES, 3 * triangles_qnty, GL_UNSIGNED_INT, nullptr));

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

    GLCall(glDeleteProgram(shader));

    return 0;
}

bool operator==(const Vertex& v1, const Vertex& v2)
{
    bool xs_iguales = abs(v1.x - v2.x) < TOLERANCE_ERROR ? true : false;
    bool ys_iguales = abs(v1.y - v2.y) < TOLERANCE_ERROR ? true : false;

    return xs_iguales && ys_iguales;
}

bool operator!=(const Vertex& v1, const Vertex& v2)
{
    bool xs_distintos = abs(v1.x - v2.x) < TOLERANCE_ERROR ? false : true;
    bool ys_distintos = abs(v1.y - v2.y) < TOLERANCE_ERROR ? false : true;

    return xs_distintos || ys_distintos;
}
