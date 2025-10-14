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
{
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

}

void Renderer::Clear(glm::vec4 color) const
{
	glClearColor(color.r, color.g, color.b, color.a);
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& vao, const IndexBuffer& ibo, const UniformBuffer& ubo, const ShaderProgram& shader, uint32_t DrawMode) const
{
	vao.Bind();
	ibo.Bind();
	ubo.Bind();
	shader.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ibo.GetCount(), GL_UNSIGNED_INT, nullptr));
}
