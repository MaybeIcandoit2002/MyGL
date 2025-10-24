#include "Texture.h"
#include <iostream>
#include "../vendor/stb_image/stb_image.h"
Texture::Texture(const std::string& path)
	: filePath(path)
{
	localBuffer = stbi_load(path.c_str(), &width, &height, &BPP, 4);
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	if (localBuffer)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		stbi_image_free(localBuffer);
	}
	else
	{
		std::cout << "Failed to load texture: " << path << std::endl;
	}
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &id));
}

void Texture::Bind(uint32_t slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
