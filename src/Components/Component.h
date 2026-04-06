#pragma once

#include <glm.hpp>
#include <cstdint>

#include "../renderComponents/renderer.h"
#include "../collisionSystem/PhysicWorld.h"

#include "Utils.h"

class Component
{
protected:
	static std::uint64_t nextStableId;
	std::uint64_t stableId;

	uint16_t meshIndex;					// 组件在网格中的索引
	Mesh* mesh;							// 组件应用的网格
	UITransform transform;				// 组件的变换属性
	glm::vec2 physicRelativePosition;
	glm::vec2 physicRelativeScale;
    std::vector<cpVect> physicVertices;

	glm::vec4 backgroundColor;			// 组件的背景颜
	int textureSlot;				// 组件的纹理槽位

	cpShape* shape;						// 物理形状

	Component* parent;					// 组件的父组件
public:
	std::vector<Component*> children;
	std::string name;
	std::string templateName;
	ShapeType shapeType;
	float physicMass;					// 物理质量
	float physicSize1;					// 物理尺寸1（圆形为半径，矩形为宽度，多边形为顶点数量）
	float* physicSize2;					// 物理尺寸2（矩形为高度，多边形为顶点数组指针）
	bool hasPhysicBody;					// 是否有物理刚体

	bool enabled = true;				// 用于触摸系统过滤
	bool isDescriptionComponent = false;
	std::string descriptionText;
	glm::vec2 descriptionOffset = glm::vec2(0.0f, 40.0f);
	bool descriptionShowParams = false;
	bool descriptionShowParamName = true;
	bool descriptionShowParamPosition = true;
	bool descriptionShowParamScale = false;
	bool descriptionShowParamRotation = false;
	bool descriptionShowParamPhysMass = false;
	bool descriptionShowParamPhysFriction = false;
	bool descriptionShowParamPhysRestitution = false;
	bool descriptionShowParamPhysVelocity = false;
	bool descriptionShowParamPhysAngularVelocity = false;
	float descriptionFontSize = 16.0f;
	float descriptionLineSpacing = 2.0f;

	Component(Mesh* mesh) :
		hasPhysicBody(false), mesh(mesh), stableId(nextStableId++),
		meshIndex(uint16_t(mesh->uniform.size())),
		transform({ glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f }),
		physicRelativePosition(0.0f, 0.0f),
		physicRelativeScale(1.0f, 1.0f),
		backgroundColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)), textureSlot(-1),
		shape(nullptr),
		parent(nullptr),
		templateName(""),
		shapeType(ShapeType::Box), physicMass(0), physicSize1(0), physicSize2(nullptr)
	{
		mesh->count++;
		mesh->changedCount = true;
		mesh->uniform.push_back(UniformData{});
		mesh->owners.push_back(this);
	}
	~Component() {
		for (Component* child : children)
		{
			delete child;
		}
		if (mesh && meshIndex < mesh->uniform.size() && mesh->uniform.size() == mesh->owners.size())
		{
			const uint16_t lastIndex = static_cast<uint16_t>(mesh->uniform.size() - 1);
			if (meshIndex != lastIndex)
			{
				mesh->uniform[meshIndex] = mesh->uniform[lastIndex];
				mesh->owners[meshIndex] = mesh->owners[lastIndex];
				if (mesh->owners[meshIndex])
					mesh->owners[meshIndex]->meshIndex = meshIndex;
			}
			mesh->uniform.pop_back();
			mesh->owners.pop_back();
			mesh->count = static_cast<uint16_t>(mesh->uniform.size());
			mesh->changedCount = true;
		}
		if (shape)
		{
			cpBody* body = cpShapeGetBody(shape);
			PhysicWorld* world = PhysicWorld::Instance();
			if (world)
				world->RemoveJointsByBody(body);

			cpSpace* shapeSpace = cpShapeGetSpace(shape);
			if (shapeSpace)
				cpSpaceRemoveShape(shapeSpace, shape);

			cpSpace* bodySpace = cpBodyGetSpace(body);
			if (bodySpace)
				cpSpaceRemoveBody(bodySpace, body);

			cpShapeFree(shape);
			cpBodyFree(body);
			shape = nullptr;
		}
	}
	void BeforeUpdate(Renderer* renderer, bool physicRunning);
	void Update(Renderer* renderer);

	void AddChild(Component* child);
	void RemoveChild(Component* child);
	Component* GetDescriptionChild() const;
	Component* CreateOrGetDescriptionChild();
	void RemoveDescriptionChild();
	inline Component* GetParent() const { return parent; }
	bool CheckPointInVisualRect(float x, float y) const;
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
	inline void SetTextureSlot(int slot) { textureSlot = slot; }
	void SetPhysicRelativePosition(glm::vec2 position);
	void SetPhysicRelativePosition(float x, float y);
	void SetPhysicRelativeScale(glm::vec2 scale);
	void SetPhysicRelativeScale(float x, float y);
	void SetPhysicSize(float size1, float size2 = 0.0f);

	void GetPosition(glm::vec2& position) const;
	void GetWorldPosition(glm::vec2& position) const;
	void GetScale(glm::vec2& scale) const;
	void GetWorldScale(glm::vec2& scale) const;
	void GetRotation(float& rotation) const;
	void GetWorldRotation(float& rotation) const;
	void GetBackgroundColor(glm::vec4& color) const;
	inline int GetTextureSlot() const { return textureSlot; }
	void GetPhysicRelativePosition(glm::vec2& position) const;
	void GetPhysicRelativeScale(glm::vec2& scale) const;

	/// <summary>
	/// 初始化圆形物理碰撞体和刚体。
	/// </summary>
	/// <param name="radius">圆形碰撞体半径</param>
	/// <param name="mass">刚体质量</param>
	/// <param name="restitution">弹性系数（0-1，越大越“弹”）</param>
	/// <param name="friction">摩擦系数（0-1，越大摩擦越大）</param>
	void InitPhysicProperty(float radius, float mass, float restitution, float friction);
	/// <summary>
	/// 初始化矩形物理碰撞体和刚体。
	/// 以组件中心为原点创建一个轴对齐矩形形状。
	/// </summary>
	/// <param name="width">矩形宽度</param>
	/// <param name="height">矩形高度</param>
	/// <param name="mass">刚体质量</param>
	/// <param name="restitution">弹性系数（0-1，越大越“弹”）</param>
	/// <param name="friction">摩擦系数（0-1，越大摩擦越大）</param>
	void InitPhysicProperty(float width, float height, float mass, float restitution, float friction);
	/// <summary>
	/// 初始化多边形物理碰撞体和刚体。
	/// 顶点坐标为局部空间坐标，以组件中心为原点。
	/// </summary>
	/// <param name="count">多边形顶点数量</param>
	/// <param name="vertecies">顶点数组指针，长度为 count * 2，按 (x0, y0, x1, y1, ...) 排列</param>
	/// <param name="mass">刚体质量</param>
	/// <param name="restitution">弹性系数（0-1，越大越“弹”）</param>
	/// <param name="friction">摩擦系数（0-1，越大摩擦越大）</param>
	void InitPhysicProperty(int count, float* vertecies, float mass, float restitution, float friction);

	inline bool HasPhysicsShape() const { return shape != nullptr; }
	inline void SwitchToStatic()
	{
		if (!shape) return;
		cpBodySetType(cpShapeGetBody(shape), CP_BODY_TYPE_STATIC);
	}
	inline void SwitchToDynamic()
	{
		if (!shape) return;
		cpBodySetType(cpShapeGetBody(shape), CP_BODY_TYPE_DYNAMIC);
		float moment = 0;
		switch (shapeType)
		{
		case ShapeType::Circle:
			moment = static_cast<float>(cpMomentForCircle(physicMass, 0, physicSize1, cpvzero));
			break;
		case ShapeType::Box:
			moment = static_cast<float>(cpMomentForBox(physicMass, physicSize1, physicSize2[0]));
			break;
		case ShapeType::Polygon:
			moment = static_cast<float>(cpMomentForPoly(physicMass, static_cast<int>(physicSize1), (cpVect*)physicSize2, cpvzero, 0));
			break;
		}
		cpBodySetMass(cpShapeGetBody(shape), physicMass);
		cpBodySetMoment(cpShapeGetBody(shape), moment);
	}
	//inline void SwitchToKinematic() { cpBodySetType(cpShapeGetBody(shape), CP_BODY_TYPE_KINEMATIC); }

	inline void SetSensor(bool sensor)
	{
		hasPhysicBody = !sensor && shape != nullptr;
		if (!shape) return;
		cpShapeSetSensor(shape, cpBool(sensor));
	}
	inline void SetVelocity(glm::vec2 velocity) { if (!shape) return; cpBodySetVelocity(cpShapeGetBody(shape), cpv(velocity.x, velocity.y)); }
	inline void SetAngleVelocity(float angularVelocity) { if (!shape) return; cpBodySetAngularVelocity(cpShapeGetBody(shape), glm::radians(angularVelocity)); }
	inline void SetMass(float mass) { physicMass = mass; if (!shape) return; cpShapeSetMass(shape, mass); }
	inline void SetRestitution(float restitution) { if (!shape) return; cpShapeSetElasticity(shape, restitution); }
	inline void SetFriction(float friction) { if (!shape) return; cpShapeSetFriction(shape, friction); }

	inline void syncPosition() { transform.position = utils::ParseVec(cpBodyGetPosition(cpShapeGetBody(shape))); }

	inline std::uint64_t GetStableId() const { return stableId; }
	inline void SetStableId(std::uint64_t id)
	{
		stableId = id;
		if (id >= nextStableId)
			nextStableId = id + 1;
	}
	inline static std::uint64_t PeekNextStableId() { return nextStableId; }

	inline cpBody* GetBody() const { return shape ? cpShapeGetBody(shape) : nullptr; }
	inline float GetMass() const { return physicMass; }
	inline float GetMoment() const { return shape ? static_cast<float>(cpShapeGetMoment(shape)) : 0.0f; }
	inline float GetRestitution() const { return shape ? static_cast<float>(cpShapeGetElasticity(shape)) : 0.0f; }
	inline float GetFriction() const { return shape ? static_cast<float>(cpShapeGetFriction(shape)) : 0.0f; }
	inline glm::vec2 GetVelocity() const { return shape ? utils::ParseVec(cpBodyGetVelocity(cpShapeGetBody(shape))) : glm::vec2(0.0f, 0.0f); }
	inline glm::vec2 GetForce() const { return shape ? utils::ParseVec(cpBodyGetForce(cpShapeGetBody(shape))) : glm::vec2(0.0f, 0.0f); }
	inline float GetTorque() const { return shape ? static_cast<float>(cpBodyGetTorque(cpShapeGetBody(shape))) : 0.0f; }
	inline glm::vec2 GetAcceleration() const { return GetForce() / GetMass(); }
	inline float GetAngularAcceleration() const
	{
		const float moment = GetMoment();
		if (moment == 0.0f) return 0.0f;
		return glm::degrees(GetTorque() / moment);
	}
	inline glm::vec2 GetMomentum() const { return GetVelocity() * GetMass(); }
	inline float GetAngle() const { return shape ? glm::degrees(static_cast<float>(cpBodyGetAngle(cpShapeGetBody(shape)))) : 0.0f; }
	inline float GetAngleVelocity() const { return shape ? glm::degrees(static_cast<float>(cpBodyGetAngularVelocity(cpShapeGetBody(shape)))) : 0.0f; }
	inline float GetKineticEnergy() const { return shape ? static_cast<float>(cpBodyKineticEnergy(cpShapeGetBody(shape))) : 0.0f; }

	void DrawColliderOutline(
		void* drawList,
		float windowWidth,
		float windowHeight,
		float viewScale,
		float viewCenterX,
		float viewCenterY,
		uint32_t color) const;
	void DrawColliderOutlineRecursive(
		void* drawList,
		float windowWidth,
		float windowHeight,
		float viewScale,
		float viewCenterX,
		float viewCenterY,
		uint32_t color) const;

	bool checkPointInShape(float x, float y) const { return shape ? (cpShapePointQuery(shape, cpv(x, y), nullptr) <= 0) : false; }

private:
	void SyncPhysicsShapeBySize();
	void SyncPhysicsBodyFromRender();
	void SyncRenderFromPhysicsBody();
};