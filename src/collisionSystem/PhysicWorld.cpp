#include "PhysicWorld.h"

#include <cmath>

PhysicWorld* PhysicWorld::instance = nullptr;

PhysicWorld::PhysicWorld(cpFloat width, cpFloat height, cpFloat elasticity, cpFloat friction, cpFloat thickness)
	: width(width), height(height), boundaryElasticity(elasticity), boundaryFriction(friction), boundaryThickness(thickness),
	staticFrictionCoeff(1.2f), kineticFrictionCoeff(1.0f), slipSpeedThreshold(0.5f)
{
    m_space = cpSpaceNew();
    staticBody = cpSpaceGetStaticBody(m_space);

    cpCollisionHandler* defaultHandler = cpSpaceAddDefaultCollisionHandler(m_space);
    defaultHandler->preSolveFunc = CollisionPreSolve;
    defaultHandler->userData = this;

	const cpFloat halfW = width * 0.5f;
	const cpFloat halfH = height * 0.5f;
    
	topBoundaryLine = cpSegmentShapeNew(staticBody, cpv(-halfW, halfH), cpv(halfW, halfH), thickness);
	bottomBoundaryLine = cpSegmentShapeNew(staticBody, cpv(-halfW, -halfH), cpv(halfW, -halfH), thickness);
	leftBoundaryLine = cpSegmentShapeNew(staticBody, cpv(-halfW, -halfH), cpv(-halfW, halfH), thickness);
	rightBoundaryLine = cpSegmentShapeNew(staticBody, cpv(halfW, -halfH), cpv(halfW, halfH), thickness);
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

cpBool PhysicWorld::CollisionPreSolve(cpArbiter* arb, cpSpace* space, cpDataPointer userData)
{
	(void)space;
	PhysicWorld* world = static_cast<PhysicWorld*>(userData);
	if (!world) return cpTrue;

	cpShape* shapeA = nullptr;
	cpShape* shapeB = nullptr;
	cpArbiterGetShapes(arb, &shapeA, &shapeB);
	if (!shapeA || !shapeB) return cpTrue;

	const cpContactPointSet contactSet = cpArbiterGetContactPointSet(arb);
	if (contactSet.count <= 0) return cpTrue;

	const cpBody* bodyA = cpShapeGetBody(shapeA);
	const cpBody* bodyB = cpShapeGetBody(shapeB);
	const cpVect normal = cpArbiterGetNormal(arb);
	const cpVect tangent = cpvperp(normal);

	cpFloat maxTangentialSpeed = 0.0f;
	for (int i = 0; i < contactSet.count; ++i)
	{
		const cpVect velocityA = cpBodyGetVelocityAtWorldPoint(bodyA, contactSet.points[i].pointA);
		const cpVect velocityB = cpBodyGetVelocityAtWorldPoint(bodyB, contactSet.points[i].pointB);
		const cpVect relativeVelocity = cpvsub(velocityB, velocityA);
		const cpFloat tangentialSpeed = (cpFloat)std::fabs(cpvdot(relativeVelocity, tangent));
		if (tangentialSpeed > maxTangentialSpeed)
			maxTangentialSpeed = tangentialSpeed;
	}

	const cpFloat baseFriction = cpShapeGetFriction(shapeA) * cpShapeGetFriction(shapeB);
	const cpFloat modelCoeff = (maxTangentialSpeed < world->slipSpeedThreshold)
		? world->staticFrictionCoeff
		: world->kineticFrictionCoeff;

	cpArbiterSetFriction(arb, baseFriction * modelCoeff);
	return cpTrue;
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