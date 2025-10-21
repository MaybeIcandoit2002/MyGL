#pragma once
#include <chipmunk.h>
#include "../Models.h"

// 物理世界封装类
class PhysicWorld
{
public:
    PhysicWorld();
    ~PhysicWorld();

	void SetGravity(cpVect gravity);
	void SetBoundaryLine(cpVect v1, cpVect v2);

    // 步进物理世界
    void Step(double dt);

    // 添加一个圆形物体
    void AddCircle(Models& model);

    // 可选：添加其他形状
    // cpBody* AddBox(float x, float y, float width, float height, float mass);

private:
    cpSpace* m_space; 
    cpBody* staticBody;
};

