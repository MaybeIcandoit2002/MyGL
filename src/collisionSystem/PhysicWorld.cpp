#include "PhysicWorld.h"

PhysicWorld::PhysicWorld()
{
    // ����   
    m_space = cpSpaceNew();

    // ��������߽磨��̬body��
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

void PhysicWorld::AddCircle(Models& model)
{
    float* pose = model.GetPose();
	float* physicProps = model.GetPhysicProperties();
    float x = pose[0] + physicProps[0];
    float y = pose[1] + physicProps[1];
    float radius = physicProps[2] / 2;
	float mass = physicProps[3];

    // ����ת������
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    // ��������
    cpBody* body = cpSpaceAddBody(m_space, cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(x, y));

    // ����Բ��shape
    cpShape* shape = cpSpaceAddShape(m_space, cpCircleShapeNew(body, radius, cpvzero));
    cpShapeSetFriction(shape, 0.7f);
    cpShapeSetElasticity(shape, 0.8f);

    // ��ѡ��������ײ���͡��û����ݵ�
    // cpShapeSetCollisionType(shape, 1);
    // cpShapeSetUserData(shape, ...);

	model.SetPhysicBody(body);
}

// ��ѡ����Ӿ�������
/*
cpBody* PhysicWorld::AddBox(float x, float y, float width, float height, float mass)
{
    float moment = cpMomentForBox(mass, width, height);
    cpBody* body = cpSpaceAddBody(m_space, cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(x, y));
    cpShape* shape = cpSpaceAddShape(m_space, cpBoxShapeNew(body, width, height, 0));
    cpShapeSetFriction(shape, 0.7f);
    cpShapeSetElasticity(shape, 0.8f);
    return body;
}
*/
