#pragma once

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;

	// Mapa para guardar los valores "location" de uniform para no llamarlos una y otra vez en GetUniformLocation
	std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	ShaderProgramSource ParseShader(const std::string& filepath);

	unsigned int CompileShader(unsigned int type, const std::string& source);

	/// <summary>
	/// El propósito de esta función es proveer a OpenGL el código fuente del shader real,
	/// y queremos que OpenGL compile ese programa y enlace los dos juntos en un único
	/// programa que sería el shader completo.
	/// </summary>
	/// <param name="vertexShader"></param>
	/// <param name="fragmentShader"></param>
	/// <returns>Algún identificador único para ese shader creado para que podamos luego asociarlo y usarlo</returns>
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	int GetUniformLocation(const std::string& name);
};