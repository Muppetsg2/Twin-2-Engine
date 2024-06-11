#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_INCLUDE_IMGUI_USER_CPP

#include <imgui.h>
#include <imgui_internal.h>

#include <imgui_impl/IconsFontAwesome6.h>

namespace ImGui {
	IMGUI_API void          RenderSquare(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float rounding = 0.0f, float scale = 1.0f);
	IMGUI_API bool          StopButtonEx(const char* str_id, ImVec2 size_arg, float rounding = 0.0f, ImGuiButtonFlags flags = 0);
	IMGUI_API bool          StopButton(const char* str_id, float rounding = 0.0f);							// stop button with an square shape

	IMGUI_API void          RenderTwoRects(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float rounding = 0.0f, float scale = 1.0f);
	IMGUI_API bool          PauseButtonEx(const char* str_id, ImVec2 size_arg, float rounding = 0.0f, ImGuiButtonFlags flags = 0);
	IMGUI_API bool          PauseButton(const char* str_id, float rounding = 0.0f);							// pause button with an two rects shape

	IMGUI_API void			RenderArrowAndWall(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float rounding = 0.0f, float scale = 1.0f);
	IMGUI_API bool          ArrowAndWallButtonEx(const char* str_id, ImGuiDir dir, ImVec2 size, float rounding = 0.0f, ImGuiButtonFlags flags = 0);
	IMGUI_API bool          ArrowAndWallButton(const char* str_id, ImGuiDir dir, float rounding = 0.0f);	// arrow and wall button with an arrow and wall shapes

	IMGUI_API void			RenderBin(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float scale = 1.0f);
	IMGUI_API bool          RemoveButtonEx(const char* str_id, ImVec2 size, ImGuiButtonFlags flags = 0);
	IMGUI_API bool          RemoveButton(const char* str_id);	// remove button with an bin shape

	const ImWchar* GetGlyphRangesPolish();

	// DragUInt
	IMGUI_API bool          DragUInt(const char* label, unsigned int* v, float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);  // If v_min >= v_max we have no bound
	IMGUI_API bool          DragUInt2(const char* label, unsigned int v[2], float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
	IMGUI_API bool          DragUInt3(const char* label, unsigned int v[3], float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
	IMGUI_API bool          DragUInt4(const char* label, unsigned int v[4], float v_speed = 1.0f, unsigned int v_min = 0, unsigned int v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);

	// DragDouble
	IMGUI_API bool          DragDouble(const char* label, double* v, float v_speed = 1.0f, double v_min = 0, double v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);  // If v_min >= v_max we have no bound
	IMGUI_API bool          DragDouble2(const char* label, double v[2], float v_speed = 1.0f, double v_min = 0, double v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
	IMGUI_API bool          DragDouble3(const char* label, double v[3], float v_speed = 1.0f, double v_min = 0, double v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);
	IMGUI_API bool          DragDouble4(const char* label, double v[4], float v_speed = 1.0f, double v_min = 0, double v_max = 0, const char* format = "%d", ImGuiSliderFlags flags = 0);

	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(" ICON_FA_QUESTION ")");
		if (ImGui::BeginItemTooltip())
		{
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}
}