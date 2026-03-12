#pragma once

#include "Test.h"
#include <vector>
#include <string>
#include <cstdint>
namespace test
{
	class TestMenu : public Test
	{
	private:
		struct TestFactoryEntry
		{
			std::string name;
			Test* (*factory)();
			bool showInRoot;
		};
		struct OpenWindowEntry
		{
			std::string name;
			Test* instance;
			bool open;
			bool focusNextFrame;
			std::uint64_t imguiWindowId;
		};
		std::vector<TestFactoryEntry> tests;
		std::vector<OpenWindowEntry> openWindows;
		std::uint64_t nextImGuiWindowId;
	public:
		MyWindow* window;
		TestMenu(MyWindow* window);
		~TestMenu() override;

		Test* OnImGuiRender(MyWindow* window) override;
		void OpenWindow(const std::string& name);
		void CloseWindow(const std::string& name);
		template<typename T>
		void RegisterTest(const std::string& name)
		{
			tests.push_back({ name, []() -> Test* { return new T(); }, true });
		}
		template<typename T>
		void RegisterHiddenTest(const std::string& name)
		{
			tests.push_back({ name, []() -> Test* { return new T(); }, false });
		}
	};
}

