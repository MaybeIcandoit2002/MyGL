#pragma once
#include "renderComponents/Renderer.h"
#include <chipmunk.h>
#include <memory>
class Models
{
private:
	// Vertexs Data
	//position[2], color[4], texCoord[2], texID
	const std::shared_ptr<Vertexs2D[]> vertexs;
	const std::shared_ptr<uint32_t[]> indices;
	const uint32_t vertexCount;
	const uint32_t indexCount;

	// center[2], size, mass
	const std::shared_ptr<float[]> physicProperties;

	bool isPhysicObject;
	// position[2], rotation, scale
	float pose[4];

	VertexBuffer* objectSkeleton;
	VertexArray* objectSkeletonShap;
	IndexBuffer* objectSkeletonIndecies;
	UniformBuffer* objectPosition;

	cpBody* physicBody;

	const ShaderProgram* shader;
public:
	Models(Models& model, const VertexBufferLayout* layout);
	Models(const Vertexs2D* vertexs, const uint32_t* indices, const VertexBufferLayout* layout, uint32_t vertexCount, uint32_t indexCount, const ShaderProgram* shader);
	~Models();
	void SetPose(float x, float y, float angle, float scale);
	void SetPhysicProperties(bool isPhysicObject);
	void SetPhysicProperties(const float* physicProperties);

	void MoveTo(float x, float y);
	void SetRotation(float angle);
	void SetScale(float scale);

	void SetPhysicBody(cpBody* body) { physicBody = body; }

	void Draw(Renderer* render) const;
	void GetTransForms2D(TransForms2D& tf) const;
	void UpdatePhysicFromModel();

	cpBody* GetPhysicBody() const { return physicBody; }
	float* GetPhysicProperties() const { return physicProperties.get(); }
	float* GetPose() { return pose; }
};

