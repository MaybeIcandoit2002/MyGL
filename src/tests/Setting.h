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

            return nullptr;
        }
    };
}
