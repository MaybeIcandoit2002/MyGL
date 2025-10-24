#pragma once
#include <fstream>
#include "vendor/json.hpp"
#include "MyWindow.h"
#include "Models.h"
#include "renderComponents/Layout.h"

std::vector<Vertexs2D> ParseVertexs(const nlohmann::json& arr) {
	std::vector<Vertexs2D> result = std::vector<Vertexs2D>();
    for (const auto& v : arr) {
        Vertexs2D vert = {};
        vert.position = glm::vec2(v[0].get<float>(), v[1].get<float>());
        vert.color    = glm::vec4(v[2].get<float>(), v[3].get<float>(), v[4].get<float>(), v[5].get<float>());
        vert.texCoord = glm::vec2(v[6].get<float>(), v[7].get<float>());
        vert.texID    = v[8].get<float>();
        result.push_back(vert);
    }
    return result;
}

std::vector<uint32_t> Parsei4(const nlohmann::json& arr) {
	std::vector<uint32_t> result = std::vector<uint32_t>();
    for (const auto& idx : arr) {
        result.push_back(idx.get<int>());
    }
    return result;
}

std::vector<float> Parsef4(const nlohmann::json& arr) {
	std::vector<float> result = std::vector<float>();
    for (const auto& idx : arr) {
        result.push_back(idx.get<float>());
    }
    return result;
}

void LoadModelsFromJson(
	MyWindow* window,
    const std::string& filename,
	std::vector<std::pair<std::string, uint32_t>>& models
) {
    std::ifstream in(filename);
    if (!in) return;
    nlohmann::json source;
    in >> source;
    for (auto& modelNameCount : models) {
		const std::string& modelName = modelNameCount.first;
        if (!source.contains(modelName)) continue;
        auto& member = source[modelName];
        Models* model = new Models(window);
		model->SetModelCount(modelNameCount.second);
        std::vector<Vertexs2D> vertexs = ParseVertexs(member["vertexs"]);
        std::vector<uint32_t> indices = Parsei4(member["indices"]);
		model->InitializeModelData(vertexs, indices);
        std::vector<float> pose = Parsef4(member["pose"]);
        model->SetPose(pose[0], pose[1], pose[2], pose[3]);
        if (member.contains("physicProperties")) {
            std::vector<float> physicProperties = Parsef4(member["physicProperties"]);
            model->InitializePhysicProperties(physicProperties);
        }
    }
}