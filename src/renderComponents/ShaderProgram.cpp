#include "ShaderProgram.h"
#include "Shader.h"
ShaderProgram::ShaderProgram()
{
	GLCall(id = glCreateProgram());
}

ShaderProgram::~ShaderProgram()
{
	if (id != 0)
	{
		glDeleteProgram(id);
		id = 0;
	}
}

void ShaderProgram::Add(uint32_t type, const std::string& path)
{
	Shader* shader = new Shader(type, path);
	shaders.push_back(shader);
}

void ShaderProgram::Link() const
{
	for (const auto& shader : shaders) {
		GLCall(glAttachShader(id, shader->id));
	}
	GLCall(glLinkProgram(id));
	GLCall(glValidateProgram(id));
	GLCall(glUseProgram(id));
	for (const auto& shader : shaders)
		shader->Remove();
}

void ShaderProgram::Bind() const
{
	GLCall(glUseProgram(id));
}

void ShaderProgram::Unbind() const
{
	GLCall(glUseProgram(0));
}

void ShaderProgram::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void ShaderProgram::SetUniform1iv(const std::string& name, int count, int* value)
{
	GLCall(glUniform1iv(GetUniformLocation(name), count, value));
}

void ShaderProgram::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void ShaderProgram::SetUniformMat4f(const std::string& name, const glm::mat4 matrix)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

void ShaderProgram::SetUniformBlock(const std::string& name, int32_t bindIndex) const
{
	uint32_t block = glGetUniformBlockIndex(id, name.c_str());
	GLCall(glUniformBlockBinding(id, block, bindIndex));
}

int ShaderProgram::GetUniformLocation(const std::string& name) const
{
	int location = -1;
	if (uniformLocationCache.find(name) == uniformLocationCache.end())
	{
		GLCall(uniformLocationCache[name] = glGetUniformLocation(id, name.c_str()));
		ASSERT(location == -1);
	}
	return uniformLocationCache[name];
}
