#pragma once
#include <vector>
#include <map>
#include <fstream>
#include <cstdint>
#include <algorithm>

#include "../vendor/json.hpp"
#include "../vendor/stb_image/stb_image.h"
#include "../collisionSystem/PhysicWorld.h"
#include "../renderComponents/Renderer.h"

typedef UniformBuffer* MeshBuffer;
struct ImageProperty { uint16_t slot; int width; int height; int BPP; };
struct Mesh
{
    bool isDraw = false;
	bool changedCount = false;
    uint16_t count = 0;
    uint16_t renderID = 0;

    std::vector<Vertex2D>* vertexs = nullptr;
    std::vector<uint32_t>* indices = nullptr;

    std::vector<UniformData> uniform;
    std::vector<class Component*> owners;

    MeshBuffer transformBuffer = nullptr;
};
struct ComponentTemplate
{
    std::string name;
    std::string meshName;
    float scale1;
    float scale2;
    float* backgroundColor;
    int textureSlot;
    ShapeType shapeType;
    float physicSize1;
    float* physicSize2;
    float physicMass;
    float physicFriction;
	float physicRestitution;
};
struct UITransform
{
    glm::vec2 position;
    glm::vec2 scale;
    float rotation;
};
class Sources
{
private:
	std::map<std::string, ImageProperty> images = {};
    std::map<std::string, Mesh> meshs = {};
	std::vector<ComponentTemplate> componentTemplates = {};
public:
    std::vector<ComponentTemplate>& GetComponentTemplate() { return componentTemplates; }
    bool HasImage(const std::string& imagePath) { return images.find(imagePath) != images.end(); }
    bool HasMesh(const std::string& meshName) { return meshs.find(meshName) != meshs.end(); }
    ImageProperty* GetImage(const std::string& imagePath) { return &images[imagePath]; }
    Mesh* GetMesh(const std::string& meshName) { return &meshs[meshName]; }
    void LoadImages(const std::string& imagePath, Renderer* renderer);
    void LoadMeshsFromJson(const std::string& filename, Renderer* renderer);
	void LoadComponentFromJson(const std::string& filename);
    Mesh* CreateMesh(const std::string& name, int vCount, float* vertecis, int iCount, uint32_t indices, Renderer* renderer);
    void DeleteMesh(const std::string& meshName) {
        delete meshs[meshName].vertexs;
        delete meshs[meshName].indices;
        delete meshs[meshName].transformBuffer;
		meshs.erase(meshName);
    }
    inline void DrawOver() {
        for (auto& meshPair : meshs) {
			meshPair.second.isDraw = false; 
		}
    }
private:
    static Sources* instance;
    Sources() = default;
    Sources(const Sources&) = delete;
    ~Sources() = default;
    Sources& operator=(const Sources&) = delete;
public:
    static Sources* Instance() {
        if (!instance) instance = new Sources();
        return instance;
    }
};

namespace utils
{
    inline glm::vec2 ParseVec(const cpVect v) { return glm::vec2(static_cast<float>(v.x), static_cast<float>(v.y)); }
    std::vector<Vertex2D>* ParseVertexs(const nlohmann::json& arr);
    std::vector<uint32_t>* Parsei4(const nlohmann::json& arr);
    std::vector<float>* Parsef4(const nlohmann::json& arr);
    /// <summary>
    /// 以给定 mesh 为模板，创建一个新的 mesh 副本。
    /// 如果提供 changeInfo 和 renderer，可在复制后对顶点进行调整并重新上传渲染数据。
    /// </summary>
    /// <param name="name">新 mesh 的名称前缀（name_序号），用于在 Sources 中生成唯一名称。</param>
    /// <param name="mesh">要作为模板复制的源 mesh 指针，必须为有效且已初始化的 mesh。</param>
    /// <param name="changeInfo">
    /// 可选的顶点修改信息。
    /// 根据具体实现可用于在复制后的 mesh 上做尺寸、位置、颜色等调整。
    /// </param>
    /// <param name="renderer">
    /// 可选的渲染器指针。
    /// 若非空且复制过程中修改了顶点/索引数据，可通过该渲染器重新创建 GPU 缓冲并更新 renderID。
    /// </param>
    /// <returns>
    /// 返回指向新创建 mesh 的指针。
    /// </returns>
    Mesh* CopyMesh(const std::string& name, Mesh* mesh, const Vertex2D& changeInfo = Vertex2D{}, Renderer* renderer = nullptr);
}