#pragma once
#include <chipmunk.h>
#include <memory>
#include "Contrals.h"
class Models
{
private:
	// center[2], size, mass
	const std::shared_ptr<float[]> physicProperties;

	bool isPhysicObject;
	// position[2], rotation, scale
	float pose[4];

	Contrals* ctrl;

	cpBody* physicBody;
public:
	Models(Models& model, Contrals* ctrl);
	Models(Contrals* ctrl);
	~Models();
	void SetPose(float x, float y, float angle, float scale);
	void SetPhysicPropertiesOff();
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

