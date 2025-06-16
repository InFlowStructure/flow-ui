// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <flow/core/TypeName.hpp>

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>

FLOW_UI_NAMESPACE_BEGIN

/**
 * @brief Enumeration of different types of Port Icons.
 */
enum class PortIconType : std::uint8_t
{
    Flow,
    Circle,
    Square,
    Grid,
    RoundSquare,
    Diamond
};

/**
 * @brief RGBA Colour type. Values are uint8_t.
 */
struct Colour
{
  public:
    constexpr Colour() noexcept = default;
    constexpr Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b) : R(r), G(g), B(b) {}
    constexpr Colour(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint8_t a) : R(r), G(g), B(b), A(a) {}

  public:
    std::uint8_t R = 255;
    std::uint8_t G = 255;
    std::uint8_t B = 255;
    std::uint8_t A = 255;
};

/**
 * @brief Universal style for the Editor.
 */
struct Style
{
  public:
    Style();

    /**
     * @brief Set port colour for specific port typename.
     * @param type The name of the type.
     * @param colour The colour to render the port as.
     */
    void SetTypeColour(std::string_view type, const Colour& colour);

    /**
     * @brief Get the colour of a port type by typename.
     * @param type The name of the type.
     * @returns The colours registered for the given typename.
     */
    Colour GetTypeColour(std::string_view type) const;

    /**
     * @brief Get the colour of a port type by typename.
     * @tparam T The type of the port.
     * @param colour The colour to render the port as.
     */
    template<typename T>
    void SetTypeColour(const Colour& colour)
    {
        return SetTypeColour(flow::TypeName_v<T>, colour);
    }

    /**
     * @brief Get the colour of a port type by typename.
     * @tparam T The type of the port.
     * @returns The colours registered for the given type.
     */
    template<typename T>
    Colour GetTypeColour() const
    {
        return GetTypeColour(flow::TypeName_v<T>);
    }

  public:
    /// The Icon shapes for different kinds of ports.
    struct
    {
        /// Icon shape for default port type.
        PortIconType Default;

        /// Icon shape for reference port types.
        PortIconType Ref;
    } PortShapes;

    /// Registered colours for port types.
    std::unordered_map<std::string_view, Colour> TypeColours;
};

/**
 * @brief Get the global style for the editor.
 * @returns The global style.
 */
Style& GetStyle();

FLOW_UI_NAMESPACE_END
