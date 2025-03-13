#include <imgui_impl/imgui_user.h>

void ImGui::RenderSquare(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float rounding, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f);

    ImVec2 min, max;
    min = ImVec2(-0.7f, -0.7f) * r;
    max = ImVec2(+0.7f, +0.7f) * r;

    draw_list->AddRectFilled(center + min, center + max, col, rounding);
}

bool ImGui::StopButtonEx(const char* str_id, ImVec2 size, float rounding, ImGuiButtonFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const float default_size = GetFrameHeight();
    ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : -1.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    const ImU32 text_col = GetColorU32(ImGuiCol_Text);
    const float scale = ImMax(size.x, size.y) / g.FontSize;
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderSquare(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, rounding * scale, scale);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, str_id, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImGui::StopButton(const char* str_id, float rounding)
{
    float sz = GetFrameHeight();
    return StopButtonEx(str_id, ImVec2(sz, sz), rounding, ImGuiButtonFlags_None);
}

void ImGui::RenderTwoRects(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float rounding, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f);

    ImVec2 minL, maxL, minR, maxR;
    minL = ImVec2(-0.7f, -0.7f) * r;
    maxL = ImVec2(-0.200f, +0.7f) * r;
    minR = ImVec2(+0.200f, -0.7f) * r;
    maxR = ImVec2(+0.7f, +0.7f) * r;

    draw_list->AddRectFilled(center + minL, center + maxL, col, rounding);
    draw_list->AddRectFilled(center + minR, center + maxR, col, rounding);
}

bool ImGui::PauseButtonEx(const char* str_id, ImVec2 size, float rounding, ImGuiButtonFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const float default_size = GetFrameHeight();
    ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : -1.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    const ImU32 text_col = GetColorU32(ImGuiCol_Text);
    const float scale = ImMax(size.x, size.y) / g.FontSize;
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderTwoRects(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, rounding * scale, scale);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, str_id, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImGui::PauseButton(const char* str_id, float rounding)
{
    float sz = GetFrameHeight();
    return PauseButtonEx(str_id, ImVec2(sz, sz), rounding, ImGuiButtonFlags_None);
}

void ImGui::RenderArrowAndWall(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float rounding, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f);

    ImVec2 a, b, c, min, max;
    switch (dir)
    {
    case ImGuiDir_Up:
    case ImGuiDir_Down:
        if (dir == ImGuiDir_Up) r = -r;
        a = ImVec2(+0.000f, +0.200f) * r;
        b = ImVec2(-0.7f, -0.7f) * r;
        c = ImVec2(+0.7f, -0.7f) * r;

        min = ImVec2(-0.7f, +0.450f) * r;
        max = ImVec2(+0.7f, +0.7f) * r;
        break;
    case ImGuiDir_Left:
    case ImGuiDir_Right:
        if (dir == ImGuiDir_Left) r = -r;
        a = ImVec2(+0.200f, +0.000f) * r;
        b = ImVec2(-0.7f, +0.7f) * r;
        c = ImVec2(-0.7f, -0.7f) * r;

        min = ImVec2(+0.450f, -0.7f) * r;
        max = ImVec2(+0.7f, +0.7f) * r;
        break;
    case ImGuiDir_None:
    case ImGuiDir_COUNT:
        IM_ASSERT(0);
        break;
    }
    draw_list->AddTriangleFilled(center + a, center + b, center + c, col);
    draw_list->AddRectFilled(center + min, center + max, col, rounding);
}

bool ImGui::ArrowAndWallButtonEx(const char* str_id, ImGuiDir dir, ImVec2 size, float rounding, ImGuiButtonFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const float default_size = GetFrameHeight();
    ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : -1.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    const ImU32 text_col = GetColorU32(ImGuiCol_Text);
    const float scale = ImMax(size.x, size.y) / g.FontSize;
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderArrowAndWall(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, dir, rounding * scale, scale);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, str_id, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImGui::ArrowAndWallButton(const char* str_id, ImGuiDir dir, float rounding)
{
    float sz = GetFrameHeight();
    return ArrowAndWallButtonEx(str_id, dir, ImVec2(sz, sz), rounding, ImGuiButtonFlags_None);
}

void ImGui::RenderBin(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    const float rounding = 1.f * scale;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f);

    ImVec2 min, max;
    min = ImVec2(-0.55f, -0.4f) * r;
    max = ImVec2(+0.55f, +0.7f) * r;

    draw_list->AddRect(center + min, center + max, col, rounding, ImDrawFlags_RoundCornersBottom);

    min = ImVec2(-0.7f, -0.55f) * r;
    max = ImVec2(+0.7f, -0.4f) * r;

    draw_list->AddRect(center + min, center + max, col, rounding, ImDrawFlags_RoundCornersAll);

    min = ImVec2(-0.2f, -0.7f) * r;
    max = ImVec2(+0.2f, -0.55f) * r;

    draw_list->AddRect(center + min, center + max, col, rounding, ImDrawFlags_RoundCornersTop);

    draw_list->AddLine(center + ImVec2(0.f, -0.275f) * r, center + ImVec2(0.f, +0.55f) * r, col);

    draw_list->AddLine(center + ImVec2(-0.25f, -0.275f) * r, center + ImVec2(-0.25f, +0.55f) * r, col);

    draw_list->AddLine(center + ImVec2(+0.25f, -0.275f) * r, center + ImVec2(+0.25f, +0.55f) * r, col);
}

bool ImGui::RemoveButtonEx(const char* str_id, ImVec2 size, ImGuiButtonFlags flags)
{
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    const ImGuiID id = window->GetID(str_id);
    const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
    const float default_size = GetFrameHeight();
    ItemSize(size, (size.y >= default_size) ? g.Style.FramePadding.y : -1.0f);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

    // Render
    const ImU32 bg_col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
    const ImU32 text_col = GetColorU32(ImGuiCol_Text);
    const float scale = ImMax(size.x, size.y) / g.FontSize;
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderBin(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, scale);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, str_id, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImGui::RemoveButton(const char* str_id)
{
    float sz = GetFrameHeight();
    return RemoveButtonEx(str_id, ImVec2(sz, sz), ImGuiButtonFlags_None);
}

const ImWchar* ImGui::GetGlyphRangesPolish()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0100, 0x017F, // Polish + Polish Supplement
        0,
    };
    return &ranges[0];
}

bool ImGui::DragUInt(const char* label, unsigned int* v, float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalar(label, ImGuiDataType_U32, v, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragUInt2(const char* label, unsigned int v[2], float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_U32, v, 2, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragUInt3(const char* label, unsigned int v[3], float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_U32, v, 3, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragUInt4(const char* label, unsigned int v[4], float v_speed, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_U32, v, 4, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDouble(const char* label, double* v, float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalar(label, ImGuiDataType_Double, v, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDouble2(const char* label, double v[2], float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Double, v, 2, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDouble3(const char* label, double v[3], float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Double, v, 3, v_speed, &v_min, &v_max, format, flags);
}

bool ImGui::DragDouble4(const char* label, double v[4], float v_speed, double v_min, double v_max, const char* format, ImGuiSliderFlags flags)
{
    return DragScalarN(label, ImGuiDataType_Double, v, 4, v_speed, &v_min, &v_max, format, flags);
}