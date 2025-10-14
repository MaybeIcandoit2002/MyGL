#pragma once
#include "renderComponents/Renderer.h"
class Models
{
private:
	const Vertexs2D* vertexs;
	const uint32_t* indices;
	uint32_t vertexCount;
	uint32_t indexCount;

	TransForms2D position;
	float rotation = 1.0f;

	VertexBuffer* objectSkeleton;
	VertexArray* objectSkeletonShap;
	IndexBuffer* objectSkeletonIndecies;
	UniformBuffer* objectPosition;

	const ShaderProgram* shader;
public:
	Models(const Vertexs2D* vertexs, const uint32_t* indices, const VertexBufferLayout* layout, uint32_t vertexCount, uint32_t indexCount, const ShaderProgram* shader);
	~Models();
	void MoveTo(float x, float y);
	void SetRotation(float angle);
	void SetScale(float scale);
	void Draw(Renderer* render) const;
	TransForms2D GetPosition() { return position; }
};

