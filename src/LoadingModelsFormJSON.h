#pragma once
#include <fstream>
#include "vendor/json.hpp"
#include "Models.h"
#include "renderComponents/Layout.h"

std::vector<Vertexs2D> ParseVertexs(const nlohmann::json& arr) {
    std::vector<Vertexs2D> result;
    for (const auto& v : arr) {
        Vertexs2D vert;
        vert.position = glm::vec2(v[0].get<float>(), v[1].get<float>());
        vert.color    = glm::vec4(v[2].get<float>(), v[3].get<float>(), v[4].get<float>(), v[5].get<float>());
        vert.texCoord = glm::vec2(v[6].get<float>(), v[7].get<float>());
        vert.texID    = v[8].get<float>();
        result.push_back(vert);
    }
    return result;
}

std::vector<uint32_t> Parsei4(const nlohmann::json& arr) {
    std::vector<uint32_t> result;
    for (const auto& idx : arr) {
        result.push_back(idx.get<int>());
    }
    return result;
}

std::vector<float> Parsef4(const nlohmann::json& arr) {
    std::vector<float> result;
    for (const auto& idx : arr) {
        result.push_back(idx.get<float>());
    }
    return result;
}

void LoadModelsFromJson(
    const std::string& filename,
	const std::string& modelName,
    const VertexBufferLayout* layout,
    const ShaderProgram* shader,
	Models*& outModel
) {
    std::ifstream in(filename);
    nlohmann::json source;
    in >> source;

	auto& member = source[modelName];
    std::vector<Vertexs2D> vertexs = ParseVertexs(member["vertexs"]);
    std::vector<uint32_t> indices = Parsei4(member["indices"]);
    Models* model = new Models(
        vertexs.data(),
        indices.data(),
        layout,
        (uint32_t)vertexs.size(),
        (uint32_t)indices.size(),
        shader
    );

    std::vector<float> pose = Parsef4(member["pose"]);
    model->SetPose(pose[0], pose[1], pose[2], pose[3]);

    if (member.contains("physicProperties")) {
		std::vector<float> pP = Parsef4(member["physicProperties"]);
        model->SetPhysicProperties(pP.data());
    }
	outModel = model;
}