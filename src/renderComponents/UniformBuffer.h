#pragma once
#include <cstdint>
class UniformBuffer
{
private:
	uint32_t id;
	uint32_t bindIndex;
public:
	UniformBuffer(uint32_t bindIndex, uint32_t size);
	~UniformBuffer();
	void Bind() const;
	void Unbind() const;
	void SetData(const void* data, uint32_t size, uint32_t offset);
};
