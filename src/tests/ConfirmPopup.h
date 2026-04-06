#pragma once

#include "../vendor/imgui/imgui.h"

namespace test
{
	inline int RenderCenteredConfirmModal(const char* popupId, const char* message, bool& openRequested)
	{
		if (openRequested)
		{
			ImGui::OpenPopup(popupId);
			openRequested = false;
		}

		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		int result = 0;
		if (ImGui::BeginPopupModal(popupId, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
		{
			ImGui::TextWrapped("%s", message);
			ImGui::Separator();
			if (ImGui::Button("Confirm", ImVec2(120.0f, 0.0f)))
			{
				result = 1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f)))
			{
				result = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		return result;
	}
}
