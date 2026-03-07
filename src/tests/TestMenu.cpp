#include "TestMenu.h"

namespace test
{
	TestMenu::TestMenu(MyWindow* window)
		:window(window)
	{
	}
	Test* TestMenu::OnImGuiRender(MyWindow* window)
	{
		for (auto& test : tests)
		{
			if (ImGui::Button(test.first.c_str()))
			{
				return test.second();
			}
		}
		return nullptr;
	}
}