#include "Renderer.h"
#include <iostream>
Texture::Texture(const unsigned char* localBuffer, int width, int height)
{
	GLCall(glGenTextures(1, &id));
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	// 将图片数据上传到 GPU
	GLCall(glTexImage2D(
		GL_TEXTURE_2D,        // 目标: 2D 纹理
		0,                    // Mipmap 级别: 0(基础级别)
		GL_RGBA8,             // GPU 内部格式: RGBA 每通道 8 位
		width, height,        // 纹理尺寸
		0,                    // 边框(必须为 0,已废弃参数)
		GL_RGBA,              // 数据格式: RGBA
		GL_UNSIGNED_BYTE,     // 数据类型: 无符号字节
		localBuffer           // 图片数据指针
	));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
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