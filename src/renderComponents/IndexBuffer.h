#pragma once
#include <GL/glew.h>
#include <cstdint>
class IndexBuffer
{
private:
	uint32_t id;
	uint32_t count;
public:
	IndexBuffer(const void* indices, uint32_t count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
	uint32_t GetCount() const { return count; }
};