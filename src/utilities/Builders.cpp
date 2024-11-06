// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Builders.hpp"

#include <imgui.h>
#include <imgui_internal.h>

FLOW_UI_NAMESPACE_START

void NodeBuilder::Begin(ed::NodeId id)
{
    _has_header = false;
    _header_min = _header_max = ImVec2();

    ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));

    ed::BeginNode(id);

    ImGui::PushID(id.AsPointer());
    _current_node_id = id;

    SetStage(Stage::Begin);
}

void NodeBuilder::End()
{
    SetStage(Stage::End);

    ed::EndNode();

    if (ImGui::IsItemVisible())
    {
        auto drawList = ed::GetNodeBackgroundDrawList(_current_node_id);

        if ((_header_max.x > _header_min.x) && (_header_max.y > _header_min.y))
        {
            const auto uv = ImVec2((_header_max.x - _header_min.x), (_header_max.y - _header_min.y));

            drawList->AddRectFilled(_header_min - ImVec2(7, 3), _header_max + ImVec2(7, 0), _header_color,
                                    ed::GetStyle().NodeRounding - 1, ImDrawFlags_RoundCornersTop);
        }
    }

    _current_node_id = 0;

    ImGui::PopID();

    ed::PopStyleVar();

    SetStage(Stage::Invalid);
}

void NodeBuilder::Header(const ImVec4& color)
{
    _header_color = ImColor(color);
    SetStage(Stage::Header);
}

void NodeBuilder::EndHeader() { SetStage(Stage::Content); }

void NodeBuilder::Port(ed::PinId id)
{
    if (_current_stage == Stage::Begin)
    {
        SetStage(Stage::Content);
    }

    const auto apply_padding = (_current_stage == Stage::Input);

    SetStage(Stage::Input);

    if (apply_padding) ImGui::Spring(0);

    Pin(id, ed::PinKind::Input);

    ImGui::BeginHorizontal(id.AsPointer());
}

void NodeBuilder::EndPort()
{
    ImGui::EndHorizontal();
    EndPin();
}

void NodeBuilder::Middle()
{
    if (_current_stage == Stage::Begin)
    {
        SetStage(Stage::Content);
    }

    SetStage(Stage::Middle);
}

void NodeBuilder::Output(ed::PinId id)
{
    if (_current_stage == Stage::Begin)
    {
        SetStage(Stage::Content);
    }

    const auto apply_padding = (_current_stage == Stage::Output);

    SetStage(Stage::Output);

    if (apply_padding) ImGui::Spring(0);

    Pin(id, ed::PinKind::Output);

    ImGui::BeginHorizontal(id.AsPointer());
}

void NodeBuilder::EndOutput()
{
    ImGui::EndHorizontal();

    EndPin();
}

bool NodeBuilder::SetStage(Stage stage)
{
    if (stage == _current_stage)
    {
        return false;
    }

    auto old_stage = _current_stage;
    _current_stage = stage;

    ImVec2 cursor;
    switch (old_stage)
    {
    case Stage::Begin:
        break;

    case Stage::Header:
        ImGui::EndHorizontal();
        _header_min = ImGui::GetItemRectMin();
        _header_max = ImGui::GetItemRectMax();

        ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);
        break;

    case Stage::Content:
        break;

    case Stage::Input:
        ed::PopStyleVar(2);

        ImGui::Spring(1, 0);
        ImGui::EndVertical();
        break;

    case Stage::Middle:
        ImGui::EndVertical();
        break;

    case Stage::Output:
        ed::PopStyleVar(2);

        ImGui::Spring(1, 0);
        ImGui::EndVertical();
        break;

    case Stage::End:
        break;

    case Stage::Invalid:
        break;
    }

    switch (stage)
    {
    case Stage::Begin:
        ImGui::BeginVertical("node");
        break;

    case Stage::Header:
        _has_header = true;

        ImGui::BeginHorizontal("header");
        break;

    case Stage::Content:
        if (old_stage == Stage::Begin) ImGui::Spring(0);

        ImGui::BeginHorizontal("content");
        ImGui::Spring(0, 0);
        break;

    case Stage::Input:
        ImGui::BeginVertical("inputs", ImVec2(0, 0), 0.0f);

        ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(0, 0.5f));
        ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

        if (!_has_header) ImGui::Spring(1, 0);
        break;

    case Stage::Middle:
        ImGui::Spring(1);
        ImGui::BeginVertical("middle", ImVec2(0, 0), 1.0f);
        break;

    case Stage::Output:
        if (old_stage == Stage::Middle || old_stage == Stage::Input)
            ImGui::Spring(1);
        else
            ImGui::Spring(1, 0);
        ImGui::BeginVertical("outputs", ImVec2(0, 0), 1.0f);

        ed::PushStyleVar(ed::StyleVar_PivotAlignment, ImVec2(1.0f, 0.5f));
        ed::PushStyleVar(ed::StyleVar_PivotSize, ImVec2(0, 0));

        if (!_has_header) ImGui::Spring(1, 0);
        break;

    case Stage::End:
        if (old_stage == Stage::Input) ImGui::Spring(1, 0);
        if (old_stage != Stage::Begin) ImGui::EndHorizontal();

        _content_min = ImGui::GetItemRectMin();
        _content_max = ImGui::GetItemRectMax();

        ImGui::EndVertical();
        _node_min = ImGui::GetItemRectMin();
        _node_max = ImGui::GetItemRectMax();
        break;

    case Stage::Invalid:
        break;
    }

    return true;
}

void NodeBuilder::Pin(ed::PinId id, ed::PinKind kind) { ed::BeginPin(id, kind); }

void NodeBuilder::EndPin() { ed::EndPin(); }

FLOW_UI_NAMESPACE_END
