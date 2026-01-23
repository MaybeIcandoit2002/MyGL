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
		meshIndex(uint16_t(mesh->uniform.size())),
		transform({ glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f }),
		backgroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 0.0f)), textureSlot(0),
		parent(nullptr) {
		mesh->count++;
		mesh->changedCount = true;
		mesh->uniform.push_back(UniformData{});
	}
	~Component() {
		for (Component* child : children)
		{
			child->~Component();
		}
	}
	void AddChild(Component* child);
	void GetPosition(glm::vec2& position);
	void SetPosition(glm::vec2 position);
	void SetPosition(float x, float y);
	void GetWorldPosition(glm::vec2& position);
	void SetWorldPosition(glm::vec2 position);
	void SetWorldPosition(float x, float y);
	void GetScale(glm::vec2& scale);
	void SetScale(glm::vec2 scale);
	void SetScale(float x, float y);
	void GetWorldScale(glm::vec2& scale);
	void SetWorldScale(glm::vec2 scale);
	void SetWorldScale(float x, float y);
	void GetRotation(float& rotation);
	void SetRotation(float rotation);
	void GetWorldRotation(float& rotation);
	void SetWorldRotation(float rotation);
	void GetBackgroundColor(glm::vec4& color);
	void SetBackgroundColor(glm::vec4 color);
	void SetBackgroundColor(float r, float g, float b, float a);

	void BeforeUpdate(Renderer* renderer);
	void Update(Renderer* renderer);
};