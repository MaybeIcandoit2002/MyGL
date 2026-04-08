#pragma once

#include "Test.h"

namespace test {
    class Setting : public Test
    {
    public:
        Setting() {}
        void OnUpdate(const void* p, float deltaTime) override {}
        void OnRender() override {}
        Test* OnImGuiRender(MyWindow* window) override
        {
            ImGui::Text("Window Setting");
            float scale = window->GetWindowScale();
            if (ImGui::DragFloat("Window Scale", &scale, 0.1f, 1.0f, 2.5f, "%.1f"))
            {
                window->SetWindowScale(scale);
            }
            ImGui::Text("(Range: 1.0 - 2.5, Step: 0.1)");
            ImGui::Text("View Scale (Mouse Wheel): %.2f", window->GetViewScale());

            ImGui::Separator();
            glm::vec4 clearColor = window->GetClearColor();
            float clearColorValue[4] = { clearColor.r, clearColor.g, clearColor.b, clearColor.a };
            if (ImGui::ColorEdit4("Clear Color", clearColorValue))
            {
                window->SetClearColor(clearColorValue[0], clearColorValue[1], clearColorValue[2], clearColorValue[3]);
            }

            Component* root = window->GetRootComponent();
            if (root)
            {
                glm::vec4 rootColor;
                root->GetBackgroundColor(rootColor);
                float rootColorValue[4] = { rootColor.r, rootColor.g, rootColor.b, rootColor.a };
                if (ImGui::ColorEdit4("Root Node Color", rootColorValue))
                {
                    root->SetBackgroundColor(rootColorValue[0], rootColorValue[1], rootColorValue[2], rootColorValue[3]);
                }
            }

            bool autoHideEditProperties = window->GetAutoHideEditProperties();
            if (ImGui::Checkbox("Auto Hide Edit Properties", &autoHideEditProperties))
            {
                window->SetAutoHideEditProperties(autoHideEditProperties);
            }

            bool enableConfirmPopup = window->GetEnableConfirmPopup();
            if (ImGui::Checkbox("Enable Confirm Popup", &enableConfirmPopup))
            {
                window->SetEnableConfirmPopup(enableConfirmPopup);
            }

            ImGui::Separator();
            ImGui::Text("Advanced Physics (Less Common)");
            PhysicWorld* physicWorld = window->GetPhysicWorld();
            if (physicWorld)
            {
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

                if (ImGui::Button("Reset Advanced Physics Params"))
                {
                    physicWorld->SetStaticFrictionCoeff(1.2f);
                    physicWorld->SetKineticFrictionCoeff(0.8f);
                    physicWorld->SetSlipSpeedThreshold(0.5f);
                    window->SetJointLineThickness(2.0f);
                }
            }

            return nullptr;
        }
    };
}
