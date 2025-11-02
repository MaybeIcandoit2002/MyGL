#pragma once
#include <cstdint>
class VertexBuffer;
class VertexLayout;

class VertexArray
{
private:
	uint32_t id;
public:
	VertexArray(const VertexBuffer& vbo, VertexLayout layout);
	~VertexArray();

	void Bind() const;
	void Unbind() const;
};