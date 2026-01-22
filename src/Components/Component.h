#pragma once
#include <glm.hpp>
#include <vector>
#include <cstdint>

#include "Utils.h"
#include "../renderComponents/renderer.h"

class Component
{
private:
	bool hasPhysicBody;					// 是否有物理刚体

	Mesh* mesh;							// 组件应用的网格
public:
	uint16_t meshIndex;					// 组件在网格中的索引

	UITransform transform;				// 组件的变换属性

	glm::vec4 backgroundColor;			// 组件的背景颜
	uint16_t textureSlot;				// 组件的纹理槽位

	Component* parent;					// 组件的父组件
	std::vector<Component*> children;	// 组件的子组件

public:
	Component(bool hasPhysicBody, Mesh* mesh) :
		hasPhysicBody(hasPhysicBody), mesh(mesh),
		meshIndex(uint16_t(mesh->transform.size())),
		transform({ glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f }),
		backgroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)), textureSlot(0),
		parent(nullptr) {
		mesh->count++;
		mesh->changedCount = true;
		mesh->transform.push_back(UniformData{});
	}
	~Component() {
		for (Component* child: children)
		{
			child->~Component();
		}
	}
	void Draw(Renderer* renderer);
};