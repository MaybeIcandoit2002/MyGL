#include "Utils.h"

Sources* Sources::instance = nullptr;

namespace utils
{
    std::vector<Vertex2D>* ParseVertexs(const nlohmann::json& arr) {
        std::vector<Vertex2D>* result = new std::vector<Vertex2D>();
        for (const auto& v : arr) {
            Vertex2D vert = {};
            vert.position = glm::vec2(v[0].get<float>(), v[1].get<float>());
            vert.color = glm::vec4(v[2].get<float>(), v[3].get<float>(), v[4].get<float>(), v[5].get<float>());
            vert.uv = glm::vec2(v[6].get<float>(), v[7].get<float>());
            vert.textureID = v[8].get<float>();
            result->push_back(vert);
        }
        return result;
    }

    std::vector<uint32_t>* Parsei4(const nlohmann::json& arr) {
        std::vector<uint32_t>* result = new std::vector<uint32_t>();
        for (const auto& idx : arr) {
            result->push_back(idx.get<int>());
        }
        return result;
    }

    std::vector<float>* Parsef4(const nlohmann::json& arr) {
        std::vector<float>* result = new std::vector<float>();
        for (const auto& idx : arr) {
            result->push_back(idx.get<float>());
        }
        return result;
    }

    void CopyMesh(const std::string& name, Mesh* mesh) {
        int index = 1;
        while (Sources::GetInstance()->HasMesh(name + "_" + std::to_string(index))) {
            index++;
        }
        Mesh* newMesh = Sources::GetInstance()->GetMesh(name + "_" + std::to_string(index));
        newMesh->renderID = mesh->renderID;
        newMesh->vertexs = mesh->vertexs;
        newMesh->indices = mesh->indices;
    }

    void LoadImages(const std::string imagePath, Renderer* renderer) {
        if (Sources::GetInstance()->HasImage(imagePath)) return;
        ImageProperty* imgProp = Sources::GetInstance()->GetImage(imagePath);
        unsigned char* localBuffer = stbi_load(imagePath.c_str(), &imgProp->width, &imgProp->height, &imgProp->BPP, 4);
        if (!localBuffer) {
            throw std::runtime_error("Failed to load texture: " + imagePath);
        }
        renderer->AddTexture(localBuffer, imgProp->width, imgProp->height);
        stbi_image_free(localBuffer);
    }
    void LoadMeshsFromJson(const std::string& filename, Renderer* renderer) {
        std::ifstream in(filename);
        if (!in) return;
        nlohmann::json meshsJson;
        in >> meshsJson;
        for (auto& meshPair : meshsJson.items()) {
            const std::string& meshName = meshPair.key();
            const nlohmann::json& member = meshPair.value();
            Mesh* mesh = Sources::GetInstance()->GetMesh(meshName);
            if (mesh->vertexs != nullptr && mesh->indices != nullptr) {
                continue; // 已经加载过完整数据
            }
            mesh->vertexs = ParseVertexs(member["vertexs"]);
            mesh->indices = Parsei4(member["indices"]);
			mesh->renderID = renderer->AddData(mesh->vertexs->data(), mesh->vertexs->size(), mesh->indices->data(), mesh->indices->size());
        }
    }
}