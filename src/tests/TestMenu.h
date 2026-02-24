#pragma once
#include "Test.h"
#include <vector>
#include <string>
namespace test
{
	class TestMenu : public Test
	{
	private:
		std::vector<std::pair<std::string, Test* (*)()>> tests;
	public:
		MyWindow* window;
		TestMenu(MyWindow* window);

		Test* OnImGuiRender(MyWindow* window) override;
		template<typename T>
		void RegisterTest(const std::string& name)
		{
			tests.push_back(std::make_pair(name, []() -> Test* { return new T(); }));
		}
	};
}

