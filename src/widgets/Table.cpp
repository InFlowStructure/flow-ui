#include "Table.hpp"

#include <imgui.h>

FLOW_UI_SUBNAMESPACE_START(widgets)

Table::Table(const std::string& name, std::size_t columns, std::size_t outer_width, std::size_t outer_height)
    : _name(name), _columns(columns), _outer_width(outer_width), _outer_height(outer_height)
{
}

void Table::operator()() noexcept
{
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(15.f, 5.f));
    ImGui::BeginTable(_name.c_str(), static_cast<int>(_columns), ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg,
                      ImVec2{static_cast<float>(_outer_width), static_cast<float>(_outer_height)});

    for (const auto& widget : _widgets)
    {
        if (!widget) continue;

        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        (*widget)();
    }

    ImGui::EndTable();
    ImGui::PopStyleVar();
}

void Table::AddEntry(std::shared_ptr<Widget> widget) { _widgets.emplace_back(std::move(widget)); }

FLOW_UI_SUBNAMESPACE_END
