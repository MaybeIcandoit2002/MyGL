#include "PhysicWorld.h"

PhysicWorld* PhysicWorld::instance = nullptr;

PhysicWorld::PhysicWorld(cpFloat width, cpFloat height, cpFloat elasticity, cpFloat friction, cpFloat thickness)
	: width(width), height(height), boundaryElasticity(elasticity), boundaryFriction(friction), boundaryThickness(thickness)
{
    m_space = cpSpaceNew();
    staticBody = cpSpaceGetStaticBody(m_space);
    
    topBoundaryLine = cpSegmentShapeNew(staticBody, cpv(0, height), cpv(width, height), thickness);
	bottomBoundaryLine = cpSegmentShapeNew(staticBody, cpv(0, 0), cpv(width, 0), thickness);
	leftBoundaryLine = cpSegmentShapeNew(staticBody, cpv(0, 0), cpv(0, height), thickness);
    rightBoundaryLine = cpSegmentShapeNew(staticBody, cpv(width, 0), cpv(width, height), thickness);
	cpShapeSetElasticity(topBoundaryLine, elasticity);
    cpShapeSetElasticity(bottomBoundaryLine, elasticity);
    cpShapeSetElasticity(leftBoundaryLine, elasticity);
    cpShapeSetElasticity(rightBoundaryLine, elasticity);
	cpShapeSetFriction(topBoundaryLine, friction);
    cpShapeSetFriction(bottomBoundaryLine, friction);
    cpShapeSetFriction(leftBoundaryLine, friction);
    cpShapeSetFriction(rightBoundaryLine, friction);
    cpSpaceAddShape(m_space, topBoundaryLine);
    cpSpaceAddShape(m_space, bottomBoundaryLine);
    cpSpaceAddShape(m_space, leftBoundaryLine);
	cpSpaceAddShape(m_space, rightBoundaryLine);
    
    cpSpaceSetGravity(m_space, cpv(0, 9.8));
}

PhysicWorld::~PhysicWorld()
{
	cpShapeFree(topBoundaryLine);
	cpShapeFree(bottomBoundaryLine);
	cpShapeFree(leftBoundaryLine);
	cpShapeFree(rightBoundaryLine);
    cpSpaceFree(m_space);
}

void PhysicWorld::SetGravity(cpVect gravity)
{
	cpSpaceSetGravity(m_space, gravity);
}
void PhysicWorld::Step(double dt)
{
    cpSpaceStep(m_space, dt);
}

void PhysicWorld::AddCircle(
    cpBody*& body, cpShape*& shape,
	cpFloat positionX, cpFloat positionY, cpFloat radius,
    cpFloat mass, cpFloat elasticity, cpFloat friction
) {
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
    body = cpSpaceAddBody(m_space, (mass <= 0) ? cpBodyNewStatic() : cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(positionX, positionY));
    shape = cpSpaceAddShape(m_space, cpCircleShapeNew(body, radius, cpvzero));
    cpShapeSetFriction(shape, friction);
    cpShapeSetElasticity(shape, elasticity);
}

void PhysicWorld::AddBox(
    cpBody*& body, cpShape*& shape,
    cpFloat positionX, cpFloat positionY, cpFloat width, cpFloat height,
    cpFloat mass, cpFloat elasticity, cpFloat friction
)  {
    cpFloat moment = cpMomentForBox(mass, width, height);
    
	body = cpSpaceAddBody(m_space, (mass <= 0) ? cpBodyNewStatic() : cpBodyNew(mass, moment));
	cpBodySetPosition(body, cpv(positionX, positionY));
	shape = cpSpaceAddShape(m_space, cpBoxShapeNew(body, width, height, 0));
	cpShapeSetFriction(shape, friction);
	cpShapeSetElasticity(shape, elasticity);
}

void PhysicWorld::AddPolygon(
    cpBody*& body, cpShape*& shape,
    cpFloat positionX, cpFloat positionY, int count, cpVect* vertices,
    cpFloat mass, cpFloat elasticity, cpFloat friction
) {
    cpFloat moment = cpMomentForPoly(mass, count, vertices, cpvzero,0);
    body = cpSpaceAddBody(m_space, (mass <= 0) ? cpBodyNewStatic() : cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(positionX, positionY));
    shape = cpSpaceAddShape(m_space, cpPolyShapeNew(body, count, vertices, cpTransformIdentity,0));
    cpShapeSetFriction(shape, friction);
    cpShapeSetElasticity(shape, elasticity);
}

void PhysicWorld::AddCord(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat distance)
{
    cpConstraint* constraint = cpSlideJointNew(bodyA, bodyB, anchorA, anchorB, 0.0f, distance);
    cpSpaceAddConstraint(m_space, constraint);
}

void PhysicWorld::AddSpring(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat restLength, cpFloat stiffness, cpFloat damping)
{
	cpConstraint* constraint = cpDampedSpringNew(
		bodyA,  bodyB,
		anchorA,anchorB,
		restLength, stiffness, damping
	);
	cpSpaceAddConstraint(m_space, constraint);
}

void PhysicWorld::Initialize(PhysicWorld*& world, cpFloat width, cpFloat height, cpFloat elasticity, cpFloat friction, cpFloat thickness)
{
    if (instance) return;
    world = instance = new PhysicWorld(width, height, elasticity, friction, thickness);
}

void PhysicWorld::Free()
{
    delete instance;
    instance = nullptr;
}