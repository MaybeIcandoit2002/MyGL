#pragma once
#include <cstdint>
#include <GL/glew.h>
#include <vector>

#include "Layout.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "ShaderProgram.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
	Renderer();
	void Clear(glm::vec4 color) const;
	void Draw(const VertexArray& vao, const IndexBuffer& ibo, const UniformBuffer& ubo, const ShaderProgram& shader, uint32_t DrawMode = GL_TRIANGLES) const;
};