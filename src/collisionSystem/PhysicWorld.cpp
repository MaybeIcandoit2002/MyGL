#include "PhysicWorld.h"
#include "../Models.h"

PhysicWorld::PhysicWorld()
{
    // 创建   
    m_space = cpSpaceNew();

    // 创建世界边界（静态body）
    staticBody = cpSpaceGetStaticBody(m_space);
}

PhysicWorld::~PhysicWorld()
{
    cpSpaceFree(m_space);
}

void PhysicWorld::SetGravity(cpVect gravity)
{
	cpSpaceSetGravity(m_space, gravity);
}

void PhysicWorld::SetBoundaryLine(cpVect v1, cpVect v2)
{
    cpShape* boundaryLine = cpSegmentShapeNew(staticBody, v1, v2, 50);
    cpShapeSetElasticity(boundaryLine, 1.0f);
    cpShapeSetFriction(boundaryLine, 1.0f);
    cpSpaceAddShape(m_space, boundaryLine);
}

void PhysicWorld::Step(double dt)
{
    cpSpaceStep(m_space, dt);
}

void PhysicWorld::AddCircle(Models* model)
{
    std::vector<ModelsPose> pose = model->GetPoses();
    std::vector<ModelsPhysicProperties> physicProps = model->GetPhysicProperties();
    std::vector<cpBody*> vecBody = model->GetPhysicBody();
    std::vector<cpShape*> vecShape = model->GetPhysicShape();
	uint32_t count = model->GetModelCount();
    for (uint32_t i = 0; i < count; i++)
    {
		float mass = physicProps[i].mass;
		float radius = physicProps[i].shape1;
        cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
        cpBody* body = cpSpaceAddBody(m_space, cpBodyNew(mass, moment));
        cpBodySetPosition(body, cpv(pose[i].x, pose[i].y));
        cpShape* shape = cpSpaceAddShape(m_space, cpCircleShapeNew(body, radius, cpvzero));
        cpShapeSetFriction(shape, physicProps[i].friction);
        cpShapeSetElasticity(shape, physicProps[i].elasticity);
		vecBody[i] = body;
		vecShape[i] = shape;
    }
}

void PhysicWorld::AddBox(Models* model)
{
    std::vector<ModelsPose> pose = model->GetPoses();
    std::vector<ModelsPhysicProperties> physicProps = model->GetPhysicProperties();
    std::vector<cpBody*> vecBody = model->GetPhysicBody();
    std::vector<cpShape*> vecShape = model->GetPhysicShape();
    uint32_t count = model->GetModelCount();
    for (uint32_t i = 0; i < count; i++)
    {
        float mass = physicProps[i].mass;
		float width = physicProps[i].shape1;
		float height = physicProps[i].shape2;
        float moment = cpMomentForBox(mass, width, height);
        cpBody* body = cpSpaceAddBody(m_space, cpBodyNew(mass, moment));
        cpBodySetPosition(body, cpv(pose[i].x, pose[i].y));
        cpShape* shape = cpSpaceAddShape(m_space, cpBoxShapeNew(body, width, height, 0));
        cpShapeSetFriction(shape, physicProps[i].friction);
        cpShapeSetElasticity(shape, physicProps[i].elasticity);
        vecBody[i] = body;
        vecShape[i] = shape;
    }
}