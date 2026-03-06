#pragma once
#include "Test.h"

namespace test {
    class EditProperties : public Test
    {
	private:
        int _ColliderType;
        int _ColliderShape;
    public:
        EditProperties(): _ColliderType(0), _ColliderShape(0) {}
        void OnUpdate(const void* p, float deltaTime) override {}
        void OnRender() override {}
        Test* OnImGuiRender(MyWindow* window) override
        {
            ImGui::Text("Editing Properties...");
			Component* target = window->selectedComponent;
            // Transform
            glm::vec2 pos, scale;
            float rot = 0.0f;
            target->GetPosition(pos);
            target->GetScale(scale);
            target->GetRotation(rot);

            if (ImGui::DragFloat2("Position", &pos.x, 1.0f)) {
                target->SetPosition(pos);
            }
            if (ImGui::DragFloat2("Scale", &scale.x, 0.1f, 0.0f, 1000.0f)) {
                target->SetScale(scale);
            }
            if (ImGui::DragFloat("Rotation", &rot, 1.0f, -360.0f, 360.0f)) {
                target->SetRotation(rot);
            }

            // Background color
            glm::vec4 color;
            target->GetBackgroundColor(color);
            if (ImGui::ColorEdit4("Background Color", &color.x)) {
                target->SetBackgroundColor(color);
            }

            // Physics flags
            //bool hasBody = target->hasPhysicBody;
            //if (ImGui::Checkbox("Has Physic Body", &hasBody)) {
            //    target->hasPhysicBody = hasBody;
            //    // 这里如果需要，可以在将来补：勾选时自动创建/销毁物理形状
            //}

            ImGui::Separator();
            ImGui::Text("Physics (read-only helpers / TODO):");
            
            if (!target->hasPhysicBody)
            {
                static const char* colliderItems[] = { "dynamic", "static" };
                ImGui::Text("Collider Type:");
                ImGui::SameLine();
                ImGui::Combo("##ColliderType", &_ColliderType, colliderItems, IM_ARRAYSIZE(colliderItems));
                if (ImGui::Button("Add Collider"))
                {
                    target->SetSensor(false);
                    if (_ColliderType == 0) // dynamic
                    {
						target->SwitchToDynamic();
                    }
                    else // static
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