#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_INCLUDE_IMGUI_USER_CPP

#include <imgui.h>
#include <imgui_internal.h>

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

	IMGUI_API bool          SliderScalarv2(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* display = NULL, ImGuiSliderFlags flags = 0);
	IMGUI_API bool          SliderFloatv2(const char* label, float* v, float v_min, float v_max, const char* display = "", ImGuiSliderFlags flags = 0);
}