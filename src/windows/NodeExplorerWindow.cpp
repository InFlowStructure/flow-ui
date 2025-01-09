#include "NodeExplorerWindow.hpp"

#include "Config.hpp"

#include <hello_imgui/hello_imgui.h>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui.h>
#include <imgui_node_editor.h>
#include <imgui_stdlib.h>

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

NodeExplorerWindow::NodeExplorerWindow(std::shared_ptr<Env> env) : Window("Node Selection"), _env(std::move(env)) {}

void NodeExplorerWindow::Draw()
{
    if (!_active_graph)
    {
        return Window::Draw();
    }

    ImGui::BeginHorizontal("search");

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 20.f);
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(36, 36, 36, 255));

    ImGui::SetNextItemAllowOverlap();

    ImGui::PushItemWidth(ImGui::GetWindowWidth() - (ImGui::GetStyle().WindowPadding.x * 2));
    ImGui::InputText("##Search", &node_lookup, 0);
    ImGui::PopItemWidth();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::SetCursorPosX((ImGui::GetItemRectMax() - ImGui::GetItemRectMin()).x - 18);

    ImGui::PushFont(std::bit_cast<ImFont*>(GetConfig().IconFont.get()));
    ImGui::TextUnformatted(ICON_FA_MAGNIFYING_GLASS);
    ImGui::PopFont();

    ImGui::EndHorizontal();

    auto factory = _env->GetFactory();

    auto registered_nodes = factory->GetCategories();

    if (!node_lookup.empty())
    {
        auto partial_match_func = [&](const auto& entry) -> bool {
            auto [_, class_name]     = entry;
            std::string display_name = factory->GetFriendlyName(class_name);
            std::string filter       = node_lookup;

            const auto& to_lower = [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); };
            std::transform(filter.begin(), filter.end(), filter.begin(), to_lower);
            std::transform(class_name.begin(), class_name.end(), class_name.begin(), to_lower);
            std::transform(display_name.begin(), display_name.end(), display_name.begin(), to_lower);

            return std::string_view(display_name).find(filter) == std::string_view::npos &&
                   std::string_view(class_name).find(filter) == std::string_view::npos;
        };

        std::erase_if(registered_nodes, partial_match_func);
    }

    if (ImGui::BeginChild("Categories"))
    {
        std::set<std::string> categories;
        for (const auto& [category, _] : registered_nodes)
        {
            categories.insert(category);
        }

        for (const auto& category : categories)
        {
            DrawPopupCategory(category, registered_nodes);
        }

        ImGui::EndChild();
    }
}

void NodeExplorerWindow::DrawPopupCategory(const std::string& category, const flow::CategoryMap& registered_nodes)
{
    if (!ImGui::TreeNodeEx(category.c_str(), node_lookup.empty() ? 0 : ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto [begin_it, end_it] = registered_nodes.equal_range(category);
    for (auto it = begin_it; it != end_it; ++it)
    {
        const auto display_name = _env->GetFactory()->GetFriendlyName(it->second);

        ImGui::Bullet();
        ImGui::Selectable(display_name.c_str());

        if (ImGui::BeginDragDropSource())
        {
            const auto class_name = it->second;
            ImGui::Text("+ Create Node");
            ImGui::SetDragDropPayload("NewNode", class_name.c_str(), class_name.size() + 1, ImGuiCond_Once);
            ImGui::EndDragDropSource();
        }
    }

    ImGui::TreePop();
}

FLOW_UI_NAMESPACE_END
