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

            if (ImGui::Button("box"))
            {
                Mesh* mesh = Sources::Instance()->GetMesh("box");
                if (mesh && window) {
                    Component* child = new Component(mesh);
                    window->GetRootComponent()->AddChild(child);
					window->selectedComponent = child;
					child->name = "box";
					child->shapeType = "box";
					child->SetPosition(0, 0);
                    child->SetScale(defaultWidth, defaultHeight);
                    child->InitPhysicProperty(defaultWidth, defaultHeight, -1.0f, 0.2f, 0.2f);
					child->SetSensor(true);
					return new EditProperties();
                }
            }
            if (ImGui::Button("circle"))
            {
                Mesh* mesh = Sources::Instance()->GetMesh("circle");
                if (mesh && window) {
                    Component* child = new Component(mesh);
                    window->GetRootComponent()->AddChild(child);
                    window->selectedComponent = child;
					child->name = "circle";
					child->shapeType = "circle";
                    child->SetPosition(0, 0);
                    child->SetScale(defaultWidth, defaultHeight);
					child->InitPhysicProperty((defaultWidth / 2) / 4.0f, -1.0f, 0.2f, 0.2f);
                    child->SetSensor(true);
                    return new EditProperties();
                }
            }
            if (ImGui::Button("polygon"))
            {
                Mesh* mesh = Sources::Instance()->GetMesh("box");
                if (mesh && window) {
                    float verts[] = { -50, -50, 50, -50, 0, 50 };
                    Component* child = new Component(mesh);
                    window->GetRootComponent()->AddChild(child);
                    window->selectedComponent = child;
					child->name = "polygon";
					child->shapeType = "polygon";
                    child->SetPosition(0, 0);
                    child->SetScale(defaultWidth, defaultHeight);
                    child->InitPhysicProperty(3, verts, -1.0f, 0.2f, 0.2f);
                    child->SetSensor(true);
                    return new EditProperties();
                }
			}
			return nullptr;
        }
    };
}