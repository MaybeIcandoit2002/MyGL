#include "Utils.h"

void Sources::LoadImages(const std::string imagePath, Renderer* renderer) {
    if (Instance()->HasImage(imagePath)) return;
    ImageProperty* imgProp = Instance()->GetImage(imagePath);
    unsigned char* localBuffer = stbi_load(imagePath.c_str(), &imgProp->width, &imgProp->height, &imgProp->BPP, 4);
    if (!localBuffer) {
        throw std::runtime_error("Failed to load texture: " + imagePath);
    }
    renderer->AddTexture(localBuffer, imgProp->width, imgProp->height);
    stbi_image_free(localBuffer);
}
void Sources::LoadMeshsFromJson(const std::string& filename, Renderer* renderer) {
    std::ifstream in(filename);
    if (!in) return;
    nlohmann::json meshsJson;
    in >> meshsJson;
    for (auto& meshPair : meshsJson.items()) {
        const std::string& meshName = meshPair.key();
        const nlohmann::json& member = meshPair.value();
        Mesh* mesh = Instance()->GetMesh(meshName);
        std::vector<Vertex2D>* vertexsTemp = utils::ParseVertexs(member["vertexs"]);
        std::vector<uint32_t>* indicesTemp = utils::Parsei4(member["indices"]);
        if (mesh->vertexs != nullptr && mesh->indices != nullptr &&
            vertexsTemp != nullptr && indicesTemp != nullptr &&
            !vertexsTemp->empty() && !indicesTemp->empty()) {
            continue; // 已经加载过完整数据 并且 新数据不为空
        }
        mesh->vertexs = vertexsTemp;
        mesh->indices = indicesTemp;
        if (vertexsTemp != nullptr && !vertexsTemp->empty() &&
            indicesTemp != nullptr && !indicesTemp->empty()) {
            mesh->renderID = renderer->AddData(vertexsTemp->data(), vertexsTemp->size(), indicesTemp->data(), indicesTemp->size());
        }
    }
}

Mesh* Sources::CreateMesh(const std::string& name, int vCount, float* vertecis, int iCount, uint32_t indices, Renderer* renderer)
{
    std::vector<Vertex2D>* vertexsTemp = new std::vector<Vertex2D>();
    std::vector<uint32_t>* indicesTemp = new std::vector<uint32_t>(iCount, indices);
    for (int i = 0; i < vCount; i++)
    {
        vertexsTemp->push_back(
        {
            glm::vec2(vertecis[9 * i], vertecis[9 * i + 1]),
            glm::vec4(vertecis[9 * i + 2], vertecis[9 * i + 3], vertecis[9 * i + 4], vertecis[9 * i + 5]),
            glm::vec2(vertecis[9 * i + 6], vertecis[9 * i + 7]),
            vertecis[9 * i + 8]
        });
    }
    Mesh* mesh = Instance()->GetMesh(name);
    if (mesh->vertexs != nullptr && mesh->indices != nullptr &&
        !vertexsTemp->empty() && !indicesTemp->empty()) {
        return nullptr; // 已经加载过完整数据 并且 新数据不为空
    }
    mesh->vertexs = vertexsTemp;
    mesh->indices = indicesTemp;
    mesh->renderID = renderer->AddData(vertexsTemp->data(), vertexsTemp->size(), indicesTemp->data(), indicesTemp->size());
    return mesh;
}

namespace utils
{
    std::vector<Vertex2D>* ParseVertexs(const nlohmann::json& arr)
    {
        std::vector<Vertex2D>* result = new std::vector<Vertex2D>();
        for (const auto& v : arr) {
            Vertex2D vert = {};
            vert.position = glm::vec2(v[0].get<float>(), v[1].get<float>());
            vert.color = glm::vec4(v[2].get<float>(), v[3].get<float>(), v[4].get<float>(), v[5].get<float>());
            vert.uv = glm::vec2(v[6].get<float>(), v[7].get<float>());
            vert.textureID = v[8].get<float>();
            result->push_back(std::move(vert));
        }
        return result;
    }

    std::vector<uint32_t>* Parsei4(const nlohmann::json& arr)
    {
        std::vector<uint32_t>* result = new std::vector<uint32_t>();
        for (const auto& idx : arr) {
            result->push_back(idx.get<uint32_t>());
        }
        return result;
    }

    std::vector<float>* Parsef4(const nlohmann::json& arr)
    {
        std::vector<float>* result = new std::vector<float>();
        for (const auto& idx : arr) {
            result->push_back(idx.get<float>());
        }
        return result;
    }

    Mesh* CopyMesh(const std::string& name, Mesh* mesh, const Vertex2D& changeInfo, Renderer* renderer)
    {
        int index = 1;
        while (Sources::Instance()->HasMesh(name + "_" + std::to_string(index)))
        {
            index++;
        }
        Mesh* newMesh = Sources::Instance()->GetMesh(name + "_" + std::to_string(index));

        if (renderer)
        {
            newMesh->vertexs = new std::vector<Vertex2D>(*mesh->vertexs);
            newMesh->indices = new std::vector<uint32_t>(*mesh->indices);
            for (Vertex2D& v : *newMesh->vertexs)
            {
                v.position *= changeInfo.position;
                v.color = changeInfo.color;
                v.uv *= changeInfo.uv;
                v.textureID = changeInfo.textureID;
            }
            mesh->renderID = renderer->AddData(newMesh->vertexs->data(), newMesh->vertexs->size(), newMesh->indices->data(), newMesh->indices->size());
        }
        else
        {
            newMesh->vertexs = mesh->vertexs;
            newMesh->indices = mesh->indices;
            newMesh->renderID = mesh->renderID;
        }
        return mesh;
    }
}