#include "Component.h"

void Component::Draw(Renderer* renderer)
{
	if (mesh->changedCount) {
		mesh->changedCount = false;
		mesh->transformBuffer = new UniformBuffer(0, mesh->count * sizeof(UniformData));
		mesh->needUpdate = true;
	}
	if (mesh->needUpdate) {
		mesh->needUpdate = false;
		mesh->transformBuffer->SetData(mesh->transform.data(), mesh->count * sizeof(UniformData), 0);
	}
	if (!mesh->isDraw) {
		mesh->isDraw = true;
		mesh->transformBuffer->Bind();
		renderer->Draw(mesh->renderID, mesh->indices->size(), mesh->count);
	}
	for (Component* child : children)
	{
		child->Draw(renderer);
	}
}