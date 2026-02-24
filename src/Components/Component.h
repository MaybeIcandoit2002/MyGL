#pragma once
#include <glm.hpp>

#include "../renderComponents/renderer.h"
#include "../collisionSystem/PhysicWorld.h"

#include "Utils.h"

class Component
{
protected:

	uint16_t meshIndex;					// 组件在网格中的索引
	Mesh* mesh;							// 组件应用的网格
	UITransform transform;				// 组件的变换属性

	glm::vec4 backgroundColor;			// 组件的背景颜
	uint16_t textureSlot;				// 组件的纹理槽位

	cpShape* shape;						// 物理形状

	Component* parent;					// 组件的父组件
	std::vector<Component*> children;	// 组件的子组件
public:
	bool hasPhysicBody;					// 是否有物理刚体

	bool enabled = true;				// 用于触摸系统过滤

	Component(Mesh* mesh) :
		hasPhysicBody(false), mesh(mesh),
		meshIndex(uint16_t(mesh->uniform.size())),
		transform({ glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f }),
		backgroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)), textureSlot(0),
		shape(nullptr),
		parent(nullptr)
	{
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
	void BeforeUpdate(Renderer* renderer);
	void Update(Renderer* renderer);

	void AddChild(Component* child);
	void RemoveChild(Component* child);
	void SetPosition(glm::vec2 position);
	void SetPosition(float x, float y);
	void SetWorldPosition(glm::vec2 position);
	void SetWorldPosition(float x, float y);
	void SetScale(glm::vec2 scale);
	void SetScale(float x, float y);
	void SetWorldScale(glm::vec2 scale);
	void SetWorldScale(float x, float y);
	void SetRotation(float rotation);
	void SetWorldRotation(float rotation);
	void SetBackgroundColor(glm::vec4 color);
	void SetBackgroundColor(float r, float g, float b, float a);

	void GetPosition(glm::vec2& position) const;
	void GetWorldPosition(glm::vec2& position) const;
	void GetScale(glm::vec2& scale) const;
	void GetWorldScale(glm::vec2& scale) const;
	void GetRotation(float& rotation) const;
	void GetWorldRotation(float& rotation) const;
	void GetBackgroundColor(glm::vec4& color) const;

	/// <summary>
	/// 初始化圆形物理碰撞体和刚体。
	/// </summary>
	/// <param name="radius">圆形碰撞体半径</param>
	/// <param name="mass">刚体质量</param>
	/// <param name="restitution">弹性系数（0-1，越大越“弹”）</param>
	/// <param name="friction">摩擦系数（0-1，越大摩擦越大）</param>
	void InitPhsicProperty(float radius, float mass, float restitution, float friction);
	/// <summary>
	/// 初始化矩形物理碰撞体和刚体。
	/// 以组件中心为原点创建一个轴对齐矩形形状。
	/// </summary>
	/// <param name="width">矩形宽度</param>
	/// <param name="height">矩形高度</param>
	/// <param name="mass">刚体质量</param>
	/// <param name="restitution">弹性系数（0-1，越大越“弹”）</param>
	/// <param name="friction">摩擦系数（0-1，越大摩擦越大）</param>
	void InitPhsicProperty(float width, float height, float mass, float restitution, float friction);
	/// <summary>
	/// 初始化多边形物理碰撞体和刚体。
	/// 顶点坐标为局部空间坐标，以组件中心为原点。
	/// </summary>
	/// <param name="count">多边形顶点数量</param>
	/// <param name="vertecies">顶点数组指针，长度为 count * 2，按 (x0, y0, x1, y1, ...) 排列</param>
	/// <param name="mass">刚体质量</param>
	/// <param name="restitution">弹性系数（0-1，越大越“弹”）</param>
	/// <param name="friction">摩擦系数（0-1，越大摩擦越大）</param>
	void InitPhsicProperty(int count, float* vertecies, float mass, float restitution, float friction);
	inline void SetMass(float mass) { cpShapeSetMass(shape, mass); }
	inline void SetRestitution(float restitution) { cpShapeSetElasticity(shape, restitution); }
	inline void SetFriction(float friction) { cpShapeSetFriction(shape, friction); }
	inline void synPosition() { transform.position = utils::ParseVec(cpBodyGetPosition(cpShapeGetBody(shape))); }

	inline float GetMass() const { return static_cast<float>(cpShapeGetMass(shape)); }
	inline float GetMoment() const { return static_cast<float>(cpShapeGetMoment(shape)); }
	inline float GetRestitution() const { return static_cast<float>(cpShapeGetElasticity(shape)); }
	inline float GetFriction() const { return static_cast<float>(cpShapeGetFriction(shape)); }
	inline glm::vec2 GetVelocity() const { return utils::ParseVec(cpBodyGetVelocity(cpShapeGetBody(shape)));}
	inline glm::vec2 GetForce() const { return utils::ParseVec(cpBodyGetForce(cpShapeGetBody(shape))); }
	inline glm::vec2 GetAcceleration() const { return GetForce() / GetMass(); }
	inline glm::vec2 GetMomentum() const { return GetVelocity() * GetMass(); }
	inline float GetAngle() const { return static_cast<float>(cpBodyGetAngle(cpShapeGetBody(shape))); }
	inline float GetAngleVelocity() const { return static_cast<float>(cpBodyGetAngularVelocity(cpShapeGetBody(shape))); }
	inline float GetKineticEnergy() const { return static_cast<float>(cpBodyKineticEnergy(cpShapeGetBody(shape))); }

	bool checkPointInShape(float x, float y) const { return cpShapePointQuery(shape, cpv(x, y), nullptr) != 0; }
};