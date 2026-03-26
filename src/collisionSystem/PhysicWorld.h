#pragma once
#include <chipmunk.h>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <utility>

enum ShapeType
{
    Circle,
    Box,
    Polygon
};

class PhysicWorld
{
private:
    struct JointInfo
    {
        cpConstraint* constraint;
        cpVect anchorA;
        cpVect anchorB;
    };

public:
    using JointId = std::uint32_t;
    static constexpr JointId InvalidJointId = 0;

    struct JointSnapshot
    {
        JointId id;
        cpBody* bodyA;
        cpBody* bodyB;
        cpVect anchorA;
        cpVect anchorB;
        int type; // 0: Cord(Slide), 1: Rod(Pin), 2: Spring(DampedSpring)
        cpFloat distance;
        cpFloat stiffness;
        cpFloat damping;
    };

    PhysicWorld(cpFloat width, cpFloat height, cpFloat elasticity, cpFloat friction, cpFloat thickness);
    ~PhysicWorld();
	void SetGravity(cpVect gravity);
	cpVect GetGravity() const { return cpSpaceGetGravity(const_cast<cpSpace*>(m_space)); }
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
    JointId AddCord(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat distance);
    JointId AddRod(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat distance);
    JointId AddSpring(cpBody* bodyA, cpBody* bodyB, cpVect anchorA, cpVect anchorB, cpFloat restLength, cpFloat stiffness, cpFloat damping);
    bool RemoveJoint(JointId jointId);
    std::size_t RemoveJointsByBody(cpBody* body);
    std::vector<JointId> GetJointsByBody(cpBody* body) const;
    std::vector<std::pair<cpVect, cpVect>> GetJointWorldSegments() const;
    std::vector<JointSnapshot> GetJointSnapshots() const;
    bool GetJointSnapshot(JointId jointId, JointSnapshot& outSnapshot) const;
    bool ResetFlexibleJoint(JointId jointId);
    bool SetSpringParams(JointId jointId, cpFloat restLength, cpFloat stiffness, cpFloat damping);
    void SyncFixedLengthJointsByBody(cpBody* body);

    void SetStaticFrictionCoeff(cpFloat coeff) { staticFrictionCoeff = coeff; }
    void SetKineticFrictionCoeff(cpFloat coeff) { kineticFrictionCoeff = coeff; }
    void SetSlipSpeedThreshold(cpFloat threshold) { slipSpeedThreshold = threshold; }
    cpFloat GetStaticFrictionCoeff() const { return staticFrictionCoeff; }
    cpFloat GetKineticFrictionCoeff() const { return kineticFrictionCoeff; }
    cpFloat GetSlipSpeedThreshold() const { return slipSpeedThreshold; }

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

    cpFloat staticFrictionCoeff;
    cpFloat kineticFrictionCoeff;
    cpFloat slipSpeedThreshold;
    cpFloat stepAccumulator;

    std::unordered_map<JointId, JointInfo> joints;
    JointId nextJointId;

    JointId AddJoint(cpConstraint* constraint, cpVect anchorA, cpVect anchorB);

    static cpBool CollisionPreSolve(cpArbiter* arb, cpSpace* space, cpDataPointer userData);
public:
    static void Initialize(
        PhysicWorld*& world, cpFloat width, cpFloat height,
        cpFloat elasticity = 1.0f, cpFloat friction = 1.0f, cpFloat thickness = 50.f);
    static void Free();
    static PhysicWorld* Instance() { return PhysicWorld::instance; }
};