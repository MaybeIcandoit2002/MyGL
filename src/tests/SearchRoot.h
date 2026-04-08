#pragma once

#include "EditProperties.h"
#include <cstdio>

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
                const char* displayName = child->name.empty() ? "<unnamed>" : child->name.c_str();
                char buttonLabel[192] = {};
                std::snprintf(buttonLabel, sizeof(buttonLabel), "%s##%llu", displayName, static_cast<unsigned long long>(child->GetStableId()));
                if (ImGui::Button(buttonLabel))
                {
                    window->SelectComponent(child);
                    return nullptr;
                }
                ImGui::SameLine();
                ImGui::TextDisabled("ID:%llu", static_cast<unsigned long long>(child->GetStableId()));
            }
            return nullptr;
        }
    };
}
