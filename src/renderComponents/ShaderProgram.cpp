#include "Renderer.h"

/**
 * @brief 构造函数 - 创建 OpenGL 着色器程序对象
 */
ShaderProgram::ShaderProgram()
{
	GLCall(id = glCreateProgram());
}

/**
 * @brief 析构函数 - 删除着色器程序并释放资源
 */
ShaderProgram::~ShaderProgram()
{
	if (id != 0)
	{
		glDeleteProgram(id);
		id = 0;
	}
}

/**
 * @brief 添加着色器到程序
 * @param type 着色器类型 (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER 等)
 * @param path 着色器源文件路径
 * 
 * 创建指定类型的着色器并加入到着色器列表中
 */
void ShaderProgram::Add(uint32_t type, const std::string& path)
{
	Shader* shader = new Shader(type, path);
	shaders.push_back(shader);
}

/**
 * @brief 链接着色器程序
 * 
 * 流程:
 * 1. 将所有着色器附加到程序
 * 2. 链接程序
 * 3. 验证程序
 * 4. 激活程序
 * 5. 删除独立的着色器对象(已链接到程序中,不再需要)
 */
void ShaderProgram::Link() const
{
	// 附加所有着色器到程序
	for (const auto& shader : shaders) {
		GLCall(glAttachShader(id, shader->id));
	}
	
	// 链接程序(将顶点着色器和片段着色器组合)
	GLCall(glLinkProgram(id));
	
	// 验证程序是否可以在当前 OpenGL 状态下执行
	GLCall(glValidateProgram(id));
	
	// 激活此着色器程序
	GLCall(glUseProgram(id));
	
	// 删除独立的着色器对象(已链接,不再需要)
	for (const auto& shader : shaders)
		shader->Remove();
}

/**
 * @brief 绑定(激活)此着色器程序
 * 
 * 后续的渲染调用将使用此程序
 */
void ShaderProgram::Bind() const
{
	GLCall(glUseProgram(id));
}

/**
 * @brief 解绑着色器程序
 * 
 * 将当前着色器程序设置为 0(无程序)
 */
void ShaderProgram::Unbind() const
{
	GLCall(glUseProgram(0));
}

/**
 * @brief 设置整型 uniform 变量
 * @param name uniform 变量名
 * @param value 要设置的整型值
 */
void ShaderProgram::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

/**
 * @brief 设置整型数组 uniform 变量
 * @param name uniform 变量名
 * @param count 数组元素个数
 * @param value 整型数组指针
 * 
 * 常用于设置纹理采样器数组
 */
void ShaderProgram::SetUniform1iv(const std::string& name, int count, int* value)
{
	GLCall(glUniform1iv(GetUniformLocation(name), count, value));
}

/**
 * @brief 设置 4 分量浮点 uniform 变量
 * @param name uniform 变量名
 * @param v0, v1, v2, v3 四个浮点分量
 * 
 * 常用于设置颜色(RGBA)或 vec4
 */
void ShaderProgram::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

/**
 * @brief 设置 4x4 矩阵 uniform 变量
 * @param name uniform 变量名
 * @param matrix 4x4 矩阵(glm::mat4)
 * 
 * 常用于设置变换矩阵(MVP 矩阵等)
 */
void ShaderProgram::SetUniformMat4f(const std::string& name, const glm::mat4 matrix)
{
	// 参数: location, count, transpose, value
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

/**
 * @brief 设置 uniform 块绑定点
 * @param name uniform 块名称
 * @param bindIndex 绑定点索引
 * 
 * 用于将 uniform 块绑定到 UBO(Uniform Buffer Object)
 */
void ShaderProgram::SetUniformBlock(const std::string& name, int32_t bindIndex) const
{
	// 获取 uniform 块索引
	uint32_t block = glGetUniformBlockIndex(id, name.c_str());
	// 将 uniform 块绑定到指定的绑定点
	GLCall(glUniformBlockBinding(id, block, bindIndex));
}

/**
 * @brief 获取 uniform 变量的位置
 * @param name uniform 变量名
 * @return uniform 变量在着色器程序中的位置
 * 
 * 使用缓存机制避免重复查询,提高性能
 */
int ShaderProgram::GetUniformLocation(const std::string& name) const
{
	int location = -1;
	
	// 如果缓存中没有此 uniform,则查询并缓存
	if (uniformLocationCache.find(name) == uniformLocationCache.end())
	{
		GLCall(uniformLocationCache[name] = glGetUniformLocation(id, name.c_str()));
		ASSERT(location == -1); // 注意:这里的断言逻辑可能有误,应该检查返回值
	}
	
	return uniformLocationCache[name];
}
