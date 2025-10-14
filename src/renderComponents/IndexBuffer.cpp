#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const void* indices, uint32_t count)
	: count(count)
{
    GLCall(glGenBuffers(1, &id));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    if (id != 0)
    {
        GLCall(glDeleteBuffers(1, &id));
        id = 0;
    }
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
}

void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
