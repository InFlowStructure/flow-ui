// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"
#include "Style.hpp"

#include <flow/core/Concepts.hpp>
#include <flow/core/NodeData.hpp>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <imgui.h>
#include <imgui_stdlib.h>

#include <chrono>
#include <stdexcept>

FLOW_UI_SUBNAMESPACE_START(widgets)

void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, PortIconType type, bool filled, ImU32 color,
              ImU32 innerColor);

void Icon(const ImVec2& size, PortIconType type, bool filled, const ImVec4& color = ImVec4(1, 1, 1, 1),
          const ImVec4& innerColor = ImVec4(0, 0, 0, 0));

template<std::integral T>
static inline bool InputInteger(ImGuiDataType type, std::string_view name, T& value, ImGuiInputTextFlags flags)
{
    ImGui::PushItemWidth(50.0f);

    ImGui::InputScalar(("##" + std::string{name}).c_str(), type, &value, 0, 0, "%d", flags);

    ImGui::PopItemWidth();

    return ImGui::IsItemDeactivatedAfterEdit();
}

template<concepts::Duration D>
static inline bool InputChrono(std::string_view name, D& value, ImGuiInputTextFlags flags)
{
    ImGui::PushItemWidth(100.0f);

    const D step{1};
    ImGui::InputScalar(("##" + std::string{name}).c_str(), ImGuiDataType_S64, &value, &step, 0, "%d", flags);

    ImGui::PopItemWidth();

    return ImGui::IsItemDeactivatedAfterEdit();
}

template<typename T>
static inline bool InputField(std::string_view name, T& value, ImGuiInputTextFlags flags);

template<>
inline bool InputField<bool>(std::string_view name, bool& value, ImGuiInputTextFlags)
{
    return ImGui::Checkbox(("##" + std::string{name}).c_str(), &value);
}

template<>
inline bool InputField<std::int8_t>(std::string_view name, std::int8_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::int8_t>(ImGuiDataType_S8, name, value, flags);
}

template<>
inline bool InputField<std::int16_t>(std::string_view name, std::int16_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::int16_t>(ImGuiDataType_S16, name, value, flags);
}

template<>
inline bool InputField<std::int32_t>(std::string_view name, std::int32_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::int32_t>(ImGuiDataType_S32, name, value, flags);
}

template<>
inline bool InputField<std::int64_t>(std::string_view name, std::int64_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::int64_t>(ImGuiDataType_S64, name, value, flags);
}

template<>
inline bool InputField<std::uint8_t>(std::string_view name, std::uint8_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::uint8_t>(ImGuiDataType_U8, name, value, flags);
}

template<>
inline bool InputField<std::uint16_t>(std::string_view name, std::uint16_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::uint16_t>(ImGuiDataType_U16, name, value, flags);
}

template<>
inline bool InputField<std::uint32_t>(std::string_view name, std::uint32_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::uint32_t>(ImGuiDataType_U32, name, value, flags);
}

template<>
inline bool InputField<std::uint64_t>(std::string_view name, std::uint64_t& value, ImGuiInputTextFlags flags)
{
    return InputInteger<std::uint64_t>(ImGuiDataType_U64, name, value, flags);
}

template<>
inline bool InputField<float>(std::string_view name, float& value, ImGuiInputTextFlags flags)
{
    ImGui::PushItemWidth(50.0f);

    ImGui::InputFloat(("##" + std::string{name}).c_str(), &value, 0, 0, "%.3f", flags);

    ImGui::PopItemWidth();

    return ImGui::IsItemDeactivatedAfterEdit();
}

template<>
inline bool InputField<double>(std::string_view name, double& value, ImGuiInputTextFlags flags)
{
    ImGui::PushItemWidth(50.0f);

    ImGui::InputDouble(("##" + std::string{name}).c_str(), &value, 0, 0, "%.3f", flags);

    ImGui::PopItemWidth();

    return ImGui::IsItemDeactivatedAfterEdit();
}

template<>
inline bool InputField<std::string>(std::string_view name, std::string& value, ImGuiInputTextFlags flags)
{
    ImGui::PushItemWidth(150.0f);

    ImGui::InputText(("##" + std::string{name}).c_str(), &value, flags);

    ImGui::PopItemWidth();

    return ImGui::IsItemDeactivatedAfterEdit();
}

template<concepts::Duration D>
inline bool InputField(std::string_view name, D& value, ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::nanoseconds>(std::string_view name, std::chrono::nanoseconds& value,
                                                 ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::milliseconds>(std::string_view name, std::chrono::milliseconds& value,
                                                  ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::microseconds>(std::string_view name, std::chrono::microseconds& value,
                                                  ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::seconds>(std::string_view name, std::chrono::seconds& value,
                                             ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::minutes>(std::string_view name, std::chrono::minutes& value,
                                             ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::hours>(std::string_view name, std::chrono::hours& value, ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::days>(std::string_view name, std::chrono::days& value, ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::months>(std::string_view name, std::chrono::months& value,
                                            ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

template<>
inline bool InputField<std::chrono::years>(std::string_view name, std::chrono::years& value, ImGuiInputTextFlags flags)
{
    return InputChrono(name, value, flags);
}

FLOW_UI_SUBNAMESPACE_END
