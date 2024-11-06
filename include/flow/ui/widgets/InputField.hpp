// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/utilities/Widgets.hpp"

FLOW_UI_SUBNAMESPACE_START(widgets)

/**
 * Input Widget interface for creating input field widgets for port data based on type..
 */
struct InputInterface
{
    virtual flow::SharedNodeData Draw() = 0;
};

/**
 * Input field widget
 */
template<typename T>
class Input : public InputInterface
{
  public:
    Input(std::string name, const T& initial_value, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None)
        : _name{std::move(name)}, _value{initial_value}, _flags{flags}
    {
    }

    flow::SharedNodeData Draw() override
    {
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(36, 36, 36, 255));
        if (InputField<typename std::remove_cvref_t<T>>(_name, _value, _flags))
        {
            ImGui::PopStyleColor();
            return MakeNodeData<T>(_value);
        }
        ImGui::PopStyleColor();

        return nullptr;
    }

  private:
    std::string _name;
    T _value;
    ImGuiInputTextFlags _flags;
    flow::SharedNodeData _ref_data;
};

FLOW_UI_SUBNAMESPACE_END
