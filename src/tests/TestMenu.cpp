#include "TestMenu.h"

namespace test
{
	TestMenu::TestMenu(MyWindow* window)
		: window(window), nextImGuiWindowId(1)
	{
	}

	TestMenu::~TestMenu()
	{
		for (auto& opened : openWindows)
		{
			delete opened.instance;
		}
		tests.clear();
		openWindows.clear();
	}

	void TestMenu::OpenWindow(const std::string& name)
	{
		for (auto& opened : openWindows)
		{
			if (opened.name == name)
			{
				opened.open = true;
				opened.focusNextFrame = true;
				return;
			}
		}

		for (const auto& test : tests)
		{
			if (test.name == name)
			{
				openWindows.push_back({ test.name, test.factory(), true, true, nextImGuiWindowId++ });
				return;
			}
		}
	}

	void TestMenu::CloseWindow(const std::string& name)
	{
		for (size_t i = 0; i < openWindows.size(); ++i)
		{
			if (openWindows[i].name == name)
			{
				openWindows[i].open = false;
				openWindows[i].focusNextFrame = false;
				return;
			}
		}
	}

	Test* TestMenu::OnImGuiRender(MyWindow* window)
	{
		ImGui::Begin("Main Menu");
		ImGui::Text("Open Panels");
		for (const auto& test : tests)
		{
			if (!test.showInRoot) continue;
			if (ImGui::Button(test.name.c_str()))
			{
				OpenWindow(test.name);
			}
		}
		ImGui::End();

		for (size_t i = 0; i < openWindows.size(); ++i)
		{
			auto& opened = openWindows[i];
			if (!opened.open)
				continue;
			if (opened.focusNextFrame)
			{
				ImGui::SetNextWindowFocus();
				opened.focusNextFrame = false;
			}

			std::string title = opened.name + "###Panel_" + std::to_string(opened.imguiWindowId);
			if (ImGui::Begin(title.c_str(), &opened.open))
			{
				opened.instance->OnUpdate(nullptr, 0.0f);
				opened.instance->OnRender();
				opened.instance->OnImGuiRender(window);
			}
			ImGui::End();
		}
		return nullptr;
	}
}