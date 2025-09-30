#include "render/shader.hpp"
#include "log/logger.hpp"

#include <fstream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader() : id(0)
{
}

Shader::~Shader()
{
	clear();
}

void Shader::createFromString(const char* vertex_code, const char* fragment_code)
{
	compile(vertex_code, fragment_code);
	unbind();
}

void Shader::createFromFile(const std::string& vertex_code_path, const std::string& fragment_code_path)
{
	_vertexPath = vertex_code_path;
	_fragmentPath = fragment_code_path;

	compile(readFile(vertex_code_path).c_str(), readFile(fragment_code_path).c_str());
	unbind();
}

void Shader::bind()
{
	glUseProgram(id);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::clear()	
{
	if (id != 0)
	{
	 	glDeleteProgram(id);
	 	id = 0;
	}
}

void Shader::setInt(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setFloat2(const std::string& name, const glm::vec2& value)
{
	glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setFloat4(const std::string& name, float r, float g, float b, float a)
{
	glUniform4f(getUniformLocation(name), r, g, b, a);
}

void Shader::setMat4(const std::string& name, float* value)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value);
}

void Shader::setFloat3(const std::string& name, const glm::vec3& value)
{
	glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setFloat4(const std::string& name, const glm::vec4& value)
{
	glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3Array(const std::string& name, std::vector<glm::vec3> values)
{
	std::stringstream ss;

	for (int i = 0; i < values.size(); i++)
	{
		ss << name << "[" << i << "]";
		std::string uniform = ss.str();
		setFloat3(uniform, values[i]);
		ss.str("");
	}
}
//void Shader::set_material(const Material& material)
//{
//	set_float3("material.ambient_color", material.m_AmbientColor);
//	set_float3("material.diffuse_color", material.m_DiffuseColor);
//	set_float3("material.specular_color", material.m_SpecularColor);
//}

int Shader::getUniformLocation(std::string name)
{
	int location;
	if (_uniformCache.find(name) != _uniformCache.end())
	{
		location = _uniformCache.at(name);
		return location;
	}
	location = glGetUniformLocation(id, name.c_str());
	_uniformCache.insert(std::make_pair(name, location));
	return location;
}

bool Shader::reload()
{
	if(_vertexPath.empty() || _fragmentPath.empty())
		return false;

	clear();
	_uniformCache.clear();
	createFromFile(_vertexPath, _fragmentPath);
	bind();
	return true;
}

void Shader::compile(const char* vertex_code, const char* fragment_code)
{
	id = glCreateProgram();

	if (!id) {
		Logger::err("ERROR CREATING SHADER PROGRAM!");
		return;
	}

	add(vertex_code, GL_VERTEX_SHADER);
	add(fragment_code, GL_FRAGMENT_SHADER);

	int result = 0;
	char error_log[1024] = { 0 };

	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(id, sizeof(error_log), NULL, error_log);
		Logger::err("ERRROR LINKING PROGRAM: ");
		Logger::err(error_log);

		return;
	}

	glValidateProgram(id);
	glGetProgramiv(id, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(id, sizeof(error_log), NULL, error_log);
		Logger::err("ERRROR VALIDATING PROGRAM: ");
		Logger::err(error_log);
		return;
	}
}

void Shader::add(const char* shader_code, GLenum shader_type)
{
	unsigned int shader = glCreateShader(shader_type);
	const char* code[1];
	code[0] = shader_code;

	int code_length[1];
	code_length[0] = strlen(shader_code);

	glShaderSource(shader, 1, code, code_length);
	glCompileShader(shader);

	int result = 0;
	char error_log[1024] = { 0 };

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		std::string shader_type_name = shader_type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";

		glGetShaderInfoLog(shader, sizeof(error_log), NULL, error_log);
		Logger::err("ERRROR COMPILING THE " + shader_type);
		Logger::err(" SHADER: ");
		Logger::err(error_log);
		return;
	}

	glAttachShader(id, shader);

}

std::string Shader::readFile(const std::string& filepath)
{
	std::fstream stream(filepath);
	std::stringstream fragmentShaderBuffer;

	fragmentShaderBuffer << stream.rdbuf();
	stream.close();

	std::string source_code = fragmentShaderBuffer.str();
	return source_code;
}
