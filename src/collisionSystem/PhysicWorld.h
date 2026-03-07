#pragma once
#include <chipmunk.h>
#include <vector>

class PhysicWorld
{
public:
    PhysicWorld(cpFloat width, cpFloat height, cpFloat elasticity, cpFloat friction, cpFloat thickness);
    ~PhysicWorld();
	void SetGravity(cpVect gravity);
    cpFloat GetWidth() const { return width; }
    cpFloat GetHeight() const { return height; }

    // 步进物理世界
    void Step(double dt);

    void AddCircle(
        cpBody*& body, cpShape*& shape,
        cpFloat positionX, cpFloat positionY, cpFloat radius,
        cpFloat mass, cpFloat elasticity, cpFloat friction
    );
	void AddBox(
        cpBody*& body, cpShape*& shape,
		cpFloat positionX, cpFloat positionY, cpFloat width, cpFloat height,
        cpFloat mass, cpFloat elasticity, cpFloat friction
    );
	void AddPolygon(
        cpBody*& body, cpShape*& shape,
		cpFloat positionX, cpFloat positionY, int count, cpVect* vertices,
        cpFloat mass, cpFloat elasticity, cpFloat friction
    );
    void AddCord(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat distance);
    void AddSpring(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat restLength, cpFloat stiffness, cpFloat damping);

private:
    static PhysicWorld* instance;

    cpSpace* m_space; 
    cpBody* staticBody;
	cpFloat width;
	cpFloat height;
    cpFloat boundaryThickness;
    cpFloat boundaryElasticity;
    cpFloat boundaryFriction;

    cpShape* topBoundaryLine;
    cpShape* bottomBoundaryLine;
    cpShape* leftBoundaryLine;
    cpShape* rightBoundaryLine;
public:
    static void Initialize(
        PhysicWorld*& world, cpFloat width, cpFloat height,
        cpFloat elasticity = 1.0f, cpFloat friction = 1.0f, cpFloat thickness = 50.f);
    static void Free();
    static PhysicWorld* Instance() { return PhysicWorld::instance; }
};