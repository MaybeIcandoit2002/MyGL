#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include "Renderer.h"

class Shader
{
public:
	uint32_t id;
	uint32_t type;
	std::string filePath;
	Shader(uint32_t type, std::string filePath) : type(type), filePath(filePath) {
        Compile();
    }
	void Compile() {
        GLCall(id = glCreateShader(type));
		std::string srcStr = ParseShader(filePath);
        const char* src = srcStr.c_str();
		GLCall(glShaderSource(id, 1, &src, nullptr));
        GLCall(glCompileShader(id));
        CheckShaderError();
	}
    void Remove() {
        if (id != 0)
        {
            glDeleteShader(id);
            id = 0;
        }
    }
private:
    inline std::string ParseShader(const std::string& filepath) {
        std::ifstream stream(filepath);
        std::stringstream shaderString;
        shaderString << stream.rdbuf();
        return shaderString.str();
    }
    inline void CheckShaderError() {
        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*)alloca(length * sizeof(char));
            glGetShaderInfoLog(id, length, &length, message);
            std::cout << "Failed to compile " <<
                (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
                << " shader!" << std::endl;
            std::cout << message << std::endl;
            glDeleteShader(id);
            id = 0;
        }
	}
};

