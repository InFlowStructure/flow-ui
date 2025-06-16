#pragma once

#include "Config.hpp"
#include "Core.hpp"
#include "Style.hpp"

#include <hello_imgui/hello_imgui.h>
#include <imgui.h>
#include <imgui_node_editor.h>

FLOW_UI_SUBNAMESPACE_BEGIN(utility)

using namespace ax;
namespace ed = ax::NodeEditor;

constexpr RendererBackend to_RendererBackend(const HelloImGui::RendererBackendType& backend)
{
    switch (backend)
    {
    case HelloImGui::RendererBackendType::Null:
        return RendererBackend::Null;
    case HelloImGui::RendererBackendType::FirstAvailable:
        return RendererBackend::FirstAvailable;
    case HelloImGui::RendererBackendType::OpenGL3:
        return RendererBackend::OpenGL3;
    case HelloImGui::RendererBackendType::Metal:
        return RendererBackend::Metal;
    case HelloImGui::RendererBackendType::Vulkan:
        return RendererBackend::Vulkan;
    case HelloImGui::RendererBackendType::DirectX11:
        return RendererBackend::DirectX11;
    case HelloImGui::RendererBackendType::DirectX12:
        return RendererBackend::DirectX12;
    }

    return RendererBackend::Null;
}

constexpr ImGuiDir to_ImGuiDir(DockspaceSplitDirection dir)
{
    switch (dir)
    {
    case DockspaceSplitDirection::Up:
        return ImGuiDir_Up;
    case DockspaceSplitDirection::Down:
        return ImGuiDir_Down;
    case DockspaceSplitDirection::Left:
        return ImGuiDir_Left;
    case DockspaceSplitDirection::Right:
        return ImGuiDir_Right;
    }
    return ImGuiDir_Down;
}

constexpr ImColor to_ImColor(const Colour& c) noexcept { return ImColor(c.R, c.G, c.B, c.A); }
constexpr Colour to_Colour(const ImVec4& c) noexcept
{
    return Colour(static_cast<std::uint8_t>(c.x * 255.f), static_cast<std::uint8_t>(c.y * 255.f),
                  static_cast<std::uint8_t>(c.z * 255.f), static_cast<std::uint8_t>(c.w * 255.f));
}

FLOW_UI_SUBNAMESPACE_END
