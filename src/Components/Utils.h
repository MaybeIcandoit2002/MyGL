#pragma once
#include <vector>
#include <map>
#include <fstream>

#include "../vendor/json.hpp"
#include "../vendor/stb_image/stb_image.h"
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

    MeshBuffer transformBuffer = nullptr;
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
    std::map<std::string, ImageProperty> images;
    std::map<std::string, Mesh> meshs;
public:
    bool HasImage(const std::string& imagePath) { return images.find(imagePath) != images.end(); }
    ImageProperty* GetImage(const std::string& imagePath) { return &images[imagePath]; }
	bool HasMesh(const std::string& meshName) { return meshs.find(meshName) != meshs.end(); }
    Mesh* GetMesh(const std::string& meshName) { return &meshs[meshName]; }
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
    Sources() {};
    Sources(const Sources&) = delete;
    ~Sources() {};
    Sources& operator=(const Sources&) = delete;
public:
    static Sources* GetInstance() {
        if (instance == nullptr) instance = new Sources();
        return instance;
    }
};

namespace utils
{
    std::vector<Vertex2D>* ParseVertexs(const nlohmann::json& arr);
    std::vector<uint32_t>* Parsei4(const nlohmann::json& arr);
    std::vector<float>* Parsef4(const nlohmann::json& arr);
    void CopyMesh(const std::string& name, Mesh* mesh);
    void LoadImages(const std::string imagePath, Renderer* renderer);
    void LoadMeshsFromJson(const std::string& filename, Renderer* renderer);
}