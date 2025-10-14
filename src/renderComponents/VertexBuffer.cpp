#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, uint32_t size)
{
    GLCall(glGenBuffers(1, &id));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    if (id != 0) {
        GLCall(glDeleteBuffers(1, &id));
		id = 0;
    }
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
