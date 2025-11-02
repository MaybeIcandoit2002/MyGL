#include <fstream>
#include <sstream>
#include "Renderer.h"


Shader::Shader(uint32_t type, const std::string filePath) : type(type), filePath(filePath) {
    Compile();
}

void Shader::Compile() {
    // 创建指定类型的着色器对象
    GLCall(id = glCreateShader(type));

    // 从文件解析着色器源代码
    std::string srcStr = ParseShader(filePath);
    const char* src = srcStr.c_str();

    // 设置着色器源代码
    GLCall(glShaderSource(id, 1, &src, nullptr));

    // 编译着色器
    GLCall(glCompileShader(id));

    // 检查编译错误
    CheckShaderError();
}

void Shader::Remove() {
    if (id != 0)
    {
        glDeleteShader(id);
        id = 0;
    }
}

inline std::string Shader::ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);
    std::stringstream shaderString;
    shaderString << stream.rdbuf();  // 读取整个文件内容
    return shaderString.str();
}

inline void Shader::CheckShaderError() {
    int result;
    // 获取编译状态
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        // 获取错误日志长度
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        // 分配栈内存存储错误消息
        char* message = (char*)alloca(length * sizeof(char));

        // 获取错误日志
        glGetShaderInfoLog(id, length, &length, message);

        // 打印错误信息
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader!" << std::endl;
        std::cout << message << std::endl;

        // 清理失败的着色器对象
        glDeleteShader(id);
        id = 0;
    }
}