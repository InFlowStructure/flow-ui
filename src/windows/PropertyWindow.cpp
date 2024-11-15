// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "PropertyWindow.hpp"

#include "GraphWindow.hpp"
#include "Widgets.hpp"

#include <imgui-node-editor/imgui_node_editor.h>
#include <imgui.h>

#include <array>
#include <set>
#include <string>
#include <string_view>

using namespace ax;
namespace ed = ax::NodeEditor;

FLOW_UI_NAMESPACE_START

PropertyWindow::PropertyWindow(std::shared_ptr<flow::Env> env)
    : Window(PropertyWindow::Name, DefaultDockspaces::Property), _env{env}
{
}

struct NodeProperty
{
    std::string Name;
    flow::SharedNode Node;
};

void PropertyWindow::Draw()
{
    auto env = _env.lock();
    if (!env) return;

    auto graph = _graph.lock();
    if (!ed::GetCurrentEditor() || !graph)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(175, 175, 175, 255));
        widgets::TextCentered("Nothing to show");
        ImGui::PopStyleColor();
        return;
    }

    std::array<ed::NodeId, 256> selected_ids;
    auto result = ed::GetSelectedNodes(selected_ids.data(), 256);

    if (result == 0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(175, 175, 175, 255));
        widgets::TextCentered("Select one or more nodes");
        ImGui::PopStyleColor();
        return;
    }

    std::set<ed::NodeId> ids(selected_ids.begin(), std::next(selected_ids.begin(), result));

    std::vector<NodeProperty> nodes;
    nodes.reserve(result);

    graph->Visit([&](auto& node) {
        if (!ids.contains(std::hash<flow::UUID>{}(node->ID()))) return;
        nodes.emplace_back(NodeProperty{std::string{node->GetName()}, node});
    });

    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(38, 38, 38, 255));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 1.f));
    for (auto& node : nodes)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, 10.f));
        std::string name = std::string{node.Node->GetName()};
        if (!ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                 ImGuiTreeNodeFlags_FramePadding))
        {
            ImGui::PopStyleVar();
            continue;
        }
        ImGui::PopStyleVar();

        ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(5.f, 1.f));
        ImGui::BeginTable("Node", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg,
                          ImGui::GetItemRectSize() + ImVec2(1.f, 0.f));
        ImGui::TableNextRow();

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.f, 5.f));

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Name");
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImGui::TableNextColumn();
        if (widgets::InputText("##name", &node.Name, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            node.Node->SetName(node.Name);
        }
        ImGui::EndTable();

        ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(36, 36, 36, 255));
        if (!node.Node->GetInputPorts().empty() &&
            ImGui::TreeNodeEx("Inputs", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                            ImGuiTreeNodeFlags_FramePadding))
        {
            for (const auto& [key, input] : node.Node->GetInputPorts())
            {
                const std::string input_name = std::string{std::string_view(key)};
                if (!ImGui::TreeNodeEx(input_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                               ImGuiTreeNodeFlags_FramePadding))
                {
                    continue;
                }

                ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(5.f, 1.f));
                ImGui::BeginTable("Inputs", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg,
                                  ImGui::GetItemRectSize() + ImVec2(1.f, 0.f));

                ImGui::TableNextRow();

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Name");
                ImGui::PopStyleColor();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(input_name.c_str());

                ImGui::TableNextRow();

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Type");
                ImGui::PopStyleColor();

                ImGui::TableNextColumn();
                ImGui::TextUnformatted(std::string{input->GetDataType()}.c_str());

                if (auto data = input->GetData(); data && !data->ToString().empty())
                {
                    ImGui::TableNextRow();

                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Value");
                    ImGui::PopStyleColor();

                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(data->ToString().c_str());
                }

                ImGui::EndTable();
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        if (!node.Node->GetOutputPorts().empty() &&
            ImGui::TreeNodeEx("Outputs", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                             ImGuiTreeNodeFlags_FramePadding))
        {
            for (const auto& [key, output] : node.Node->GetOutputPorts())
            {
                const std::string output_name = std::string{std::string_view(key)};

                if (!ImGui::TreeNodeEx(output_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
                                                                ImGuiTreeNodeFlags_FramePadding))
                {
                    continue;
                }

                ImGui::SetCursorPos(ImGui::GetCursorPos() - ImVec2(5.f, 1.f));
                ImGui::BeginTable("Outputs", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg,
                                  ImGui::GetItemRectSize() + ImVec2(1.f, 0.f));

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Name");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(output_name.c_str());

                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::TextUnformatted("Type");
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(std::string{output->GetDataType()}.c_str());

                if (auto data = output->GetData(); data && !data->ToString().empty())
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted("Value");
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(data->ToString().c_str());
                }

                ImGui::EndTable();
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::PopStyleColor();

        ImGui::TreePop();
    }

    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor();
}

FLOW_UI_NAMESPACE_END
