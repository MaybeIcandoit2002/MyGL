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
            for (auto CT : Sources::Instance()->GetComponentTemplate())
            {
                if (ImGui::Button(CT.name.c_str()))
                {
                    Mesh* mesh = Sources::Instance()->GetMesh(CT.meshName);
                    if (mesh && window)
                    {
                        Component* child = new Component(mesh);
                        window->GetRootComponent()->AddChild(child);
                        window->selectedComponent = child;
                        child->name = CT.name;
                        child->shapeType = CT.shapeType;
                        child->SetTextureSlot(CT.textureSlot);
                        child->SetPosition(0, 0);
                        child->SetScale(CT.scale1, CT.scale2);
                        switch (CT.shapeType)
                        {
                        case ShapeType::Circle:
                            child->InitPhysicProperty(CT.physicSize1, CT.physicMass, CT.physicRestitution, CT.physicFriction);
							break;
                        case ShapeType::Box:
                            child->InitPhysicProperty(CT.physicSize1, CT.physicSize2[0], CT.physicMass, CT.physicRestitution, CT.physicFriction);
                            break;
                        case ShapeType::Polygon:
                            child->InitPhysicProperty(static_cast<int>(CT.physicSize1), CT.physicSize2, CT.physicMass, CT.physicRestitution, CT.physicFriction);
							break;
                        }
                        child->SetSensor(true);
                        return new EditProperties();
                    }
                }
            }

			return nullptr;
        }
    };
}