#include "PhysicWorld.h"
#include <cmath>
#include <vector>

PhysicWorld* PhysicWorld::instance = nullptr;

PhysicWorld::PhysicWorld(cpFloat width, cpFloat height, cpFloat elasticity, cpFloat friction, cpFloat thickness)
	: width(width), height(height), boundaryElasticity(elasticity), boundaryFriction(friction), boundaryThickness(thickness),
	staticFrictionCoeff(1.2f), kineticFrictionCoeff(1.0f), slipSpeedThreshold(0.5f), stepAccumulator(0.0f), nextJointId(1)
{
    m_space = cpSpaceNew();
    staticBody = cpSpaceGetStaticBody(m_space);
    cpSpaceSetIterations(m_space, 20);

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
    for (auto& pair : joints)
    {
        cpConstraint* constraint = pair.second.constraint;
        if (!constraint) continue;
        cpSpaceRemoveConstraint(m_space, constraint);
        cpConstraintFree(constraint);
    }
    joints.clear();

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
    if (dt <= 0.0) return;

    const cpFloat fixedDt = 1.0f / 120.0f;
    const cpFloat maxFrameDt = 1.0f / 20.0f;
    const int maxSubSteps = 8;

    cpFloat frameDt = static_cast<cpFloat>(dt);
    if (frameDt > maxFrameDt) frameDt = maxFrameDt;

    stepAccumulator += frameDt;
    int subStepCount = 0;

    while (stepAccumulator >= fixedDt && subStepCount < maxSubSteps)
    {
        cpSpaceStep(m_space, fixedDt);
        stepAccumulator -= fixedDt;
        ++subStepCount;
    }

    if (subStepCount == maxSubSteps)
        stepAccumulator = 0.0f;
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

PhysicWorld::JointId PhysicWorld::AddJoint(cpConstraint* constraint, cpVect anchorA, cpVect anchorB)
{
    if (!constraint) return InvalidJointId;
    cpConstraintSetCollideBodies(constraint, cpFalse);
    cpSpaceAddConstraint(m_space, constraint);
    const JointId id = nextJointId++;
    joints[id] = { constraint, anchorA, anchorB };
    return id;
}

PhysicWorld::JointId PhysicWorld::AddCord(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat distance)
{
    if (!bodyA || !bodyB) return InvalidJointId;
    const cpFloat maxDistance = (distance > 1e-3f) ? distance : 1e-3f;
    cpConstraint* constraint = cpSlideJointNew(bodyA, bodyB, anchorA, anchorB, 0.0f, maxDistance);
    return AddJoint(constraint, anchorA, anchorB);
}

PhysicWorld::JointId PhysicWorld::AddRod(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat distance)
{
    if (!bodyA || !bodyB) return InvalidJointId;
    const cpFloat dist = (distance > 1e-3f) ? distance : 1e-3f;
    cpConstraint* constraint = cpPinJointNew(bodyA, bodyB, anchorA, anchorB);
    cpPinJointSetDist(constraint, dist);
    return AddJoint(constraint, anchorA, anchorB);
}

PhysicWorld::JointId PhysicWorld::AddSpring(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat restLength, cpFloat stiffness, cpFloat damping)
{
    if (!bodyA || !bodyB) return InvalidJointId;
    const cpFloat safeRestLength = (restLength > 1e-3f) ? restLength : 1e-3f;
	cpConstraint* constraint = cpDampedSpringNew(
		bodyA,  bodyB,
		anchorA,anchorB,
		safeRestLength, stiffness, damping
	);
	return AddJoint(constraint, anchorA, anchorB);
}

bool PhysicWorld::RemoveJoint(JointId jointId)
{
    auto it = joints.find(jointId);
    if (it == joints.end()) return false;

    cpConstraint* constraint = it->second.constraint;
    if (constraint)
    {
        cpSpaceRemoveConstraint(m_space, constraint);
        cpConstraintFree(constraint);
    }

    joints.erase(it);
    return true;
}

std::size_t PhysicWorld::RemoveJointsByBody(cpBody* body)
{
    if (!body) return 0;

    std::vector<JointId> toRemove;
    toRemove.reserve(joints.size());

    for (const auto& pair : joints)
    {
        cpConstraint* constraint = pair.second.constraint;
        if (!constraint) continue;

        if (cpConstraintGetBodyA(constraint) == body || cpConstraintGetBodyB(constraint) == body)
            toRemove.push_back(pair.first);
    }

    for (JointId id : toRemove)
        RemoveJoint(id);

    return toRemove.size();
}

std::vector<PhysicWorld::JointId> PhysicWorld::GetJointsByBody(cpBody* body) const
{
    std::vector<JointId> result;
    if (!body) return result;

    result.reserve(joints.size());
    for (const auto& pair : joints)
    {
        cpConstraint* constraint = pair.second.constraint;
        if (!constraint) continue;

        if (cpConstraintGetBodyA(constraint) == body || cpConstraintGetBodyB(constraint) == body)
            result.push_back(pair.first);
    }

    return result;
}

std::vector<std::pair<cpVect, cpVect>> PhysicWorld::GetJointWorldSegments() const
{
    std::vector<std::pair<cpVect, cpVect>> segments;
    segments.reserve(joints.size());

    for (const auto& pair : joints)
    {
        const JointInfo& info = pair.second;
        cpConstraint* constraint = info.constraint;
        if (!constraint) continue;

        cpBody* bodyA = cpConstraintGetBodyA(constraint);
        cpBody* bodyB = cpConstraintGetBodyB(constraint);
        if (!bodyA || !bodyB) continue;

        const cpVect worldA = cpBodyLocalToWorld(bodyA, info.anchorA);
        const cpVect worldB = cpBodyLocalToWorld(bodyB, info.anchorB);
        segments.emplace_back(worldA, worldB);
    }

    return segments;
}

std::vector<PhysicWorld::JointSnapshot> PhysicWorld::GetJointSnapshots() const
{
    std::vector<JointSnapshot> snapshots;
    snapshots.reserve(joints.size());

    for (const auto& pair : joints)
    {
        JointSnapshot snapshot{};
        if (GetJointSnapshot(pair.first, snapshot))
            snapshots.push_back(snapshot);
    }

    return snapshots;
}

bool PhysicWorld::GetJointSnapshot(JointId jointId, JointSnapshot& outSnapshot) const
{
    auto it = joints.find(jointId);
    if (it == joints.end()) return false;

    const JointInfo& info = it->second;
    cpConstraint* constraint = info.constraint;
    if (!constraint) return false;

    cpBody* bodyA = cpConstraintGetBodyA(constraint);
    cpBody* bodyB = cpConstraintGetBodyB(constraint);
    if (!bodyA || !bodyB) return false;

    outSnapshot = {};
    outSnapshot.id = jointId;
    outSnapshot.bodyA = bodyA;
    outSnapshot.bodyB = bodyB;
    outSnapshot.anchorA = info.anchorA;
    outSnapshot.anchorB = info.anchorB;
    outSnapshot.type = 0;
    outSnapshot.distance = cpvdist(cpBodyLocalToWorld(bodyA, info.anchorA), cpBodyLocalToWorld(bodyB, info.anchorB));
    outSnapshot.stiffness = 0.0f;
    outSnapshot.damping = 0.0f;

    if (cpConstraintIsPinJoint(constraint))
    {
        outSnapshot.type = 1;
        outSnapshot.distance = cpPinJointGetDist(constraint);
    }
    else if (cpConstraintIsSlideJoint(constraint))
    {
        outSnapshot.type = 0;
        outSnapshot.distance = cpSlideJointGetMax(constraint);
    }
    else if (cpConstraintIsDampedSpring(constraint))
    {
        outSnapshot.type = 2;
        outSnapshot.distance = cpDampedSpringGetRestLength(constraint);
        outSnapshot.stiffness = cpDampedSpringGetStiffness(constraint);
        outSnapshot.damping = cpDampedSpringGetDamping(constraint);
    }

    return true;
}

bool PhysicWorld::ResetFlexibleJoint(JointId jointId)
{
    auto it = joints.find(jointId);
    if (it == joints.end()) return false;

    JointInfo& info = it->second;
    cpConstraint* constraint = info.constraint;
    if (!constraint) return false;

    cpBody* bodyA = cpConstraintGetBodyA(constraint);
    cpBody* bodyB = cpConstraintGetBodyB(constraint);
    if (!bodyA || !bodyB) return false;

    const cpFloat distance = cpvdist(cpBodyLocalToWorld(bodyA, info.anchorA), cpBodyLocalToWorld(bodyB, info.anchorB));

    if (cpConstraintIsSlideJoint(constraint))
    {
        const cpFloat d = (distance > 1e-3f) ? distance : 1e-3f;
        cpSlideJointSetMax(constraint, d);
        return true;
    }

    if (cpConstraintIsDampedSpring(constraint))
    {
        const cpFloat d = (distance > 1e-3f) ? distance : 1e-3f;
        cpDampedSpringSetRestLength(constraint, d);
        return true;
    }

    return false;
}

bool PhysicWorld::SetSpringParams(JointId jointId, cpFloat restLength, cpFloat stiffness, cpFloat damping)
{
    auto it = joints.find(jointId);
    if (it == joints.end()) return false;

    cpConstraint* constraint = it->second.constraint;
    if (!constraint || !cpConstraintIsDampedSpring(constraint)) return false;

    if (restLength < 1e-3f) restLength = 1e-3f;
    if (stiffness < 0.0f) stiffness = 0.0f;
    if (damping < 0.0f) damping = 0.0f;

    cpDampedSpringSetRestLength(constraint, restLength);
    cpDampedSpringSetStiffness(constraint, stiffness);
    cpDampedSpringSetDamping(constraint, damping);
    return true;
}

void PhysicWorld::SyncFixedLengthJointsByBody(cpBody* body)
{
    if (!body) return;

    for (auto& pair : joints)
    {
        JointInfo& info = pair.second;
        cpConstraint* constraint = info.constraint;
        if (!constraint || !cpConstraintIsPinJoint(constraint))
            continue;

        cpBody* bodyA = cpConstraintGetBodyA(constraint);
        cpBody* bodyB = cpConstraintGetBodyB(constraint);
        if (bodyA != body && bodyB != body)
            continue;

        const cpFloat distance = cpvdist(cpBodyLocalToWorld(bodyA, info.anchorA), cpBodyLocalToWorld(bodyB, info.anchorB));
        cpPinJointSetDist(constraint, (distance > 1e-3f) ? distance : 1e-3f);
    }
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