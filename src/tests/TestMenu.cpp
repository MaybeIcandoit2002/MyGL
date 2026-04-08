#include "TestMenu.h"

namespace test
{
	TestMenu::TestMenu(MyWindow* window)
		: window(window), nextImGuiWindowId(1), activeWindowIndex(-1), leftMenuWidth(320.0f), rightMenuWidth(420.0f), rootWindowExtraRenderer(nullptr), rootWindowExtraRendererData(nullptr)
	{
	}

	void TestMenu::CloseAllSubWindows()
	{
		for (auto& opened : openWindows)
		{
			opened.open = false;
			opened.focusNextFrame = false;
		}
		activeWindowIndex = -1;
	}

	TestMenu::OpenWindowEntry* TestMenu::GetActiveWindow()
	{
		if (activeWindowIndex < 0 || activeWindowIndex >= static_cast<std::int64_t>(openWindows.size()))
			return nullptr;

		OpenWindowEntry& opened = openWindows[static_cast<size_t>(activeWindowIndex)];
		if (!opened.open)
			return nullptr;

		return &opened;
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
		for (size_t i = 0; i < openWindows.size(); ++i)
		{
			auto& opened = openWindows[i];
			if (opened.name == name)
			{
				CloseAllSubWindows();
				opened.open = true;
				opened.focusNextFrame = true;
				activeWindowIndex = static_cast<std::int64_t>(i);
				return;
			}
		}

		for (auto& opened : openWindows)
		{
			opened.open = false;
			opened.focusNextFrame = false;
		}

		for (const auto& test : tests)
		{
			if (test.name == name)
			{
				openWindows.push_back({ test.name, test.factory(), true, true, nextImGuiWindowId++ });
				activeWindowIndex = static_cast<std::int64_t>(openWindows.size() - 1);
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
				if (activeWindowIndex == static_cast<std::int64_t>(i))
					activeWindowIndex = -1;
				return;
			}
		}
	}

	void TestMenu::SetRootWindowExtraRenderer(RootWindowExtraRenderer renderer, void* userData)
	{
		rootWindowExtraRenderer = renderer;
		rootWindowExtraRendererData = userData;
	}

	Test* TestMenu::OnImGuiRender(MyWindow* window)
	{
		ImGuiIO& io = ImGui::GetIO();
		const float viewW = io.DisplaySize.x;
		const float viewH = io.DisplaySize.y;
		const float pad = 8.0f;
		const float gap = 8.0f;
		const float minLeft = 240.0f;
		const float minRight = 300.0f;
		const float minWorkArea = 220.0f;

		if (leftMenuWidth < minLeft) leftMenuWidth = minLeft;
		if (rightMenuWidth < minRight) rightMenuWidth = minRight;

		const float maxLeft = viewW - (pad * 2.0f + gap + rightMenuWidth + minWorkArea);
		if (leftMenuWidth > maxLeft)
			leftMenuWidth = maxLeft > minLeft ? maxLeft : minLeft;

		ImGui::SetNextWindowBgAlpha(0.96f);
		ImGui::SetNextWindowPos(ImVec2(pad, pad), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(leftMenuWidth, viewH - pad * 2.0f), ImGuiCond_Always);
		ImGui::Begin("##MainMenuRoot", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove);
		leftMenuWidth = ImGui::GetWindowSize().x;

		ImGui::TextColored(ImVec4(0.62f, 0.84f, 1.00f, 1.00f), "Main Menu");
		{
			ImVec2 p0 = ImGui::GetCursorScreenPos();
			ImDrawList* dl = ImGui::GetWindowDrawList();
			dl->AddRectFilled(p0, ImVec2(p0.x + ImGui::GetContentRegionAvail().x, p0.y + 3.0f), IM_COL32(80, 180, 255, 160), 2.0f);
			ImGui::Dummy(ImVec2(0.0f, 4.0f));
		}
		ImGui::Separator();

		OpenWindowEntry* active = GetActiveWindow();
		if (active)
		{
			ImGui::Text("Current: %s", active->name.c_str());
			if (ImGui::Button("Close Current"))
			{
				active->open = false;
				activeWindowIndex = -1;
				active = nullptr;
			}
			ImGui::Separator();
		}

		ImGui::TextColored(ImVec4(0.78f, 0.90f, 0.60f, 1.00f), "Common (Pinned)");
		if (rootWindowExtraRenderer)
		{
			rootWindowExtraRenderer(window, rootWindowExtraRendererData);
		}

		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.95f, 0.82f, 0.58f, 1.00f), "Workspace Tools");
		const float lineH = ImGui::GetTextLineHeightWithSpacing();
		int workspaceCount = 0;
		for (const auto& test : tests)
		{
			if (test.showInRoot && test.name != "Setting") ++workspaceCount;
		}
		const float workspaceHeight = (workspaceCount > 0) ? (workspaceCount * lineH + 12.0f) : (lineH + 12.0f);
		ImGui::BeginChild("##WorkspaceTools", ImVec2(0.0f, workspaceHeight), true);
		for (const auto& test : tests)
		{
			if (!test.showInRoot) continue;
			if (test.name == "Setting") continue;

			bool selected = false;
			if (active)
				selected = (active->name == test.name);

			if (ImGui::Selectable(test.name.c_str(), selected))
			{
				OpenWindow(test.name);
			}
		}
		ImGui::EndChild();

		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.88f, 0.72f, 0.93f, 1.00f), "System");
		int systemCount = 0;
		for (const auto& test : tests)
		{
			if (test.showInRoot && test.name == "Setting") ++systemCount;
		}
		const float systemHeight = (systemCount > 0) ? (systemCount * lineH + 12.0f) : (lineH + 12.0f);
		ImGui::BeginChild("##SystemTools", ImVec2(0.0f, systemHeight), true);
		for (const auto& test : tests)
		{
			if (!test.showInRoot) continue;
			if (test.name != "Setting") continue;

			bool selected = false;
			if (active)
				selected = (active->name == test.name);

			if (ImGui::Selectable(test.name.c_str(), selected))
			{
				OpenWindow(test.name);
			}
		}
		ImGui::EndChild();
		ImGui::End();

		active = GetActiveWindow();
		if (active)
		{
			const float maxRight = viewW - (pad * 2.0f + gap + leftMenuWidth + minWorkArea);
			if (rightMenuWidth > maxRight)
				rightMenuWidth = maxRight > minRight ? maxRight : minRight;

			if (active->focusNextFrame)
			{
				ImGui::SetNextWindowFocus();
				active->focusNextFrame = false;
			}

			ImGui::SetNextWindowBgAlpha(0.97f);
			const float rightW = rightMenuWidth;
			const float rightX = viewW - pad - rightW;
			ImGui::SetNextWindowPos(ImVec2(rightX, pad), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImVec2(rightW, viewH - pad * 2.0f), ImGuiCond_Always);

			std::string title = active->name + "###Panel_" + std::to_string(active->imguiWindowId);
			if (ImGui::Begin(title.c_str(), &active->open,
				ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings |
				ImGuiWindowFlags_NoMove))
			{
				rightMenuWidth = ImGui::GetWindowSize().x;
				{
					ImVec2 p0 = ImGui::GetCursorScreenPos();
					ImDrawList* dl = ImGui::GetWindowDrawList();
					dl->AddRectFilled(p0, ImVec2(p0.x + ImGui::GetContentRegionAvail().x, p0.y + 3.0f), IM_COL32(160, 120, 255, 140), 2.0f);
					ImGui::Dummy(ImVec2(0.0f, 4.0f));
				}
				active->instance->OnUpdate(nullptr, 0.0f);
				active->instance->OnRender();
				active->instance->OnImGuiRender(window);

				const float gripSize = 16.0f;
				ImVec2 winSize = ImGui::GetWindowSize();
				ImGui::SetCursorPos(ImVec2(0.0f, winSize.y - gripSize));
				ImGui::PushID("RightPanelLeftBottomResizeGrip");
				ImGui::InvisibleButton("##ResizeGrip", ImVec2(gripSize, gripSize));
				ImDrawList* dl = ImGui::GetWindowDrawList();
				ImVec2 base = ImGui::GetItemRectMin();
				dl->AddLine(ImVec2(base.x + 4.0f, base.y + gripSize - 4.0f), ImVec2(base.x + gripSize - 4.0f, base.y + 4.0f), IM_COL32(170, 200, 255, 200), 1.5f);
				dl->AddLine(ImVec2(base.x + 4.0f, base.y + gripSize - 8.0f), ImVec2(base.x + gripSize - 8.0f, base.y + 4.0f), IM_COL32(170, 200, 255, 170), 1.2f);
				dl->AddLine(ImVec2(base.x + 8.0f, base.y + gripSize - 4.0f), ImVec2(base.x + gripSize - 4.0f, base.y + 8.0f), IM_COL32(170, 200, 255, 170), 1.2f);
				if (ImGui::IsItemActive())
				{
					rightMenuWidth -= ImGui::GetIO().MouseDelta.x;
					if (rightMenuWidth < minRight) rightMenuWidth = minRight;
					if (rightMenuWidth > maxRight) rightMenuWidth = maxRight;
				}
				ImGui::PopID();
			}
			if (!active->open)
				activeWindowIndex = -1;
			ImGui::End();
		}
		return nullptr;
	}
}