#include "Component.h"

void Component::AddChild(Component* child)
{
	child->parent = this;
	children.push_back(child);
}
void Component::GetPosition(glm::vec2& position)
{
	position = transform.position;
	if (parent == nullptr) return;
	position -= parent->transform.position;
}
void Component::SetPosition(glm::vec2 position)
{
	transform.position = position;
	if (parent == nullptr) return;
	transform.position *= parent->transform.scale;
	transform.position += parent->transform.position;
}
void Component::SetPosition(float x, float y)
{
	transform.position.x = x;
	transform.position.y = y;
	if (parent == nullptr) return;
	transform.position *= parent->transform.scale;
	transform.position += parent->transform.position;
}
void Component::GetWorldPosition(glm::vec2& position)
{
	position = transform.position;
}
void Component::SetWorldPosition(glm::vec2 position)
{
	transform.position = position;
}
void Component::SetWorldPosition(float x, float y)
{
	transform.position.x = x;
	transform.position.y = y;
}
void Component::GetScale(glm::vec2& scale)
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
void Component::GetWorldScale(glm::vec2& scale)
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
void Component::GetRotation(float& rotation)
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
}
void Component::GetWorldRotation(float& rotation)
{
	rotation = transform.rotation;
}
void Component::SetWorldRotation(float rotation)
{
	transform.rotation = rotation;
}
void Component::GetBackgroundColor(glm::vec4& color)
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
	mesh->uniform[meshIndex].positionTransform = {
		transform.scale.x * cosf(transform.rotation), -sinf(transform.rotation)						, 0.0f, 0.0f,
		sinf(transform.rotation)					, transform.scale.y * cosf(transform.rotation)	, 0.0f, 0.0f,
		transform.position.x						, transform.position.y							, 1.0f, 0.0f
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