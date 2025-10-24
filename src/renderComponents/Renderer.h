#pragma once
#include <cstdint>
#include <GL/glew.h>
#include <vector>

#include "Layout.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "../Macros.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

struct Vertexs2D
{
	glm::vec2 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	float texID;
};

struct TransForms2D
{
	glm::mat3x4 matSRT;
};


class Renderer
{
private:

	ShaderProgram* shader;
	VertexArray* VAO;

	VertexBuffer* VBO;
	std::vector<Vertexs2D> vertexs;
	uint32_t VBOffset;

	IndexBuffer* IBO;
	std::vector<uint32_t> indices;
	VertexBufferLayout* layout;
	uint32_t IBOffset;
public:
	Renderer();
	void InitializeShader(const std::vector<std::pair<uint32_t, std::string>>& shaderPair);
	void InitializeTextures(const std::vector<std::pair<uint32_t, std::string>>& texturePaths);
	uint32_t AddData(const Vertexs2D* vertexs, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount);
	void SendToGPU();

	void Clear(glm::vec4 color) const;
	void Draw(uint32_t offset, uint32_t indexCount, uint32_t instanceCount) const;
	ShaderProgram* GetShader() const { return shader; }
private:
	inline void Only2D() const;
};