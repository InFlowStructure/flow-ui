// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"

#include <flow/core/UUID.hpp>

#include <mutex>

FLOW_UI_NAMESPACE_START

/**
 * @brief Visual representation of a flow::Connection.
 */
class ConnectionView
{
  public:
    /**
     * @brief Constructs a connection view.
     *
     * @param id The UUID hash of the Connection.
     * @param start_port_id The output Port ID.
     * @param end_port_id
     * @param colour
     */
    ConnectionView(const flow::UUID& id, std::uint64_t start_port_id, std::uint64_t end_port_id, const Colour& colour);

    /**
     * @brief Render the connection to the graph.
     */
    void Draw();

    /**
     * @brief Gets the current flowing status of the connection.
     * @returns true if the connection is flowing, false otherwise.
     */
    bool IsFlowing() const noexcept { return _flowing; }

    /**
     * @brief Sets the connection's flowing status.
     * @param flowing The new value for the flowing status.
     */
    void SetFlowing(bool flowing) noexcept { _flowing = flowing; }

    /**
     * @brief Gets the colour of the connection.
     * @returns The colour of the connection.
     */
    const Colour& GetColour() const noexcept { return _colour; }

  public:
    /// The UUID hash of the Connection.
    std::uint64_t ID;

    /// The UUID hash of the starting Port.
    std::uint64_t StartPortID;

    /// The UUID hash of the ending Port.
    std::uint64_t EndPortID;

  private:
    Colour _colour;
    bool _flowing = false;
};

FLOW_UI_NAMESPACE_END
