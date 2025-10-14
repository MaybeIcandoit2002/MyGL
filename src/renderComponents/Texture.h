#pragma once
#include "renderer.h"
class Texture
{
private:
	uint32_t id;
	std::string filePath;
	unsigned char* localBuffer;
	int width, height, BPP;
public:
	Texture(const std::string& path);
	~Texture();
	void Bind(uint32_t slot = 0) const;
	void Unbind() const;
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
};

