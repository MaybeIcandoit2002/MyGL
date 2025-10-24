#pragma once
#include <chipmunk.h>
#include <vector>
class Models;

class PhysicWorld
{
public:
    PhysicWorld();
    ~PhysicWorld();

	void SetGravity(cpVect gravity);
	void SetBoundaryLine(cpVect v1, cpVect v2);

    // 步进物理世界
    void Step(double dt);

    void AddCircle(Models* model);

	void AddBox(Models* model);

private:
    cpSpace* m_space; 
    cpBody* staticBody;
};

