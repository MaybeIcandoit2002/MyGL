#include "Renderer.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}
bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "):"
            << function << " "
            << file << ":"
            << line
            << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer()
    : VBOffset(0), IBOffset(0), shader(nullptr), VAO(nullptr), VBO(nullptr), IBO(nullptr)
{
    Only2D();
	// Vertex Buffer Layout
	layout = new VertexBufferLayout();
    layout->Push<float>(2, GL_FALSE);
    layout->Push<float>(4, GL_FALSE);
    layout->Push<float>(2, GL_FALSE);
    layout->Push<float>(1, GL_FALSE);
}

void Renderer::InitializeShader(const std::vector<std::pair<uint32_t, std::string>>& shaderPair)
{
    shader = new ShaderProgram();
	for (const std::pair<uint32_t, std::string>& pair : shaderPair)
    {
        shader->Add(pair.first, pair.second);
    }
    shader->Link();
}

void Renderer::InitializeTextures(const std::vector<std::pair<uint32_t, std::string>>& texturePaths)
{
    for (const std::pair<uint32_t, std::string>& pair : texturePaths)
    {
        Texture* texture = new Texture(pair.second);
		texture->Bind(pair.first);
	}
	std::vector<int>* l = new std::vector<int>();
    for (size_t i = 0; i < texturePaths.size(); i++)
    {
		l->push_back(i);
    }
	shader->SetUniform1iv("u_Textures", texturePaths.size(), l->data());
    delete l;
}

uint32_t Renderer::AddData(const Vertexs2D* vertexs, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount)
{
	uint32_t currentVBOffset = VBOffset;
	uint32_t currentIBOffset = IBOffset;
	VBOffset += vertexCount * sizeof(Vertexs2D);
	IBOffset += indexCount * sizeof(uint32_t);
    for (uint32_t i = 0; i < vertexCount; i++)
    {
        this->vertexs.push_back(vertexs[i]);
	}
    for (uint32_t i = 0; i < indexCount; i++)
    {
        this->indices.push_back(indices[i] + currentVBOffset);
    }

    return currentIBOffset;
}

void Renderer::SendToGPU()
{
    VBO = new VertexBuffer(vertexs.data(), VBOffset);
    IBO = new IndexBuffer(indices.data(), IBOffset);
    VAO = new VertexArray(*VBO, *layout);
}

static void SetClearColor(glm::vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::Clear(glm::vec4 color) const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(uint32_t offset, uint32_t indexCount, uint32_t instanceCount) const
{
	VAO->Bind();
	IBO->Bind();
	shader->Bind();
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const void*)(offset), instanceCount));
}

void Renderer::Only2D() const
{
	GLCall(glDisable(GL_DEPTH_TEST));
	GLCall(glDisable(GL_CULL_FACE));

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}
