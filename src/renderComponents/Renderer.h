#pragma once
#include <cstdint>
#include <GL/glew.h>
#include <vector>
#include <unordered_map>

#include "Layout.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "../Macros.h"

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__));

/**
 * @brief 清除所有 OpenGL 错误标志
 * 
 * 循环调用 glGetError() 直到没有错误,用于在调试前清空错误队列
 */
void GLClearError();
/**
 * @brief 检查并记录 OpenGL 错误
 * @param function 调用的函数名
 * @param file 源文件名
 * @param line 行号
 * @return 如果没有错误返回 true,否则返回 false
 *
 * 用于 GLCall 宏,自动捕获和打印 OpenGL 错误信息
 */
bool GLLogCall(const char* function, const char* file, int line);

class Texture
{
private:
	uint32_t id;
public:
	Texture(const unsigned char* localBuffer, int width, int height);
	~Texture();
	void Bind(uint32_t slot = 0) const;
	void Unbind() const;
};
class Shader
{
public:
    uint32_t id;           // OpenGL 着色器对象 ID
    uint32_t type;         // 着色器类型 (GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER)
    std::string filePath;  // 着色器源文件路径
	/**
	* @brief 构造函数 - 创建并编译着色器
	* @param type 着色器类型 (GL_VERTEX_SHADER 或 GL_FRAGMENT_SHADER)
	* @param filePath 着色器源代码文件路径
	*/
	Shader(uint32_t type, const std::string filePath);
	/**
	* @brief 编译着色器
	*
	* 流程:
	* 1. 创建着色器对象
	* 2. 从文件加载源代码
	* 3. 设置着色器源代码
	* 4. 编译着色器
	* 5. 检查编译错误
	*/
    void Compile();
	/**
	* @brief 删除着色器对象
	*
	* 释放 OpenGL 着色器资源
	*/
    void Remove();

private:
	/**
	* @brief 从文件解析着色器源代码
	* @param filepath 着色器文件路径
	* @return 着色器源代码字符串
	*/
    inline std::string ParseShader(const std::string& filepath);
	/**
	* @brief 检查着色器编译错误
	*
	* 如果编译失败:
	* 1. 获取错误日志
	* 2. 打印错误信息
	* 3. 删除着色器对象
	* 4. 将 id 重置为 0
	*/
	inline void CheckShaderError();
};
class ShaderProgram
{
private:
	uint32_t id;
	std::vector<Shader*> shaders;
	mutable std::unordered_map<std::string, int> uniformLocationCache;
public:
	ShaderProgram();
	~ShaderProgram();
	void Add(uint32_t type, const std::string& path);
	void Link() const;
	void Bind() const;
	void Unbind() const;
	void SetUniform1i(const std::string& name, int value);
	void SetUniform1iv(const std::string& name, int count, int* value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4 matrix);
	void SetUniformBlock(const std::string& name, int32_t bindIndex) const;
private:
	int GetUniformLocation(const std::string& name) const;
};

struct Vertex2D
{
	glm::vec2 position;
	glm::vec4 color;
	glm::vec2 uv;
	float textureID;
};

struct UniformData
{
	glm::mat3x4 positionTransform = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	};
	glm::vec4 color = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 uvTransform = { 0.0f, 0.0f, 0.0f, 0.0f };
	glm::vec4 texIndex = { -1.0f, 0.0f, 0.0f, 0.0f };
};

class Renderer
{
private:
	ShaderProgram* shader;
	VertexArray* VAO;

	VertexBuffer* VBO;
	std::vector<Vertex2D> vertexs;
	uint32_t VBOffset;

	IndexBuffer* IBO;
	std::vector<uint32_t> indices;
	VertexLayout* layout;
	uint32_t IBOffset;

	std::vector<Texture> textures;
public:
	/**
	 * @brief 渲染器构造函数
	 * @param vertexShaderPath 顶点着色器文件路径
	 * @param fragmentShaderPath 片段着色器文件路径
	 *
	 * 初始化流程:
	 * 1. 设置 2D 渲染状态
	 * 2. 配置顶点缓冲布局
	 * 3. 加载并链接着色器
	 * 4. 预分配纹理容器
	 */
	Renderer(const std::string vertexShaderPath, const std::string fragmentShaderPath);
	/**
	 * @brief 添加纹理到渲染器
	 * @param textureBuffer 纹理数据缓冲(RGBA 格式)
	 * @param width 纹理宽度
	 * @param height 纹理高度
	 * @return 纹理索引,失败返回 UINT16_MAX
	 *
	 * 使用 placement new 在预分配的 vector 空间中构造纹理对象,
	 * 并将纹理绑定到对应的纹理单元
	 */
	uint16_t AddTexture(const unsigned char* textureBuffer, int width, int height);
	/**
	 * @brief 添加顶点和索引数据到渲染批次
	 * @param vertexs 顶点数组指针
	 * @param vertexCount 顶点数量
	 * @param indices 索引数组指针
	 * @param indexCount 索引数量
	 * @return 当前索引缓冲的字节偏移量
	 *
	 * 将顶点和索引数据添加到内部缓冲区,用于批量渲染。
	 * 索引会自动调整以匹配当前的顶点偏移。
	 *
	 * 注意: currentVBOffset 是字节偏移,但用于索引偏移存在逻辑错误,
	 *       应使用顶点数量而非字节数
	 */
	uint16_t AddData(const Vertex2D* vertexs, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount);
	/**
	 * @brief 将所有数据上传到 GPU
	 *
	 * 创建并配置 OpenGL 缓冲对象:
	 * 1. 创建顶点缓冲对象(VBO)并上传顶点数据
	 * 2. 创建索引缓冲对象(IBO)并上传索引数据
	 * 3. 创建顶点数组对象(VAO)并关联 VBO 和布局
	 * 4. 设置纹理采样器 uniform 数组
	 */
	void SendToGPU();

	/**
	 * @brief 设置清屏颜色(未使用)
	 * @param color 清屏颜色 (RGBA)
	 */
	void Clear(glm::vec4 color) const;
	/**
	 * @brief 清除颜色缓冲区
	 * @param color 清屏颜色(参数未使用)
	 *
	 * 使用当前设置的清屏颜色清除颜色缓冲区
	 */
	void Draw(uint32_t offset, uint32_t indexCount, uint32_t instanceCount) const;
	/**
	 * @brief 执行实例化绘制
	 * @param offset 索引缓冲的字节偏移量
	 * @param indexCount 要绘制的索引数量
	 * @param instanceCount 实例数量
	 *
	 * 绑定必要的 OpenGL 对象并执行实例化绘制调用。
	 * 使用三角形图元,无符号整型索引。
	 */
	ShaderProgram* GetShader() const { return shader; }
private:
	/**
	 * @brief 配置 2D 渲染状态
	 *
	 * 设置 OpenGL 状态以优化 2D 渲染:
	 * 1. 禁用深度测试(2D 不需要 Z 缓冲)
	 * 2. 禁用面剔除(2D 图形双面可见)
	 * 3. 启用混合(支持透明度)
	 * 4. 设置标准 Alpha 混合模式
	 */
	inline void Only2D() const;
};