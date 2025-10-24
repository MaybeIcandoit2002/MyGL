#pragma once
#include <chipmunk.h>
#include <memory>
#include <vector>
#include "renderComponents/Renderer.h"
class MyWindow;
struct ModelsPose {
	float x;
	float y;
	float angle;
	float scale;
};

struct ModelsPhysicProperties {
	float mass;
	float shape1;
	float shape2;
	float friction;
	float elasticity;
};

class Models
{
private:
	MyWindow* window;
	bool isPhysicObject;
	uint32_t count;
	uint32_t renderIndex;

	std::vector<Vertexs2D> vertexs;
	std::vector<uint32_t> indices;

	std::vector<ModelsPhysicProperties> physicProperties;
	std::vector<ModelsPose> poses;
	std::vector<TransForms2D> transformDatas;

	std::vector<cpBody*> physicBody;
	std::vector<cpShape*> physicShape;
	UniformBuffer* objectTransformBuffer;
public:
	Models(Models& model, MyWindow* window);
	Models(MyWindow* window);
	~Models();

	void InitializeModelData(std::vector<Vertexs2D>& vertexs, std::vector<uint32_t>& indices);
	void SetModelCount(uint32_t count);

	void InitializePhysicProperties(const std::vector<float>& physicProperties);

	void SetPhysicProperties(ModelsPhysicProperties& physicProperties, uint32_t modelIndex = 0);
	void SetPhysicOn() { isPhysicObject = true; }
	void SetPhysicOff() { isPhysicObject = false; }

	void SetPose(float x, float y, float angle, float scale, uint32_t modelIndex = 0) { if (modelIndex >= poses.size()) return; poses[modelIndex] = { x, y, angle, scale }; }
	void SetPose(float x, float y, uint32_t modelIndex = 0) { if (modelIndex >= poses.size()) return; poses[modelIndex].x = x; poses[modelIndex].y = y; }
	void SetAngle(float angle, uint32_t modelIndex = 0) { if (modelIndex >= poses.size()) return; poses[modelIndex].angle = angle; }
	void SetScale(float scale, uint32_t modelIndex = 0) { if (modelIndex >= poses.size()) return; poses[modelIndex].scale = scale; }

	void InitializePhysicBody();

	void Update();
	void Updata(uint32_t modelIndex);
	void Draw() const;

	uint32_t GetModelCount() const { return count; }
	MyWindow* GetWindow() { return window; }
	std::vector<ModelsPose>& GetPoses() { return poses; }
	std::vector<ModelsPhysicProperties>& GetPhysicProperties() { return physicProperties; }
	std::vector<cpBody*>& GetPhysicBody() { return physicBody; }
	std::vector<cpShape*>& GetPhysicShape() { return physicShape; }
};

