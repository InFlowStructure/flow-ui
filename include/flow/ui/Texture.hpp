// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <flow/core/NodeData.hpp>

#include <cstdint>

FLOW_UI_NAMESPACE_START

/**
 * @brief Texture wrapper type. Access cached images or caches new ones.
 */
struct Texture
{
    /**
     * @brief Creates and caches new image data.
     *
     * @param data The data to cache.
     * @param width The width of the image to cache.
     * @param height The height of the image to cache.
     */
    Texture(std::uint8_t* data, int width, int height);

    /**
     * @brief Creates a wrapper that access an image from cached data.
     *
     * @param texture_id THe ID of the texture to retrieve from cache.
     * @param width The width of the image to cache.
     * @param height The height of the image to cache.
     */
    Texture(std::uint64_t texture_id, int width, int height);

    /// The ID of the texture as stored in cache.
    std::uint64_t ID;

    /// The size of the image.
    struct
    {
        /// The width of the image.
        int Width;

        /// The height of the image.
        int Height;
    } Size;
};

FLOW_UI_NAMESPACE_END
