#include "Models.h"
#include "Macros.h"
#include "MyWindow.h"

Models::Models(Models& model, MyWindow* window)
	: window(window)
	, isPhysicObject(model.isPhysicObject)
	, count(1)
	, vertexs(model.vertexs), indices(model.indices)
	, physicProperties(model.physicProperties), poses(model.poses)
	, objectTransformBuffer(nullptr)
{
	window->AddModel(this);
}

Models::Models(MyWindow* window)
	: window(window), isPhysicObject(false)
	, count(1)
	, objectTransformBuffer(nullptr)
{
	window->AddModel(this);
}

Models::~Models()
{
	if (objectTransformBuffer)
	{
		delete objectTransformBuffer;
	}
}

void Models::InitializeModelData(std::vector<Vertexs2D>& vertexs, std::vector<uint32_t>& indices)
{
	this->vertexs = vertexs;
	this->indices = indices;
	renderIndex = window->GetRenderer()->AddData(this->vertexs.data(), (uint32_t)this->vertexs.size(), this->indices.data(), (uint32_t)this->indices.size());
}

void Models::SetModelCount(uint32_t count)
{
	if (objectTransformBuffer) return;
	this->count = count;
	this->physicProperties.resize(count);
	this->poses.resize(count);
	this->transformDatas.resize(count);
	this->physicBody.resize(count);
	this->physicShape.resize(count);
	objectTransformBuffer = new UniformBuffer(0, count * sizeof(TransForms2D));
}

void Models::InitializePhysicProperties(const std::vector<float>& physicProperties)
{
	for (uint32_t i = 0; i < count; i++)
	{
		this->physicProperties[i] = {
			physicProperties[0],
			physicProperties[1],
			physicProperties[2],
			physicProperties[3],
			physicProperties[4]
		};
	}
}

void Models::SetPhysicProperties(ModelsPhysicProperties& physicProperties, uint32_t modelIndex)
{
	if (modelIndex >= this->physicProperties.size()) return;
	this->physicProperties[modelIndex] = physicProperties;
	isPhysicObject = true;
}

void Models::InitializePhysicBody()
{
	if (!isPhysicObject) return;
	if (physicProperties[0].shape2 < 0.00001f)
		window->GetPhysicWorld()->AddCircle(this);
	else
		window->GetPhysicWorld()->AddBox(this);
}

void Models::Update()
{
	for (uint32_t i = 0; i < count; i++)
	{
		if (isPhysicObject) {
			cpVect pos = cpBodyGetPosition(physicBody[i]);
			cpFloat angle = cpBodyGetAngle(physicBody[i]);
			poses[i].x = pos.x;
			poses[i].y = pos.y;
			poses[i].angle = angle;
		}
		float scale = poses[i].scale;
		float cosAngle = cosf(poses[i].angle) * scale;
		float sinAngle = sinf(poses[i].angle) * scale;
		transformDatas[i].matSRT = {
			cosAngle,	sinAngle,	0.0f,	0.0f,
		   -sinAngle,	cosAngle,	0.0f,	0.0f,
		   poses[i].x,	poses[i].y,	1.0f,	0.0f
		};
	}
	objectTransformBuffer->SetData((const void*)transformDatas.data(), count * sizeof(TransForms2D), 0);
}

void Models::Updata(uint32_t modelIndex)
{
	objectTransformBuffer->SetData((const void*)&transformDatas[modelIndex], sizeof(TransForms2D), modelIndex * sizeof(TransForms2D));
}

void Models::Draw() const
{
	objectTransformBuffer->Bind();
	window->GetRenderer()->Draw(renderIndex, (uint32_t)indices.size(), count);
}
