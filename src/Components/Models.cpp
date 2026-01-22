#include "Models.h"

void Models::InitializeModelData(std::vector<Vertex2D>& vertexs, std::vector<uint32_t>& indices)
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
	objectTransformBuffer = new UniformBuffer(0, count * sizeof(UniformData));
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
		transformDatas[i].positionTransform = {
			cosAngle,	sinAngle,	0.0f,	0.0f,
		   -sinAngle,	cosAngle,	0.0f,	0.0f,
		   poses[i].x,	poses[i].y,	1.0f,	0.0f
		};
	}
	objectTransformBuffer->SetData((const void*)transformDatas.data(), count * sizeof(UniformData), 0);
}

void Models::Updata(uint32_t modelIndex)
{
	objectTransformBuffer->SetData((const void*)&transformDatas[modelIndex], sizeof(UniformData), modelIndex * sizeof(UniformData));
}

void Models::Draw() const
{
	objectTransformBuffer->Bind();
	window->GetRenderer()->Draw(renderIndex, (uint32_t)indices.size(), count);
}
