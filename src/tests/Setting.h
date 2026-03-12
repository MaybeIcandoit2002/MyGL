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
            return nullptr;
        }
    };
}
