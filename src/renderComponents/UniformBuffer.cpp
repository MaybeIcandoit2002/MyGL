#include "UniformBuffer.h"
#include "Renderer.h"

UniformBuffer::UniformBuffer(uint32_t bindIndex, uint32_t size)
	: bindIndex(bindIndex)
{
	GLCall(glGenBuffers(1, &id));
	GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, bindIndex, id));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
}

UniformBuffer::~UniformBuffer()
{
	if (id != 0)
	{
		GLCall(glDeleteBuffers(1, &id));
		id = 0;
	}
}

void UniformBuffer::Bind() const
{
	GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, bindIndex, id));
}

void UniformBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, bindIndex, id));
	GLCall(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
}
