#pragma once

#include "Test.h"

namespace test {
    class GlobalPhysicParams : public Test
    {
    public:
        GlobalPhysicParams() {}
        void OnUpdate(const void* p, float deltaTime) override {}
        void OnRender() override {}
        Test* OnImGuiRender(MyWindow* window) override
        {
            if (!window) return nullptr;
            PhysicWorld* physicWorld = window->GetPhysicWorld();
            if (!physicWorld) return nullptr;

            ImGui::Text("Global Physic Params");

            Component* anchorA = window->GetBindingAnchor();
            const char* anchorName = (!anchorA) ? "<none>" : (anchorA->name.empty() ? "<unnamed>" : anchorA->name.c_str());
            ImGui::Text("Joint A: %s", anchorName);
            ImGui::SameLine();
            if (ImGui::Button("Clear Joint A"))
            {
                window->ClearBindingAnchor();
            }

            glm::vec4 jointLineColor = window->GetJointLineColor();
            float jointLineColorValue[4] = { jointLineColor.r, jointLineColor.g, jointLineColor.b, jointLineColor.a };
            if (ImGui::ColorEdit4("Joint Line Color", jointLineColorValue))
            {
                window->SetJointLineColor(glm::vec4(jointLineColorValue[0], jointLineColorValue[1], jointLineColorValue[2], jointLineColorValue[3]));
            }

            float jointLineThickness = window->GetJointLineThickness();
            if (ImGui::DragFloat("Joint Line Thickness", &jointLineThickness, 0.1f, 0.5f, 10.0f, "%.2f"))
            {
                window->SetJointLineThickness(jointLineThickness);
            }

            cpVect gravity = physicWorld->GetGravity();
            float gravityValue[2] = { static_cast<float>(gravity.x), static_cast<float>(gravity.y) };
            if (ImGui::DragFloat2("Gravity", gravityValue, 0.1f, -100.0f, 100.0f))
            {
                physicWorld->SetGravity(cpv(gravityValue[0], gravityValue[1]));
            }

            float staticFriction = static_cast<float>(physicWorld->GetStaticFrictionCoeff());
            if (ImGui::DragFloat("Static Friction Coeff", &staticFriction, 0.01f, 0.0f, 5.0f))
            {
                physicWorld->SetStaticFrictionCoeff(staticFriction);
            }

            float kineticFriction = static_cast<float>(physicWorld->GetKineticFrictionCoeff());
            if (ImGui::DragFloat("Kinetic Friction Coeff", &kineticFriction, 0.01f, 0.0f, 5.0f))
            {
                physicWorld->SetKineticFrictionCoeff(kineticFriction);
            }

            float slipThreshold = static_cast<float>(physicWorld->GetSlipSpeedThreshold());
            if (ImGui::DragFloat("Slip Speed Threshold", &slipThreshold, 0.01f, 0.0f, 20.0f))
            {
                physicWorld->SetSlipSpeedThreshold(slipThreshold);
            }

            if (ImGui::Button("Reset Global Physic Params"))
            {
                physicWorld->SetGravity(cpv(0, -9.8));
                physicWorld->SetStaticFrictionCoeff(1.2f);
                physicWorld->SetKineticFrictionCoeff(0.8f);
                physicWorld->SetSlipSpeedThreshold(0.5f);
            }

            return nullptr;
        }
    };
}
