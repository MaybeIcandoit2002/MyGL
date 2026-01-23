#include "Renderer.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << "):"
            << function << " "
            << file << ":"
            << line
            << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer(const std::string vertexShaderPath, const std::string fragmentShaderPath)
    : VBOffset(0), IBOffset(0), shader(nullptr), VAO(nullptr), VBO(nullptr), IBO(nullptr)
{
    // 启用 2D 渲染模式
    Only2D();
    
    // 配置顶点缓冲布局
    layout = new VertexLayout();
    layout->Push<float>(2, GL_FALSE);   // position (vec2)
    layout->Push<float>(4, GL_FALSE);   // color (vec4)
    layout->Push<float>(2, GL_FALSE);   // uv 纹理坐标 (vec2)
    layout->Push<float>(1, GL_FALSE);   // textureID 纹理索引 (float)

    // 创建并配置着色器程序
    shader = new ShaderProgram();
    shader->Add(GL_VERTEX_SHADER, vertexShaderPath);
    shader->Add(GL_FRAGMENT_SHADER, fragmentShaderPath);
    shader->Link();

    // 预分配 8 个纹理槽位
    textures.reserve(8);
}

uint16_t Renderer::AddTexture(const unsigned char* textureBuffer, int width, int height)
{
    int index = textures.size();
    // 检查是否达到最大纹理数量
    if (index == textures.capacity()) { return UINT16_MAX; }
    
    // 在预分配的空间中使用 placement new 构造 Texture 对象
    textures.emplace_back(textureBuffer, width, height);
    // 绑定纹理到对应的纹理单元 (GL_TEXTURE0 + index)
	textures[index].Bind(index);
    return static_cast<uint16_t>(index);
}

uint16_t Renderer::AddData(const Vertex2D* vertexs, uint32_t vertexCount, const uint32_t* indices, uint32_t indexCount)
{
    // 记录当前缓冲区的字节偏移量
    uint16_t currentVBOffset = VBOffset;
    uint16_t currentIBOffset = IBOffset;
    
    // 更新顶点和索引缓冲的偏移量(字节为单位)
    VBOffset += vertexCount * sizeof(Vertex2D);
    IBOffset += indexCount * sizeof(uint32_t);
    
    // 添加顶点数据到缓冲区
    for (uint32_t i = 0; i < vertexCount; i++)
    {
        this->vertexs.push_back(vertexs[i]);
    }
    
    // 添加索引数据,索引需要加上顶点偏移
    // 应该使用顶点数量偏移,而不是字节偏移
    for (uint32_t i = 0; i < indexCount; i++)
    {
        this->indices.push_back(indices[i] + currentVBOffset / sizeof(Vertex2D));
    }

    // 返回当前索引缓冲的字节偏移(用于 glDrawElements 的 offset 参数)
    return currentIBOffset;
}

void Renderer::SendToGPU()
{
    // 创建并上传顶点缓冲
    VBO = new VertexBuffer(vertexs.data(), VBOffset);
    
    // 创建并上传索引缓冲
    IBO = new IndexBuffer(indices.data(), IBOffset / sizeof(uint32_t));
    
    // 创建顶点数组对象,关联 VBO 和顶点布局
    VAO = new VertexArray(*VBO, *layout);
    
    // 设置纹理采样器数组,对应着色器中的 uniform sampler2D u_Textures[8]
    // 数组值 {0, 1, 2, ...} 表示纹理单元编号(GL_TEXTURE0, GL_TEXTURE1, ...)
    int textureIndices[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    shader->SetUniform1iv("u_Textures", textures.size(), textureIndices);
}

void Renderer::Clear(glm::vec4 color) const
{
    glClearColor(color.r, color.g, color.b, color.a);
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(uint32_t offset, uint32_t indexCount, uint32_t instanceCount) const
{
    // 绑定顶点数组对象(自动激活 VBO 和布局配置)
    VAO->Bind();
    // 绑定索引缓冲对象
    IBO->Bind();
    // 绑定着色器程序
    shader->Bind();
    GLCall(glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (const void*)(offset), instanceCount));
}

void Renderer::Only2D() const
{
    // 禁用深度测试 - 2D 渲染不需要深度信息
    GLCall(glDisable(GL_DEPTH_TEST));
    // 禁用面剔除 - 2D 图形需要双面可见
    GLCall(glDisable(GL_CULL_FACE));
    // 启用颜色混合 - 支持透明度和半透明效果
    GLCall(glEnable(GL_BLEND));
    // 设置混合函数 - 标准 Alpha 混合
    // 公式: finalColor = srcColor * srcAlpha + dstColor * (1 - srcAlpha)
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}
