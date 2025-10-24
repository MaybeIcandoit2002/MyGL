#pragma once
#include <GL/glew.h>
#include <vector>
#include "glm.hpp"
struct VertexBufferElement
{
	uint32_t dimension;
	uint32_t dataType;
	uint32_t normalized;
	uint32_t offset;

	static uint32_t GetSizeOfType(uint32_t type) {
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_INT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> elements;
	uint32_t stride;
public:
	VertexBufferLayout() : stride(0) {};

	template<typename T>
	void Push(uint32_t dimension, uint32_t normalized = GL_FALSE) {
		static_assert(false);
	}
	template<>
	void Push<float>(uint32_t dimension, uint32_t normalized) {
		elements.push_back({ dimension, GL_FLOAT, normalized, stride });
		stride += dimension * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	template<>
	void Push<int32_t>(uint32_t dimension, uint32_t normalized) {
		elements.push_back({ dimension, GL_INT, normalized, stride });
		stride += dimension * VertexBufferElement::GetSizeOfType(GL_INT);
	}
	template<>
	void Push<uint32_t>(uint32_t dimension, uint32_t normalized) {
		elements.push_back({ dimension, GL_UNSIGNED_INT, normalized, stride });
		stride += dimension * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}
	template<>
	void Push<uint8_t>(uint32_t dimension, uint32_t normalized) {
		elements.push_back({ dimension, GL_UNSIGNED_BYTE, normalized, stride });
		stride += dimension * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return elements; }
	inline uint32_t GetStride() const { return stride; }
};