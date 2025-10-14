#pragma once
#include "Test.h"
#include <vector>
#include <string>
namespace test
{
	class TestMenu : public Test
	{
	private:
		Test*& currentTest;
		std::vector<std::pair<std::string, Test* (*)()>> tests;
	public:
		TestMenu(Test*& currentTestPointer);

		void OnImGuiRender() override;
		template<typename T>
		void RegisterTest(const std::string& name)
		{
			tests.push_back(std::make_pair(name, []() -> Test* { return new T(); }));
		}
	};
}

