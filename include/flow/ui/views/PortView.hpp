// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"

#include <flow/core/Node.hpp>

#include <memory>
#include <string_view>

FLOW_UI_NAMESPACE_START

class NodeView;

namespace widgets
{
class InputInterface;
}

namespace utility
{
class NodeBuilder;
}

enum class PortType
{
    Output,
    Input
};

/**
 * @brief View of a Port to be drawn on a NodeView.
 */
class PortView
{
  public:
    /**
     * @brief Input event type for port types that have input fields registered.
     */
    using InputEvent = Event<const flow::IndexableName&, flow::SharedNodeData>;

    /**
     * @brief Contrusts a port view for a given node.
     *
     * @param node_id The ID of the NodeVIew this port belongs to.
     * @param port_data The data pointer of the Port.
     * @param factory View factory for creating ports from registered types.
     * @param on_input Event to run on input for types that have input fields regsitered.
     * @param show_label Show the port label or not.
     */
    PortView(const std::uint64_t& node_id, std::shared_ptr<Port> port_data,
             const std::shared_ptr<class ViewFactory>& factory, InputEvent on_input, bool show_label = true);

    /**
     * @brief Renders the Port to NodeView.
     */
    void Draw();

    /**
     * @brief Get the connected status of the port.
     * @returns true if the Port is connected, false otherwise.
     */
    bool IsConnected() const noexcept { return _port->IsConnected(); }

    /**
     * @brief Checks if the prt can link to the given port.
     * @param other THe ports to check against.
     */
    bool CanLink(const std::shared_ptr<PortView>& other) const noexcept;

    /**
     * @brief Get the port data.
     * @returns The port's data pointer.
     */
    const flow::SharedNodeData& GetData() const noexcept { return _port->GetData(); }

    /**
     * @brief Get the port data cast to a specific type.
     * @tparam T The type of the data.
     * @returns The port's data pointer cast to the given type.
     */
    template<typename T>
    flow::TSharedNodeData<T> GetData() const noexcept
    {
        return CastNodeData<T>(GetData());
    }

    /**
     * @brief Gets the unique hash key of the port.
     * @returns The IndexableName of the port.
     */
    const flow::IndexableName& Key() const noexcept { return _port->GetKey(); }

    /**
     * @brief Gets the name/label of the port.
     * @returns The port's name.
     */
    std::string_view Name() const noexcept { return _port->GetVarName(); }

    /**
     * @brief Gets the caption/description of the port.
     * @returns The port's caption.
     */
    std::string_view Caption() const noexcept { return _port->GetCaption(); }

    /**
     * @brief Get's the typename of the port data.
     * @returns The port's data typename.
     */
    std::string_view Type() const noexcept { return _port->GetDataType(); }

    /**
     * @brief Gets the colour of the data type.
     * @returns The data type's registered colour.
     */
    Colour GetColour() const noexcept { return GetStyle().GetTypeColour(Type()); }

    /**
     * @brief Sets the view builder pointer.
     * @param builder The new builder to use.
     */
    void SetBuilder(std::shared_ptr<utility::NodeBuilder> builder) noexcept;

    /**
     * @brief Sets whether or not the port should be shown as connectable or not.
     * @param new_link_pin
     */
    void ShowConnectable(const std::shared_ptr<PortView>& new_link_pin);

    /**
     * @brief Set whether or not to show the label.
     * @param show true if the label should be rendered, false if not.
     */
    void SetShowLabel(bool show) { _show_label = show; }

  protected:
    void DrawInput();

  private:
    void DrawLabel();
    void DrawIcon(float alpha);

  public:
    /// The UUID hash of the Port.
    std::uint64_t ID;

    /// The owning NodeView ID.
    const std::uint64_t& NodeID;

    /// The type of port.
    PortType Kind = PortType::Input;

    /// Event run on setting a new value in the input field.
    InputEvent OnSetInput;

  private:
    std::shared_ptr<Port> _port;
    std::shared_ptr<widgets::InputInterface> _input_field;

    bool _show_label = true;
    bool _was_active = false;
    float _alpha     = 1.f;

    std::shared_ptr<utility::NodeBuilder> _builder;
};
FLOW_UI_NAMESPACE_END
