#include "Models.h"
#include "Macros.h"

Models::Models(Models& model, Contrals* ctrl)
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

Models::Models(Contrals* ctrl)
	: ctrl(ctrl),
	isPhysicObject(false),
	physicProperties(std::make_shared<float[]>(4)),
	pose { 0.0f, 0.0f, 0.0f, 1.0f },
	physicBody(nullptr)
{
}

Models::~Models()
{
}

void Models::SetPose(float x, float y, float angle, float scale)
{
	pose[0] = x;
	pose[1] = y;
	pose[2] = angle;
	pose[3] = scale;
}

void Models::SetPhysicPropertiesOff()
{
	this->isPhysicObject = false;
}

void Models::SetPhysicProperties(const float* physicProperties)
{
	this->physicProperties[0] = physicProperties[0] * this->pose[3];
	this->physicProperties[1] = physicProperties[1] * this->pose[3];
	this->physicProperties[2] = physicProperties[2] * this->pose[3];
	this->physicProperties[3] = physicProperties[3];
	this->isPhysicObject = true;
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
