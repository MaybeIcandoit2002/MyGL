#pragma once
#include "Components/Layouts.h"
#include "MyWindow.h"

namespace DefaultLayouts
{
	static MyWindow* window = nullptr;
	static Renderer* renderer = nullptr;
	static PhysicWorld* physicWorld = nullptr;
	static Layouts* toolsComponent = nullptr;

	void Initialized(const std::string name, float width, float height)
	{
		window = new MyWindow(width, height, name.c_str());
		renderer = window->GetRenderer();
		physicWorld = window->GetPhysicWorld();

		Sources::Instance()->LoadMeshsFromJson("res/meshs/default.json", renderer);

		window->SetClearColor(0.9f, 0.9f, 0.9f, 1.0f);
#pragma region tools
		toolsComponent = new Layouts(utils::CopyMesh(
			"tools", Sources::Instance()->GetMesh("box"),
			{
				glm::vec2(0.1 * width, height),
				glm::vec4(0.0f, 0.0f, 0.0f, 0.6f),
				glm::vec2(1, 1),
				-1
			},
			renderer
		));
#pragma endregion
		

		renderer->SendToGPU();
	}
}