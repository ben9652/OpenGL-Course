#include "VertexArray.h"

#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	// Set up the layout for the buffer
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(0));

        /*
            Describiré los atributos en orden de glVertexAttribPointer:
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
                          El primer atributo tiene 12 bytes (al ser 3 float); el segundo, 8 bytes; y el tercer atributo,
                          12 bytes. Eso suma un total de 32 bytes. Y ese es nuestro stride. Es básicamente el tamaño de
                          cada vértice.
                          La razón por la que es importante esto es porque, si queremos saltar de un vértice al otro, es
                          importante saber cuánto debe ser el offset en bytes para esto.
                - pointer: es el puntero hacia el atributo real. Supongamos de nuevo que tengo los atributos mencionados
                           y nos centremos en solo UN vértice. ¿Cuál es el offset entonces de cada uno de esos atributos?
                           De "posición", será 0 claramente; de "textura", 24; y de "normal", 32. Eso representa este
                           atributo, y como te darás cuenta es un puntero lo que hay que escribir, así que haz un casteo
                           si hace falta.
        */
		GLCall(
			glVertexAttribPointer(
				i,
				element.count,
				element.type,
				element.normalized,
				layout.GetStride(),
				(const void*)offset
			)
		);

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind()
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind()
{
	GLCall(glBindVertexArray(0));
}
