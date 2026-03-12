#include "Component.h"
#include "Component.h"

#include <cmath>

#include "../vendor/imgui/imgui.h"

std::uint64_t Component::nextStableId = 1;

namespace
{
	glm::vec2 RotateByDegree(const glm::vec2& v, float degree)
	{
		const float rad = glm::radians(degree);
		const float c = cosf(rad);
		const float s = sinf(rad);
		return glm::vec2(v.x * c - v.y * s, v.x * s + v.y * c);
	}
}

void Component::AddChild(Component* child)
{
	if (child->parent) {
		child->parent->RemoveChild(child);
	}
	child->parent = this;
	children.push_back(child);
}
void Component::RemoveChild(Component* child)
{
	children.erase(std::find(children.begin(), children.end(), child));
	child->parent = nullptr;
}
void Component::GetPosition(glm::vec2& position) const
{
	position = transform.position;
	if (!parent) return;
	position -= parent->transform.position;
}
void Component::SetPosition(glm::vec2 position)
{
	transform.position = position;
	if (parent)
	{
		transform.position *= parent->transform.scale;
		transform.position += parent->transform.position;
	}
	if (shape)
		SyncPhysicsBodyFromRender();
}
void Component::SetPosition(float x, float y)
{
	transform.position.x = x;
	transform.position.y = y;
	if (parent)
	{
		transform.position *= parent->transform.scale;
		transform.position += parent->transform.position;
	}
	if (shape)
		SyncPhysicsBodyFromRender();
}
void Component::GetWorldPosition(glm::vec2& position) const
{
	position = transform.position;
}
void Component::SetWorldPosition(glm::vec2 position)
{
	transform.position = position;
	if (shape)
		SyncPhysicsBodyFromRender();
}
void Component::SetWorldPosition(float x, float y)
{
	transform.position.x = x;
	transform.position.y = y;
	if (shape)
		SyncPhysicsBodyFromRender();
}
void Component::GetScale(glm::vec2& scale) const
{
	scale = transform.scale;
	if (parent == nullptr) return;
	if (parent->transform.scale.x == 0)
	scale.x /= parent->transform.scale.x;
	scale.y /= parent->transform.scale.y;
}
void Component::SetScale(glm::vec2 scale)
{
	transform.scale = scale;
	if (parent != nullptr)
	{
		transform.scale.x *= parent->transform.scale.x;
		transform.scale.y *= parent->transform.scale.y;
	}
	if (shape)
		SyncPhysicsShapeBySize();
}
void Component::SetScale(float x, float y)
{
	transform.scale.x = x;
	transform.scale.y = y;
	if (parent != nullptr)
	{
		transform.scale.x *= parent->transform.scale.x;
		transform.scale.y *= parent->transform.scale.y;
	}
	if (shape)
		SyncPhysicsShapeBySize();
}
void Component::GetWorldScale(glm::vec2& scale) const
{
	scale = transform.scale;
}
void Component::SetWorldScale(glm::vec2 scale)
{
	transform.scale = scale;
	if (shape)
		SyncPhysicsShapeBySize();
}
void Component::SetWorldScale(float x, float y)
{
	transform.scale.x = x;
	transform.scale.y = y;
	if (shape)
		SyncPhysicsShapeBySize();
}
void Component::GetRotation(float& rotation) const
{
	rotation = transform.rotation;
	if (parent == nullptr) return;
	rotation -= parent->transform.rotation;
}
void Component::SetRotation(float rotation)
{
	transform.rotation = rotation;
	if (parent != nullptr)
		transform.rotation += parent->transform.rotation;
	if (shape)
		SyncPhysicsBodyFromRender();
}
void Component::GetWorldRotation(float& rotation) const
{
	rotation = transform.rotation;
}
void Component::SetWorldRotation(float rotation)
{
	transform.rotation = rotation;
	if (shape) 
		SyncPhysicsBodyFromRender();
}
void Component::GetBackgroundColor(glm::vec4& color) const
{
	color = backgroundColor;
}
void Component::SetBackgroundColor(glm::vec4 color)
{
	backgroundColor = color;
}
void Component::SetBackgroundColor(float r, float g, float b, float a)
{
	backgroundColor.r = r;
	backgroundColor.g = g;
	backgroundColor.b = b;
	backgroundColor.a = a;
}
void Component::SetPhysicRelativePosition(glm::vec2 position)
{
	physicRelativePosition = position;
	if (shape)
		SyncPhysicsBodyFromRender();
}

void Component::SetPhysicRelativePosition(float x, float y)
{
	SetPhysicRelativePosition(glm::vec2(x, y));
}

void Component::SetPhysicRelativeScale(glm::vec2 scale)
{
	physicRelativeScale = scale;
	if (shape)
		SyncPhysicsShapeBySize();
}

void Component::SetPhysicRelativeScale(float x, float y)
{
	SetPhysicRelativeScale(glm::vec2(x, y));
}

void Component::SetPhysicSize(float size1, float size2)
{
	physicSize1 = size1;
	if (shapeType == ShapeType::Box)
	{
		if (!physicSize2) physicSize2 = new float(size2);
		else physicSize2[0] = size2;
	}
	if (shape)
		SyncPhysicsShapeBySize();
}

void Component::GetPhysicRelativePosition(glm::vec2& position) const
{
	position = physicRelativePosition;
}

void Component::GetPhysicRelativeScale(glm::vec2& scale) const
{
	scale = physicRelativeScale;
}

void Component::BeforeUpdate(Renderer* renderer, bool physicRunning)
{
	if (shape)
	{
		if (physicRunning && hasPhysicBody)
			SyncRenderFromPhysicsBody();
		else
			SyncPhysicsBodyFromRender();
	}
	float rotation = glm::radians(transform.rotation);
	mesh->uniform[meshIndex].positionTransform = {
		transform.scale.x * cosf(rotation)	, transform.scale.x * sinf(rotation)	, 0.0f, 0.0f,
		transform.scale.y * -sinf(rotation)	, transform.scale.y * cosf(rotation)	, 0.0f, 0.0f,
		transform.position.x				, transform.position.y					, 1.0f, 0.0f
	};
	mesh->uniform[meshIndex].color = backgroundColor;
	mesh->uniform[meshIndex].texIndex = glm::vec4((float)textureSlot, 0.0f, 0.0f, 0.0f);
	for (Component* child : children)
	{
		child->BeforeUpdate(renderer, physicRunning);
	}
}
void Component::Update(Renderer* renderer)
{
	if (mesh->changedCount) {
		mesh->changedCount = false;
		mesh->transformBuffer = new UniformBuffer(0, mesh->count * sizeof(UniformData));
	}
	mesh->transformBuffer->SetData(mesh->uniform.data(), mesh->count * sizeof(UniformData), 0);
	if (!mesh->isDraw) {
		mesh->isDraw = true;
		mesh->transformBuffer->Bind();
		renderer->Draw(mesh->renderID, mesh->indices->size(), mesh->count);
	}
	for (Component* child : children)
	{
		child->Update(renderer);
	}
}

void Component::InitPhysicProperty(float radius, float mass, float restitution, float friction)
{
	cpBody* body;
	PhysicWorld::Instance()->AddCircle(
		body, shape,
		transform.position.x, transform.position.y,
		radius, mass, restitution, friction
	);
	shapeType = ShapeType::Circle;
	physicMass = abs(mass);
	physicSize1 = radius;
	SyncPhysicsShapeBySize();
	SyncPhysicsBodyFromRender();
}

void Component::SyncPhysicsShapeBySize()
{
	if (!shape) return;
	cpBody* body = cpShapeGetBody(shape);
	cpSpace* space = cpBodyGetSpace(body);
	const cpFloat friction = cpShapeGetFriction(shape);
	const cpFloat elasticity = cpShapeGetElasticity(shape);
	const cpBool sensor = cpShapeGetSensor(shape);
	cpShape* oldShape = shape;
	cpShape* newShape = nullptr;

	const cpFloat scaleX = static_cast<cpFloat>(std::fabs(transform.scale.x * physicRelativeScale.x));
	const cpFloat scaleY = static_cast<cpFloat>(std::fabs(transform.scale.y * physicRelativeScale.y));

	switch (shapeType)
	{
	case ShapeType::Circle:
	{
		const cpFloat radius = static_cast<cpFloat>(physicSize1) * ((scaleX + scaleY) * 0.5f);
		newShape = cpCircleShapeNew(body, radius, cpvzero);
		if (cpBodyGetType(body) == CP_BODY_TYPE_DYNAMIC)
			cpBodySetMoment(body, cpMomentForCircle(physicMass, 0, radius, cpvzero));
		break;
	}
	case ShapeType::Box:
	{
		const cpFloat width = static_cast<cpFloat>(physicSize1) * scaleX;
		const cpFloat height = static_cast<cpFloat>(physicSize2 ? physicSize2[0] : physicSize1) * scaleY;
		newShape = cpBoxShapeNew(body, width, height, 0);
		if (cpBodyGetType(body) == CP_BODY_TYPE_DYNAMIC)
			cpBodySetMoment(body, cpMomentForBox(physicMass, width, height));
		break;
	}
	case ShapeType::Polygon:
	{
		if (!physicVertices.empty())
		{
			std::vector<cpVect> scaledVerts;
			scaledVerts.reserve(physicVertices.size());
			for (const cpVect& v : physicVertices)
				scaledVerts.emplace_back(cpv(v.x * scaleX, v.y * scaleY));

			newShape = cpPolyShapeNew(body, static_cast<int>(scaledVerts.size()), scaledVerts.data(), cpTransformIdentity, 0);
			if (cpBodyGetType(body) == CP_BODY_TYPE_DYNAMIC)
				cpBodySetMoment(body, cpMomentForPoly(physicMass, static_cast<int>(scaledVerts.size()), scaledVerts.data(), cpvzero, 0));
		}
		break;
	}
	}

	if (!newShape) return;
	if (space)
		cpSpaceRemoveShape(space, oldShape);
	cpShapeSetFriction(newShape, friction);
	cpShapeSetElasticity(newShape, elasticity);
	cpShapeSetSensor(newShape, sensor);
	if (space)
		cpSpaceAddShape(space, newShape);
	cpShapeFree(oldShape);
	shape = newShape;
}

void Component::SyncPhysicsBodyFromRender()
{
	if (!shape) return;
	cpBody* body = cpShapeGetBody(shape);
	cpSpace* space = cpBodyGetSpace(body);
	const cpBool isStaticBody = cpBodyGetType(body) == CP_BODY_TYPE_STATIC;
	if (!isStaticBody)
		cpBodyActivate(body);
	if (isStaticBody && space)
		cpSpaceRemoveShape(space, shape);
	const glm::vec2 scaledRelative(
		physicRelativePosition.x * transform.scale.x,
		physicRelativePosition.y * transform.scale.y
	);
	const glm::vec2 rotatedRelative = RotateByDegree(scaledRelative, transform.rotation);
	const glm::vec2 worldPos = transform.position + rotatedRelative;
	cpBodySetPosition(body, cpv(worldPos.x, worldPos.y));
	cpBodySetAngle(body, glm::radians(transform.rotation));
	if (isStaticBody && space)
		cpSpaceAddShape(space, shape);
	if (space)
		cpSpaceReindexShapesForBody(space, body);
	if (PhysicWorld::Instance())
		PhysicWorld::Instance()->SyncFixedLengthJointsByBody(body);
}

void Component::SyncRenderFromPhysicsBody()
{
	if (!shape) return;
	cpBody* body = cpShapeGetBody(shape);
	const cpVect bodyPos = cpBodyGetPosition(body);
	transform.rotation = glm::degrees(static_cast<float>(cpBodyGetAngle(body)));

	const glm::vec2 scaledRelative(
		physicRelativePosition.x * transform.scale.x,
		physicRelativePosition.y * transform.scale.y
	);
	const glm::vec2 rotatedRelative = RotateByDegree(scaledRelative, transform.rotation);
	transform.position = glm::vec2(static_cast<float>(bodyPos.x), static_cast<float>(bodyPos.y)) - rotatedRelative;
}

void Component::DrawColliderOutline(void* drawListRaw, float windowWidth, float windowHeight, float viewScale, float viewCenterX, float viewCenterY, uint32_t color) const
{
	if (!hasPhysicBody || !shape || !drawListRaw) return;
	ImDrawList* drawList = static_cast<ImDrawList*>(drawListRaw);
	cpBody* body = cpShapeGetBody(shape);
	auto ToScreen = [windowWidth, windowHeight, viewScale, viewCenterX, viewCenterY](const cpVect& p) {
		return ImVec2(
			windowWidth * 0.5f + (static_cast<float>(p.x) - viewCenterX) * viewScale,
			windowHeight * 0.5f - (static_cast<float>(p.y) - viewCenterY) * viewScale
		);
	};

	if (shapeType == ShapeType::Circle)
	{
		const cpVect centerWorld = cpBodyLocalToWorld(body, cpCircleShapeGetOffset(shape));
		const float radius = static_cast<float>(cpCircleShapeGetRadius(shape)) * viewScale;
		drawList->AddCircle(ToScreen(centerWorld), radius, color, 32, 1.2f);
		return;
	}

	const int count = cpPolyShapeGetCount(shape);
	if (count <= 1) return;

	for (int i = 0; i < count; ++i)
	{
		const cpVect v0Local = cpPolyShapeGetVert(shape, i);
		const cpVect v1Local = cpPolyShapeGetVert(shape, (i + 1) % count);
		const cpVect v0 = cpBodyLocalToWorld(body, v0Local);
		const cpVect v1 = cpBodyLocalToWorld(body, v1Local);
		drawList->AddLine(
			ToScreen(v0),
			ToScreen(v1),
			color,
			1.2f
		);
	}
}

void Component::DrawColliderOutlineRecursive(void* drawList, float windowWidth, float windowHeight, float viewScale, float viewCenterX, float viewCenterY, uint32_t color) const
{
	DrawColliderOutline(drawList, windowWidth, windowHeight, viewScale, viewCenterX, viewCenterY, color);
	for (const Component* child : children)
	{
		if (child)
			child->DrawColliderOutlineRecursive(drawList, windowWidth, windowHeight, viewScale, viewCenterX, viewCenterY, color);
	}
}

void Component::InitPhysicProperty(float width, float height, float mass, float restitution, float friction)
{
	cpBody* body;
	PhysicWorld::Instance()->AddBox(
		body, shape,
		transform.position.x, transform.position.y,
		width, height, mass, restitution, friction
	);
	shapeType = ShapeType::Box;
	physicMass = abs(mass);
	physicSize1 = width;
	if (physicSize2)
	{
		delete physicSize2;
	}
	physicSize2 = new float(height);
	SyncPhysicsShapeBySize();
	SyncPhysicsBodyFromRender();
}

void Component::InitPhysicProperty(int count, float* vertecies, float mass, float restitution, float friction)
{
	cpBody* body;
	std::vector<cpVect> vects = {};
	for (int i = 0; i < count; i++)
	{
		vects.emplace_back(cpv(vertecies[2 * i], vertecies[2 * i + 1]));
	}
	PhysicWorld::Instance()->AddPolygon(
		body, shape,
		transform.position.x, transform.position.y,
		count, vects.data(), mass, restitution, friction
	);
	shapeType = ShapeType::Polygon;
	physicMass = abs(mass);
	physicSize1 = static_cast<float>(count);
	physicVertices = vects;
	{
		delete physicSize2;
	}
	physicSize2 = vertecies;
	SyncPhysicsShapeBySize();
	SyncPhysicsBodyFromRender();
}