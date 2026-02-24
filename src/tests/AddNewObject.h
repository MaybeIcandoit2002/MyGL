#pragma once
#include "EditProperties.h"

namespace test {
    class AddNewObject : public Test
    {
	public:
        AddNewObject() {}
        void OnUpdate(const void* p, float deltaTime) override {}
        void OnRender() override {}
        Test* OnImGuiRender(MyWindow* window) override
        {
            ImGui::Text("Add New Object");
            float defaultWidth = window->GetHeight() / 2.0f;
            float defaultHeight = window->GetHeight() / 2.0f;

            if (ImGui::Button("box")) {
                Mesh* mesh = Sources::Instance()->GetMesh("box");
                if (mesh && window) {
                    Component* child = new Component(mesh);
                    window->GetRootComponent()->AddChild(child);
					window->selectedComponent = child;
					child->SetPosition(0, 0);
                    child->SetScale(defaultWidth, defaultHeight);
					return new EditProperties();
                }
            }
            if (ImGui::Button("circle")) {
                Mesh* mesh = Sources::Instance()->GetMesh("circle");
                if (mesh && window) {
                    Component* child = new Component(mesh);
                    window->GetRootComponent()->AddChild(child);
                    window->selectedComponent = child;
                    child->SetPosition(0, 0);
                    child->SetScale(defaultWidth, defaultHeight);
                    return new EditProperties();
                }
            }
			return nullptr;
        }
    };
}