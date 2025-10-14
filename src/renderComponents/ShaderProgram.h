#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <glm.hpp>

class Shader;

class ShaderProgram
{
private:
	uint32_t id;
	std::vector<Shader*> shaders;
	mutable std::unordered_map<std::string, int> uniformLocationCache;
public:
	ShaderProgram();
	~ShaderProgram();
	void Add(uint32_t type, const std::string& path);
	void Link() const;
	void Bind() const;
	void Unbind() const;
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1iv(const std::string& name, int count, int* value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
	void SetUniformBlock(const std::string& name, int32_t bindIndex) const;
private:
	int GetUniformLocation(const std::string& name) const;
};

