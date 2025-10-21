#include "Models.h"
#include "Macros.h"

Models::Models(Models& model, const VertexBufferLayout* layout)
	: vertexs(model.vertexs), indices(model.indices), 
	vertexCount(model.vertexCount), indexCount(model.indexCount), 
	shader(model.shader), 
	isPhysicObject(model.isPhysicObject), 
	physicProperties(model.physicProperties),
	pose { model.pose[0], model.pose[1], model.pose[2], model.pose[3] },
	physicBody(nullptr)
{
	objectSkeleton = new VertexBuffer(vertexs.get(), vertexCount * sizeof(Vertexs2D));
	objectSkeletonShap = new VertexArray(*objectSkeleton, *layout);
	objectSkeletonIndecies = new IndexBuffer(indices.get(), indexCount);
	objectPosition = new UniformBuffer(0, 6 * sizeof(glm::vec4));
	DEBUG_PRINT("copy model(v_id): " << objectSkeleton);
}

Models::Models(const Vertexs2D* vertexs, const uint32_t* indices, 
	const VertexBufferLayout* layout, uint32_t vertexCount, uint32_t indexCount, const ShaderProgram* shader)
	: vertexs(new Vertexs2D[vertexCount]), indices(new uint32_t[indexCount]),
	vertexCount(vertexCount), indexCount(indexCount), 
	shader(shader), 
	isPhysicObject(false), physicProperties(new float[4]),
	pose { 0.0f, 0.0f, 0.0f, 1.0f },
	objectSkeleton(nullptr),
	objectSkeletonShap(nullptr),
	objectSkeletonIndecies(nullptr),
	objectPosition(nullptr),
	physicBody(nullptr)
{
	std::copy(vertexs, vertexs + vertexCount, this->vertexs.get());
	std::copy(indices, indices + indexCount, this->indices.get());
}

Models::~Models()
{
	if (objectSkeleton)	delete objectSkeleton;
	if (objectSkeletonShap)	delete objectSkeletonShap;
	if (objectSkeletonIndecies)	delete objectSkeletonIndecies;
	if (objectPosition)	delete objectPosition;
	DEBUG_PRINT("destruction model(v_id): " << objectSkeleton);
}

void Models::SetPose(float x, float y, float angle, float scale)
{
	pose[0] = x;
	pose[1] = y;
	pose[2] = angle;
	pose[3] = scale;
}

void Models::SetPhysicProperties(bool isPhysicObject)
{
	this->isPhysicObject = isPhysicObject;
}

void Models::SetPhysicProperties(const float* physicProperties)
{
	this->isPhysicObject = true;
	this->physicProperties[0] = physicProperties[0] * this->pose[3];
	this->physicProperties[1] = physicProperties[1] * this->pose[3];
	this->physicProperties[2] = physicProperties[2] * this->pose[3];
	this->physicProperties[3] = physicProperties[3];
}

void Models::MoveTo(float x, float y)
{
	pose[0] = x; pose[1] = y;
}

void Models::SetRotation(float angle)
{
	pose[2] = angle;
}

void Models::SetScale(float scale)
{
	pose[3] = scale;
}

void Models::Draw(Renderer* render) const
{
	TransForms2D tf;
	GetTransForms2D(tf);
	objectPosition->SetData(&tf, sizeof(TransForms2D), 0);
	render->Draw(*objectSkeletonShap, *objectSkeletonIndecies, *objectPosition, *shader);
}

void Models::GetTransForms2D(TransForms2D& tf) const
{
	tf.matScale = glm::mat3x4(
		pose[3], 0.0f, 0.0f, 0.0f,
		0.0f, pose[3], 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f
	);
	float cosA = cosf(pose[2]);
	float sinA = sinf(pose[2]);
	tf.matRT = glm::mat3x4(
		cosA, sinA, 0.0f, 0.0f,
		-sinA, cosA, 0.0f, 0.0f,
		pose[0], pose[1], 1.0f, 0.0f
	);
}

void Models::UpdatePhysicFromModel()
{
	if (isPhysicObject)
	{
		cpVect pos = cpBodyGetPosition(physicBody);
		pose[0] = pos.x - physicProperties[0];
		pose[1] = pos.y - physicProperties[1];
		pose[2] = cpBodyGetAngle(physicBody);
	}
}
