#include "PropertyTree.hpp"
#include "Table.hpp"

#include <imgui.h>

FLOW_UI_SUBNAMESPACE_START(widgets)

PropertyTree::PropertyTree(const std::string& name, std::size_t columns) : _name(name), _columns(columns) {}

void PropertyTree::AddProperty(const std::string& name, std::shared_ptr<Widget> widget,
                               const std::string& category_name)
{
    _properties[category_name][name] = {std::move(widget)};
}

void PropertyTree::AddProperty(const std::string& name, const std::vector<std::shared_ptr<Widget>>& widgets,
                               const std::string& category_name)
{
    _properties[category_name][name] = widgets;
}

void PropertyTree::operator()() noexcept
{
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 1.f));

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, 10.f));

    float old_font_size     = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale = 1.2f;
    ImGui::PushFont(ImGui::GetFont());

    if (!ImGui::TreeNodeEx(_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                              ImGuiTreeNodeFlags_FramePadding))
    {
        ImGui::GetFont()->Scale = old_font_size;
        ImGui::PopFont();
        ImGui::PopStyleVar(5);
        return;
    }

    ImGui::GetFont()->Scale = old_font_size;
    ImGui::PopFont();
    ImGui::PopStyleVar();

    for (const auto& [category_name, properties] : _properties)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.f, 10.f));
        if (!ImGui::TreeNodeEx(category_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                          ImGuiTreeNodeFlags_FramePadding))
        {
            ImGui::PopStyleVar();
            continue;
        }

        for (const auto& [name, widgets] : properties)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15.f, 5.f));
            if (!ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                     ImGuiTreeNodeFlags_FramePadding))
            {
                ImGui::PopStyleVar();
                continue;
            }

            auto property_table = Table(category_name + "##table", _columns);
            for (const auto& widget : widgets)
            {
                property_table.AddEntry(widget);
            }

            ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(5.f, 1.f));
            property_table();

            ImGui::TreePop();
            ImGui::PopStyleVar();
        }

        ImGui::TreePop();
        ImGui::PopStyleVar();
    }

    ImGui::TreePop();
    ImGui::PopStyleVar(4);
}

FLOW_UI_SUBNAMESPACE_END
