#pragma once
#include <chipmunk.h>
#include "../Models.h"

// ���������װ��
class PhysicWorld
{
public:
    PhysicWorld();
    ~PhysicWorld();

	void SetGravity(cpVect gravity);
	void SetBoundaryLine(cpVect v1, cpVect v2);

    // ������������
    void Step(double dt);

    // ���һ��Բ������
    void AddCircle(Models& model);

    // ��ѡ�����������״
    // cpBody* AddBox(float x, float y, float width, float height, float mass);

private:
    cpSpace* m_space; 
    cpBody* staticBody;
};

