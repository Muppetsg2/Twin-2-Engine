#include <imgui_impl/imgui_user.h>

// Render an arrow aimed to be aligned with text (p_min is a position in the same space text would be positioned). To e.g. denote expanded/collapsed state
void ImGui::RenderSquare(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float rounding, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);

    ImVec2 min, max;
    min = ImVec2(-0.866f, -0.866f) * r;
    max = ImVec2(+0.866f, +0.866f) * r;

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
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderSquare(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, rounding);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, str_id, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImGui::StopButton(const char* str_id, float rounding)
{
    float sz = GetFrameHeight();
    return StopButtonEx(str_id, ImVec2(sz, sz), rounding, ImGuiButtonFlags_None);
}

// Render an arrow aimed to be aligned with text (p_min is a position in the same space text would be positioned). To e.g. denote expanded/collapsed state
void ImGui::RenderTwoRects(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float rounding, float scale)
{
    const float h = draw_list->_Data->FontSize * 1.00f;
    float r = h * 0.40f * scale;
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);

    ImVec2 minL, maxL, minR, maxR;
    minL = ImVec2(-0.866f, -0.866f) * r;
    maxL = ImVec2(-0.200f, +0.866f) * r;
    minR = ImVec2(+0.200f, -0.866f) * r;
    maxR = ImVec2(+0.866f, +0.866f) * r;

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
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderTwoRects(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, rounding);

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
    ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);

    ImVec2 a, b, c, min, max;
    switch (dir)
    {
    case ImGuiDir_Up:
    case ImGuiDir_Down:
        if (dir == ImGuiDir_Up) r = -r;
        a = ImVec2(+0.000f, +0.300f) * r;
        b = ImVec2(-0.866f, -0.866f) * r;
        c = ImVec2(+0.866f, -0.866f) * r;

        min = ImVec2(-0.866f, +0.650f) * r;
        max = ImVec2(+0.866f, +0.866f) * r;
        break;
    case ImGuiDir_Left:
    case ImGuiDir_Right:
        if (dir == ImGuiDir_Left) r = -r;
        a = ImVec2(+0.300f, +0.000f) * r;
        b = ImVec2(-0.866f, +0.866f) * r;
        c = ImVec2(-0.866f, -0.866f) * r;

        min = ImVec2(+0.650f, -0.866f) * r;
        max = ImVec2(+0.866f, +0.866f) * r;
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
    RenderNavHighlight(bb, id);
    RenderFrame(bb.Min, bb.Max, bg_col, true, g.Style.FrameRounding);
    RenderArrowAndWall(window->DrawList, bb.Min + ImVec2(ImMax(0.0f, (size.x - g.FontSize) * 0.5f), ImMax(0.0f, (size.y - g.FontSize) * 0.5f)), text_col, dir, rounding);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, str_id, g.LastItemData.StatusFlags);
    return pressed;
}

bool ImGui::ArrowAndWallButton(const char* str_id, ImGuiDir dir, float rounding)
{
    float sz = GetFrameHeight();
    return ArrowAndWallButtonEx(str_id, dir, ImVec2(sz, sz), rounding, ImGuiButtonFlags_None);
}