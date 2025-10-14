#include "Models.h"

Models::Models(const Vertexs2D* vertexs, const uint32_t* indices, const VertexBufferLayout* layout, uint32_t vertexCount, uint32_t indexCount, const ShaderProgram* shader)
	: vertexs(vertexs), indices(indices), vertexCount(vertexCount), indexCount(indexCount), rotation(0.0f), shader(shader)
{
	objectSkeleton = new VertexBuffer(vertexs, vertexCount * sizeof(Vertexs2D));
	objectSkeletonShap = new VertexArray(*objectSkeleton, *layout);
	objectSkeletonIndecies = new IndexBuffer(indices, indexCount);
	objectPosition = new UniformBuffer(0, sizeof(TransForms2D));

	position = { glm::mat3(1.0f), glm::mat3(1.0f) };
}

Models::~Models()
{
	if (objectSkeleton)	delete objectSkeleton;
	if (objectSkeletonShap)	delete objectSkeletonShap;
	if (objectSkeletonIndecies)	delete objectSkeletonIndecies;
	if (objectPosition)	delete objectPosition;
}

void Models::MoveTo(float x, float y)
{
	position.matRT[2][0] = x;
	position.matRT[2][1] = y;
}

void Models::SetRotation(float angle)
{
	rotation = angle;
	float c = std::cos(rotation);
	float s = std::sin(rotation);
	position.matRT[0][0] = c;
	position.matRT[0][1] = -s;
	position.matRT[1][0] = s;
	position.matRT[1][1] = c;
}

void Models::SetScale(float scale)
{
	position.matScale[0][0] = scale;
	position.matScale[1][1] = scale;
}

void Models::Draw(Renderer* render) const
{
	objectPosition->SetData(&position, sizeof(TransForms2D), 0);
	render->Draw(*objectSkeletonShap, *objectSkeletonIndecies, *objectPosition, *shader);
}
