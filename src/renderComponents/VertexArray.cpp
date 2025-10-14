#include "VertexArray.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "Layout.h"

VertexArray::VertexArray(const VertexBuffer& vbo, VertexBufferLayout layout)
{
    GLCall(glGenVertexArrays(1, &id));
    GLCall(glBindVertexArray(id));
    vbo.Bind();
    const auto& elements = layout.GetElements();
    uint32_t offset = 0;
    for (uint32_t i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, element.dimension, element.dataType, element.normalized, layout.GetStride(), (const void*)element.offset));
    }
}

VertexArray::~VertexArray()
{
    if (id != 0)
    {
        GLCall(glDeleteVertexArrays(1, &id));
        id = 0;
    }
}

void VertexArray::Bind() const
{
    GLCall(glBindVertexArray(id));
}

void VertexArray::Unbind() const
{
    GLCall(glBindVertexArray(0));
}
