#pragma once

#include "EditProperties.h"

namespace test {
    class SearchRoot : public Test
    {
    public:
        SearchRoot() {}
        void OnUpdate(const void* p, float deltaTime) override {}
        void OnRender() override {}
        Test* OnImGuiRender(MyWindow* window) override
        {
            ImGui::Text("RootComponent Children");
            Component* root = window->GetRootComponent();
            if (!root)
            {
                ImGui::TextDisabled("RootComponent is null.");
                return nullptr;
            }

            if (root->children.empty())
            {
                ImGui::TextDisabled("No child component.");
                return nullptr;
            }

            for (Component* child : root->children)
            {
                if (!child) continue;
                const char* label = child->name.empty() ? "<unnamed>" : child->name.c_str();
                if (ImGui::Button(label))
                {
                    window->selectedComponent = child;
                    window->changeSelectedComponent = true;
                    return nullptr;
                }
            }
            return nullptr;
        }
    };
}
