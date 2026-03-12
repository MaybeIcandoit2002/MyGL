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
            Renderer* renderer = window ? window->GetRenderer() : nullptr;

            const float buttonSize = 56.0f;
            const ImVec2 buttonSizeVec(buttonSize, buttonSize);
            const float spacing = ImGui::GetStyle().ItemSpacing.x;
            const float availWidth = ImGui::GetContentRegionAvail().x;
            const int columns = (buttonSize + spacing > 0.0f)
                ? static_cast<int>((availWidth + spacing) / (buttonSize + spacing))
                : 1;
            const int rowCount = columns > 0 ? columns : 1;

            int templateIndex = 0;
            const auto& templates = Sources::Instance()->GetComponentTemplate();
            for (size_t i = 0; i < templates.size(); ++i)
            {
                const auto& CT = templates[i];
                bool clicked = false;
                ImGui::PushID(templateIndex++);

                const uint32_t textureId = (renderer && CT.textureSlot >= 0)
                    ? renderer->GetTextureNativeId(static_cast<uint32_t>(CT.textureSlot))
                    : 0;

                if (textureId != 0)
                {
                    clicked = ImGui::ImageButton(
                        (ImTextureID)(intptr_t)textureId,
                        buttonSizeVec,
                        ImVec2(0.0f, 1.0f),
                        ImVec2(1.0f, 0.0f));
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", CT.name.c_str());
                }
                else
                {
                    clicked = ImGui::Button(CT.name.c_str(), buttonSizeVec);
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", CT.name.c_str());
                }

                ImGui::PopID();

                if ((static_cast<int>(i) + 1) % rowCount != 0)
                    ImGui::SameLine();

                if (clicked)
                {
                    Mesh* mesh = Sources::Instance()->GetMesh(CT.meshName);
                    if (mesh && window)
                    {
                        Component* child = new Component(mesh);
                        window->GetRootComponent()->AddChild(child);
                        window->SelectComponent(child);
                        child->templateName = CT.name;
                        child->name = CT.name;
                        child->shapeType = CT.shapeType;
                        child->SetTextureSlot(CT.textureSlot);
                        child->SetPosition(0, 0);
                        child->SetScale(CT.scale1, CT.scale2);
                        child->SetBackgroundColor(CT.backgroundColor[0], CT.backgroundColor[1], CT.backgroundColor[2], CT.backgroundColor[3]);
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
                        return nullptr;
                    }
                }
            }

            return nullptr;
        }
    };
}