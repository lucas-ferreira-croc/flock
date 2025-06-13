#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <glad/gl.h>
#include <glm/glm.hpp>

class Shader
{
public:
	Shader();
	~Shader();

	void createFromString(const char* vertex_code, const char* fragment_code);
	void createFromFile(const std::string& vertex_code_path, const std::string& fragment_code_path);

	void bind();
	void unbind();

	void clear();

	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setFloat3(const std::string& name, const glm::vec3& value);
	void setFloat4(const std::string& name, float r, float g, float b, float a);
	void setFloat4(const std::string& name, const glm::vec4& value);
	void setMat4(const std::string& name, const glm::mat4& value);
	void setMat4(const std::string& name, float* value);
	void setVec3Array(const std::string& name, std::vector<glm::vec3> values);
	
	//void set_directional_light(const DirectionalLight& light);
	//void set_point_lights(std::vector<PointLight>& point_lights);
	//void set_spot_lights(std::vector<SpotLight>& spot_lights);
	//void set_material(const Material& material);
	//

	int getUniformLocation(std::string name);

	bool reload();
private:
	unsigned int id;

	void compile(const char* vertex_code, const char* fragment_code);
	void add(const char* shader_code, GLenum shader_type);

	std::string readFile(const std::string& filepath);

	std::unordered_map<std::string, int> _uniformCache;

	std::string _vertexPath;
	std::string _fragmentPath;
};

#endif