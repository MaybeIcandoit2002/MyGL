#pragma once
#include <cstdint>
class VertexBuffer;
class VertexBufferLayout;

class VertexArray
{
private:
	uint32_t id;
public:
	VertexArray(const VertexBuffer& vbo, VertexBufferLayout layout);
	~VertexArray();

	void Bind() const;
	void Unbind() const;
};