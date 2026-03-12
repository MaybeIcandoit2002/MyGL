#pragma once
#include "Test.h"
#include <cstdio>

namespace test {
    class EditProperties : public Test
    {
	private:
        int _ColliderType;
        int _ColliderShape;
        Component* _NameTarget;
        char _NameBuffer[128];
    public:
        EditProperties(): _ColliderType(0), _ColliderShape(0), _NameTarget(nullptr), _NameBuffer{} {}
        void OnUpdate(const void* p, float deltaTime) override {}
        void OnRender() override {}
        Test* OnImGuiRender(MyWindow* window) override
        {
            ImGui::Text("Editing Properties...");
			Component* target = window->GetSelectedComponent();
            if (!target)
            {
                ImGui::TextDisabled("No component selected.");
                return nullptr;
            }

            if (_NameTarget != target)
            {
                _NameTarget = target;
                std::snprintf(_NameBuffer, sizeof(_NameBuffer), "%s", target->name.c_str());
            }
            if (ImGui::InputText("Name", _NameBuffer, IM_ARRAYSIZE(_NameBuffer)))
            {
                target->name = _NameBuffer;
            }
            ImGui::Text("Node ID: %llu", static_cast<unsigned long long>(target->GetStableId()));

            // Transform
            glm::vec2 pos, scale;
            float rot = 0.0f;
            target->GetPosition(pos);
            target->GetScale(scale);
            target->GetRotation(rot);

            if (ImGui::DragFloat2("Position", &pos.x, 1.0f))
            {
                if (!window->running) target->SetPosition(pos);
            }
            if (ImGui::DragFloat2("Scale", &scale.x, 0.1f, 0.0f, 1000.0f))
            {
                if (!window->running) target->SetScale(scale);
            }
            if (ImGui::DragFloat("Rotation", &rot, 1.0f, -360.0f, 360.0f))
            {
                if (!window->running) target->SetRotation(rot);
            }

            // Background color
            glm::vec4 color;
            target->GetBackgroundColor(color);
            if (ImGui::ColorEdit4("Background Color", &color.x)) {
                target->SetBackgroundColor(color);
            }

            ImGui::Separator();
            ImGui::Text("Binding Editor:");
            Component* anchor = window->GetBindingAnchor();
            if (ImGui::Button("Set Current As Joint A"))
            {
                window->SetBindingAnchor(target);
            }

            static int jointType = 0;
            static const char* jointTypeItems[] = { "Cord", "Rod", "Spring" };
            static float springRestLength = 0.0f;
            static float springStiffness = 60.0f;
            static float springDamping = 8.0f;

            const bool canBind = anchor && anchor != target
                && anchor->hasPhysicBody && target->hasPhysicBody
                && anchor->GetBody() && target->GetBody();

            if (anchor == target)
            {
                ImGui::TextDisabled("Joint A is current node. Select another node as B.");
            }
            else if (!anchor)
            {
                ImGui::TextDisabled("Set Joint A first, then switch to B and create binding.");
            }
            else if (!canBind)
            {
                ImGui::TextDisabled("Both A/B must have active physics body.");
            }
            else
            {
                ImGui::Text("B: %s", target->name.empty() ? "<unnamed>" : target->name.c_str());
                ImGui::Combo("Joint Type", &jointType, jointTypeItems, IM_ARRAYSIZE(jointTypeItems));

                const cpFloat distance = cpvdist(cpBodyGetPosition(anchor->GetBody()), cpBodyGetPosition(target->GetBody()));
                ImGui::Text("Distance(A,B): %.2f", static_cast<float>(distance));

                if (jointType == 2)
                {
                    ImGui::DragFloat("Spring Rest Length", &springRestLength, 0.1f, 0.0f, 10000.0f);
                    ImGui::DragFloat("Spring Stiffness", &springStiffness, 0.5f, 0.0f, 100000.0f);
                    ImGui::DragFloat("Spring Damping", &springDamping, 0.1f, 0.0f, 100000.0f);
                }

                if (ImGui::Button("Create A-B Binding"))
                {
                    PhysicWorld* world = window->GetPhysicWorld();
                    if (world)
                    {
                        if (jointType == 0)
                            world->AddCord(anchor->GetBody(), target->GetBody(), cpvzero, cpvzero, distance);
                        else if (jointType == 1)
                            world->AddRod(anchor->GetBody(), target->GetBody(), cpvzero, cpvzero, distance);
                        else
                        {
                            const cpFloat rest = (springRestLength > 0.0f) ? springRestLength : distance;
                            world->AddSpring(anchor->GetBody(), target->GetBody(), cpvzero, cpvzero, rest, springStiffness, springDamping);
                        }
                    }
                }
            }

            if (target->hasPhysicBody && target->GetBody())
            {
                PhysicWorld* world = window->GetPhysicWorld();
                if (world)
                {
                    std::vector<PhysicWorld::JointId> joints = world->GetJointsByBody(target->GetBody());
                    if (joints.empty())
                    {
                        ImGui::TextDisabled("No bindings on current node.");
                    }
                    else
                    {
                        ImGui::Text("Bindings on current node:");
                        for (PhysicWorld::JointId id : joints)
                        {
                            ImGui::PushID(static_cast<int>(id));
                            PhysicWorld::JointSnapshot snapshot{};
                            const bool hasSnapshot = world->GetJointSnapshot(id, snapshot);
                            const char* typeName = "Unknown";
                            if (hasSnapshot)
                            {
                                if (snapshot.type == 0) typeName = "Cord";
                                else if (snapshot.type == 1) typeName = "Rod";
                                else if (snapshot.type == 2) typeName = "Spring";
                            }

                            ImGui::Text("Joint #%u (%s)", static_cast<unsigned>(id), typeName);

                            if (hasSnapshot && snapshot.type != 1)
                            {
                                if (ImGui::Button("Reset Length"))
                                {
                                    world->ResetFlexibleJoint(id);
                                }
                                ImGui::SameLine();
                            }

                            if (hasSnapshot && snapshot.type == 2)
                            {
                                float rest = static_cast<float>(snapshot.distance);
                                float stiffness = static_cast<float>(snapshot.stiffness);
                                float damping = static_cast<float>(snapshot.damping);
                                bool changed = false;
                                if (ImGui::DragFloat("Rest Length", &rest, 0.1f, 0.0f, 10000.0f)) changed = true;
                                if (ImGui::DragFloat("Stiffness", &stiffness, 0.5f, 0.0f, 100000.0f)) changed = true;
                                if (ImGui::DragFloat("Damping", &damping, 0.1f, 0.0f, 100000.0f)) changed = true;
                                if (changed)
                                {
                                    world->SetSpringParams(id, rest, stiffness, damping);
                                }
                            }

                            if (ImGui::Button("Delete"))
                            {
                                world->RemoveJoint(id);
                                ImGui::PopID();
                                break;
                            }
                            ImGui::PopID();
                        }
                    }
                }
            }

            ImGui::Separator();
            if (ImGui::Button("Delete Component"))
            {
                if (!window->running)
                {
                    Component* root = window->GetRootComponent();
                    window->ClearBindingAnchorIf(target);
                    root->RemoveChild(target);
                    delete target;
                    window->ClearSelection();
                    return nullptr;
                }
            }

            ImGui::Separator();
            ImGui::Text("Physics Editor:");
            
            
            if (!target->hasPhysicBody)
            {
                static const char* colliderItems[] = { "dynamic", "static" };
                ImGui::Text("Collider Type:");
                ImGui::SameLine();
                ImGui::Combo("##ColliderType", &_ColliderType, colliderItems, IM_ARRAYSIZE(colliderItems));
                if (ImGui::Button("Add Collider"))
                {
                    target->SetSensor(false);
                    if (_ColliderType == 0)
                    {
						target->SwitchToDynamic();
                    }
                    else
                    {
                        target->SwitchToStatic();
                    }
				}
			}
            else
            {
                glm::vec2 vel = target->GetVelocity();
                float angularVel = target->GetAngleVelocity();
                float mass = target->GetMass();
                float friction = target->GetFriction();
                float restitution = target->GetRestitution();
                const glm::vec2 acceleration = target->GetAcceleration();
                const float angularAcceleration = target->GetAngularAcceleration();
                const float moment = target->GetMoment();
                const glm::vec2 momentum = target->GetMomentum();
                const float kineticEnergy = target->GetKineticEnergy();
                glm::vec2 relativePos, relativeScale;
                target->GetPhysicRelativePosition(relativePos);
                target->GetPhysicRelativeScale(relativeScale);

                if (ImGui::DragFloat2("Collider Relative Pos", &relativePos.x, 0.1f))
                {
                    if (!window->running) target->SetPhysicRelativePosition(relativePos);
                }
                if (ImGui::DragFloat2("Collider Relative Scale", &relativeScale.x, 0.01f, 0.01f, 100.0f))
                {
                    if (!window->running) target->SetPhysicRelativeScale(relativeScale);
                }

                float size1 = target->physicSize1;
                float size2 = target->physicSize2 ? target->physicSize2[0] : 0.0f;
                if (target->shapeType == ShapeType::Circle)
                {
                    if (ImGui::DragFloat("Collider Radius", &size1, 0.1f, 0.01f, 10000.0f))
                    {
                        if (!window->running) target->SetPhysicSize(size1);
                    }
                }
                else if (target->shapeType == ShapeType::Box)
                {
                    bool sizeChanged = false;
                    if (ImGui::DragFloat("Collider Width", &size1, 0.1f, 0.01f, 10000.0f)) sizeChanged = true;
                    if (ImGui::DragFloat("Collider Height", &size2, 0.1f, 0.01f, 10000.0f)) sizeChanged = true;
                    if (sizeChanged)
                    {
                        if (!window->running) target->SetPhysicSize(size1, size2);
                    }
                }
                ImGui::TextDisabled("Acceleration: (%.3f, %.3f)", acceleration.x, acceleration.y);
                ImGui::TextDisabled("Angular Acceleration: %.3f deg/s^2", angularAcceleration);
                ImGui::TextDisabled("Moment of Inertia: %.3f", moment);
                ImGui::TextDisabled("Momentum: (%.3f, %.3f)", momentum.x, momentum.y);
                ImGui::TextDisabled("Kinetic Energy: %.3f", kineticEnergy);

                if (ImGui::DragFloat2("Velocity", &vel.x, 0.1f))
                {
                    target->SetVelocity(vel);
                }
                if (ImGui::DragFloat("Angular Velocity", &angularVel, 0.1f))
                {
                    target->SetAngleVelocity(angularVel);
				}
                if (ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, 1000.0f))
                {
                    target->SetMass(mass);
				}
                if (ImGui::DragFloat("Friction", &friction, 0.01f, 0.0f, 1.0f))
                {
					target->SetFriction(friction);
                }
                if (ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f))
                {
                    target->SetRestitution(restitution);
				}
            }
			return nullptr;
        }
    };
}