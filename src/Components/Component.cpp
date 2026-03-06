#include "Component.h"

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
	if (!parent) return;
	transform.position *= parent->transform.scale;
	transform.position += parent->transform.position;
	if (hasPhysicBody)
	cpBodySetPosition(cpShapeGetBody(shape), cpv(transform.position.x, transform.position.y));
}
void Component::SetPosition(float x, float y)
{
	transform.position.x = x;
	transform.position.y = y;
	if (!parent) return;
	transform.position *= parent->transform.scale;
	transform.position += parent->transform.position;
	if (hasPhysicBody)
	cpBodySetPosition(cpShapeGetBody(shape), cpv(transform.position.x, transform.position.y));
}
void Component::GetWorldPosition(glm::vec2& position) const
{
	position = transform.position;
}
void Component::SetWorldPosition(glm::vec2 position)
{
	transform.position = position;
	if (hasPhysicBody)
	cpBodySetPosition(cpShapeGetBody(shape), cpv(transform.position.x, transform.position.y));
}
void Component::SetWorldPosition(float x, float y)
{
	transform.position.x = x;
	transform.position.y = y;
	if (hasPhysicBody)
	cpBodySetPosition(cpShapeGetBody(shape), cpv(transform.position.x, transform.position.y));
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
	if (parent == nullptr) return;
	transform.scale.x *= parent->transform.scale.x;
	transform.scale.y *= parent->transform.scale.y;
}
void Component::SetScale(float x, float y)
{
	transform.scale.x = x;
	transform.scale.y = y;
	if (parent == nullptr) return;
	transform.scale.x *= parent->transform.scale.x;
	transform.scale.y *= parent->transform.scale.y;
}
void Component::GetWorldScale(glm::vec2& scale) const
{
	scale = transform.scale;
}
void Component::SetWorldScale(glm::vec2 scale)
{
	transform.scale = scale;
}
void Component::SetWorldScale(float x, float y)
{
	transform.scale.x = x;
	transform.scale.y = y;
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
	if (parent == nullptr) return;
	transform.rotation += parent->transform.rotation;
	if (hasPhysicBody)
	cpBodySetAngle(cpShapeGetBody(shape), transform.rotation);
}
void Component::GetWorldRotation(float& rotation) const
{
	rotation = transform.rotation;
}
void Component::SetWorldRotation(float rotation)
{
	transform.rotation = rotation;
	if (hasPhysicBody) 
	cpBodySetAngle(cpShapeGetBody(shape), transform.rotation);
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
void Component::BeforeUpdate(Renderer* renderer)
{
	if (hasPhysicBody)
	{
		syncPosition();
		transform.rotation = GetAngle();
		if (transform.rotation > 180) SetWorldRotation(transform.rotation - 360);
		else if (transform.rotation < -180) SetWorldRotation(transform.rotation + 360);
		
	}
	float rotation = glm::radians(transform.rotation);
	mesh->uniform[meshIndex].positionTransform = {
		transform.scale.x * cosf(rotation)	, transform.scale.x * sinf(rotation)	, 0.0f, 0.0f,
		transform.scale.y * -sinf(rotation)	, transform.scale.y * cosf(rotation)	, 0.0f, 0.0f,
		transform.position.x				, transform.position.y					, 1.0f, 0.0f
	};
	mesh->uniform[meshIndex].color = backgroundColor;
	for (Component* child : children)
	{
		child->BeforeUpdate(renderer);
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
}

void Component::InitPhysicProperty(float width, float height, float mass, float restitution, float friction)
{
	cpBody* body;
	PhysicWorld::Instance()->AddBox(
		body, shape,
		transform.position.x, transform.position.y,
		width, height, mass, restitution, friction
	);
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
}