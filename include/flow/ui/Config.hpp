// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <memory>
#include <string>
#include <unordered_map>

FLOW_UI_NAMESPACE_START
class Font;
FLOW_UI_NAMESPACE_END

/**
 * Specialization to avoid trying to cleanup Fonts as they are unowned.
 */
template<>
struct std::default_delete<flow::ui::Font>
{
    void operator()(flow::ui::Font*) const {}
};

FLOW_UI_NAMESPACE_START

/**
 * @brief Flags for choosing dockspace splitting direction.
 */
enum class DockspaceSplitDirection : std::uint8_t
{
    Up,
    Down,
    Left,
    Right,
};

/**
 * @brief List of supported backends.
 */
enum class RendererBackend
{
    Null = 0,
    FirstAvailable,
    OpenGL3,
    Metal,
    Vulkan,
    DirectX11,
    DirectX12,
};

/**
 * @brief Configuration details for the editor.
 */
struct Config
{
    std::unique_ptr<Font> DefaultFont;
    std::unique_ptr<Font> IconFont;
    std::unique_ptr<Font> NodeHeaderFont;
    RendererBackend RenderBackend = RendererBackend::FirstAvailable;
};

/**
 * @brief Loads a font from the given path.
 *
 * @param font_path The path to the font file.
 * @param size The desired size of the font.
 *
 * @returns A pointer to the new font.
 */
std::unique_ptr<Font> LoadFont(const std::string& font_path, float size);

/**
 * @brief Get the global config for the library.
 * @returns The global config.
 */
Config& GetConfig();

FLOW_UI_NAMESPACE_END
