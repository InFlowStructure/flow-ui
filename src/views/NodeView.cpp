// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "NodeView.hpp"

#include "Config.hpp"
#include "ConnectionView.hpp"
#include "PortView.hpp"
#include "ViewFactory.hpp"
#include "utilities/Builders.hpp"
#include "utilities/Conversions.hpp"
#include "utilities/Widgets.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Port.hpp>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <imgui_stdlib.h>
#include <spdlog/spdlog.h>

#include <map>
#include <vector>

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

namespace
{
inline ImRect ImGui_GetItemRect() { return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()); }

inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}
} // namespace

GraphItemView::~GraphItemView()
{
    if (!ed::GetCurrentEditor())
    {
        return;
    }

    ed::BreakLinks(static_cast<ed::NodeId>(_id));
    ed::DeleteNode(_id);
}

void GraphItemView::ShowConnectables(const std::shared_ptr<PortView>&) {}

NodeView::NodeView(flow::SharedNode node, Colour header_colour)
try : GraphItemView(std::hash<flow::UUID>{}(node->ID())), Name(node->GetName()),
    HeaderColour(header_colour), Node{std::move(node)}, _builder{std::make_shared<utility::NodeBuilder>()}
{
    Node->OnCompute.Bind("ClearError", [&]() { _received_error = false; });
    Node->OnError.Bind("SetError", [&](const std::exception&) { _received_error = true; });

    auto on_input = [this](const auto& key, auto data) {
        Node->GetEnv()->AddTask([key, c = Node, d = std::move(data)] {
            std::lock_guard _(*c);
            c->SetInputData(key, std::move(d));
        });
    };

    std::vector<flow::SharedPort> sorted_ports;
    auto ins = Node->GetInputPorts();

    std::for_each(ins.begin(), ins.end(), [&](const auto p) { sorted_ports.emplace_back(p.second); });
    std::sort(sorted_ports.begin(), sorted_ports.end(), std::less<flow::SharedPort>());

    auto view_factory = std::dynamic_pointer_cast<ViewFactory>(Node->GetEnv()->GetFactory());

    Inputs.reserve(sorted_ports.size());
    for (const auto& input : sorted_ports)
    {
        auto& in = Inputs.emplace_back(std::make_shared<PortView>(_id, input, view_factory, on_input));
        in->Kind = PortType::Input;
        in->SetBuilder(_builder);
    }

    Outputs.reserve(Node->GetOutputPorts().size());
    for (const auto& [_, output] : Node->GetOutputPorts())
    {
        auto& out = Outputs.emplace_back(std::make_shared<PortView>(_id, output, view_factory, on_input));
        out->Kind = PortType::Output;
        out->SetBuilder(_builder);
    }
}
catch (const std::exception& e)
{
    SPDLOG_ERROR("Caught exception while creating node view: {0}", e.what());
}

void NodeView::Draw()
try
{
    if (_received_error)
    {
        ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(227, 36, 27));  // Red border for error state
    }

    _builder->Begin(_id);

    _builder->Header(utility::to_ImColor(HeaderColour));  // Default header color is (40, 75, 99)
    ImGui::Spring(0);
    const auto& name = Node->GetName().c_str();


    // Custom font for node headers
    if (GetConfig().NodeHeaderFont)
    {
        ImGui::PushFont(std::bit_cast<ImFont*>(GetConfig().NodeHeaderFont.get()));
        ImGui::TextUnformatted(name);
        ImGui::PopFont();
    }
    else
    {
        ImGui::TextUnformatted(name);
    }

    ImGui::Spring(1);
    ImGui::Dummy(ImVec2(0, 28));
    ImGui::Spring(0);
    _builder->EndHeader();

    try
    {
        ImGui::PushStyleColor(ImGuiCol_FrameBg,
                              ImGui::GetStyleColorVec4(ImGuiCol_FrameBg) - ImVec4(0.f, 0.f, 0.f, 25.f));
        for (auto& input : Inputs)
        {
            input->Draw();
        }

        for (auto& output : Outputs)
        {
            output->Draw();
        }
        ImGui::PopStyleColor();
    }
    catch (const std::exception& e)
    {
        SPDLOG_ERROR("Caught exception while trying to draw node '{0}': {1}", std::string(Node->ID()), e.what());
    }

    _builder->End();

    if (_received_error)
    {
        ed::PopStyleColor();
    }
}
catch (const std::exception& e)
{
    SPDLOG_ERROR("Encounter and error while trying to draw node: {0}", e.what());
}

void NodeView::ShowConnectables(const std::shared_ptr<PortView>& new_link_pin)
{
    for (auto& port : Inputs)
    {
        port->ShowConnectable(new_link_pin);
    }

    for (auto& port : Outputs)
    {
        port->ShowConnectable(new_link_pin);
    }
}

SimpleNodeView::SimpleNodeView(flow::SharedNode node) : NodeView(std::move(node))
{
    for (const auto& input : Inputs)
    {
        input->SetShowLabel(false);
    }

    for (const auto& output : Outputs)
    {
        output->SetShowLabel(false);
    }
}

void SimpleNodeView::Draw()
try
{
    const auto& name = Node->GetName().c_str();

    if (_received_error)
    {
        ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(227, 36, 27));
    }

    _builder->Begin(_id);

    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg) - ImVec4(0.f, 0.f, 0.f, 25.f));

    try
    {
        for (auto& input : Inputs)
        {
            input->Draw();
        }

        _builder->Middle();

        const ImVec2 text_size = ImGui::CalcTextSize(name) * 2.f;
        ImGui::Dummy(text_size);

        for (auto& output : Outputs)
        {
            output->Draw();
        }
    }
    catch (const std::exception& e)
    {
        SPDLOG_ERROR("Caught exception while trying to draw node '{0}': {1}", std::string(Node->ID()), e.what());
    }

    ImGui::PopStyleColor();

    _builder->End();

    auto draw_list         = ed::GetNodeBackgroundDrawList(_id);
    const ImVec2 text_size = ImGui::CalcTextSize(name) * 2.f;
    draw_list->AddText(std::bit_cast<ImFont*>(GetConfig().NodeHeaderFont.get()), 40.f,
                       _builder->GetMinPos() - (text_size / 2) + (_builder->GetSize() / 2),
                       IM_COL32(180, 180, 180, 255), name);

    if (_received_error)
    {
        ed::PopStyleColor();
    }
}
catch (const std::exception& e)
{
    SPDLOG_ERROR("Encounter and error while trying to draw node: {0}", e.what());
}

CommentView::CommentView(CommentSize size, std::string_view name)
    : GraphItemView(std::hash<flow::UUID>{}(flow::UUID{})), Name{name}, Size{size}
{
}

void CommentView::Draw()
{
    const float commentAlpha = 0.75f;  // Semi-transparent

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
    ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(0, 0, 0, 64));      // Dark semi-transparent background
    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(0, 0, 0, 64));  // Dark semi-transparent border
    ed::PushStyleVar(ed::StyleVar_NodeRounding, 0.f);                     // No rounding
    ed::PushStyleVar(ed::StyleVar_NodeBorderWidth, 0.f);                 // No border
    ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(0, 5, 0, 0));
    ed::PushStyleVar(ed::StyleVar_GroupRounding, 0.f);

    ed::BeginNode(ID());

    ImVec2 HeaderMin(0.f, 0.f);
    ImVec2 HeaderMax(0.f, 0.f);

    ImGui::PushID(std::bit_cast<void*>(ID()));
    auto cursor_pos_x = ImGui::GetCursorPosX();
    ImGui::BeginVertical("content");
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
        ImGui::BeginHorizontal("horizontal");
        {
            ImGui::PushFont(std::bit_cast<ImFont*>(GetConfig().NodeHeaderFont.get()));
            ImGui::Dummy(ImVec2(5.f, 0));

            if (_edit)
            {
                ImGui::PushItemWidth(std::min(Size.Width, ImGui::CalcTextSize(Name.c_str()).x));
                if (ImGui::InputText("", &Name,
                                     ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    _edit = false;
                }
                ImGui::PopItemWidth();
            }
            else
            {
                ImGui::PushTextWrapPos(cursor_pos_x + Size.Width * 0.99f);
                ImGui::TextWrapped("%s", Name.c_str());
                ImGui::PopTextWrapPos();
            }

            ImGui::PopFont();
        }

        ImGui::EndHorizontal();

        if (ImGui::IsItemClicked())
        {
            _edit = true;
        }

        ImGui::PopStyleVar();

        HeaderMin = ImGui::GetItemRectMin();
        HeaderMax = ImGui::GetItemRectMax();

        ed::Group(ImVec2{Size.Width, Size.Height});
    }
    ImGui::EndVertical();
    Size.Width = (ImGui::GetItemRectMax() - ImGui::GetItemRectMin()).x;
    ImGui::PopID();

    ed::EndNode();

    if (!ed::IsNodeSelected(ID()) && _edit)
    {
        _edit = false;
    }

    auto draw_list = ed::GetNodeBackgroundDrawList(ID());
    draw_list->AddRectFilled(HeaderMin - ImVec2(0.f, 5.f), HeaderMax + ImVec2(0.f, 5.f), ImColor(0, 0, 0, 255));

    ed::PopStyleColor(2);
    ed::PopStyleVar(4);
    ImGui::PopStyleVar();

    if (ed::BeginGroupHint(ID()))
    {
        auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

        auto min = ed::GetGroupMin();

        ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
        ImGui::BeginGroup();
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + Size.Width);
        ImGui::TextWrapped("%s", Name.c_str());
        ImGui::PopTextWrapPos();
        ImGui::EndGroup();

        auto drawList = ed::GetHintBackgroundDrawList();

        auto hintBounds      = ImGui_GetItemRect();
        auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 1);

        drawList->AddRectFilled(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
                                IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 0.f);

        drawList->AddRect(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
                          IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 0.f);
    }
    ed::EndGroupHint();
}

FLOW_UI_NAMESPACE_END
