// My modified ComboWithFilter with fts_fuzzy_match as include.
// Using dear imgui, v1.89 WIP
//
// Adds arrow/pgup/pgdn navigation, Enter to confirm, max_height_in_items, and
// fixed focus on open and avoids drawing past window edges.
// My contributions are CC0/public domain.

// Posted in issue: https://github.com/ocornut/imgui/issues/1658#issuecomment-1086193100
#pragma once
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#define FTS_FUZZY_MATCH_IMPLEMENTATION
#include <imgui_impl/fts_fuzzy_match.h>

#include <imgui_impl/IconsFontAwesome6.h>
#include <imgui_impl/IconsFontAwesome6Brands.h>

namespace ImGui
{
    static bool sortbysec_desc(const std::pair<int, int>& a, const std::pair<int, int>& b);

    static int index_of_key(std::vector<std::pair<int, int> > pair_list, int key);

    static float CalcMaxPopupHeightFromItemCount(int items_count);

    IMGUI_API bool ComboWithFilter(const char* label, size_t* current_item, const std::vector<std::string>& items, int popup_max_height_in_items /*= -1 */);

    IMGUI_API bool ComboWithFilter(const char* label, int* current_item, const std::vector<std::string>& items, int popup_max_height_in_items /*= -1 */);
}