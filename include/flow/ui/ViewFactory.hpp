// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"
#include "views/NodeView.hpp"
#include "views/PortView.hpp"
#include "widgets/InputField.hpp"

#include <flow/core/Concepts.hpp>
#include <flow/core/NodeFactory.hpp>

#include <string>
#include <type_traits>
#include <unordered_map>

FLOW_UI_NAMESPACE_START

template<class T>
concept NodeViewType = std::is_base_of_v<NodeView, T>;

/**
 * @brief Factory for creating nodes as well as node views and input fields.
 */
class ViewFactory : public flow::NodeFactory
{
    using NodeViewConstructorCallback = std::function<NodeView*(flow::SharedNode)>;

  public:
    ViewFactory()          = default;
    virtual ~ViewFactory() = default;

    /**
     * @brief Register a view type to a node type.
     *
     * @tparam ViewType The node view type to register.
     * @tparam NodeType The node type to register the view type to.
     */
    template<NodeViewType ViewType, flow::concepts::NodeType NodeType>
    void RegisterNodeView()
    {
        _constructors.emplace(TypeName_v<NodeType>, NodeViewConstructorHelper<ViewType>);
    }

    /**
     * @brief Register one view type for several ndoe types.
     *
     * @tparam ViewType The node view type to register.
     * @tparam NodeType The node types to register the view type to.
     */
    template<NodeViewType ViewType, flow::concepts::NodeType... NodeType>
    void RegisterNodeViews()
    {
        (RegisterNodeView<ViewType, NodeType>(), ...);
    }

    /**
     * @brief Create a node view for the given node.
     * @param node The node to create a view for.
     * @returns The newly created view for the given node.
     */
    std::shared_ptr<NodeView> CreateNodeView(flow::SharedNode node);

    /**
     * @brief Register a input field for a given type.
     * @tparam T The type to register.
     * @param initial_value The initial value to use for any instance of the registered input field.
     */
    template<typename T>
    void RegisterInputType(const T& initial_value)
    {
        _input_field_contructors[std::string{flow::TypeName_v<T>}] =
            [=](std::string name, const SharedNodeData& data) -> std::shared_ptr<widgets::InputInterface> {
            T value = initial_value;
            if (auto d = CastNodeData<T>(data))
            {
                value = d->Get();
            }
            else if (auto ref_data = CastNodeData<T&>(data))
            {
                value = ref_data->Get();
            }

            return std::make_shared<widgets::Input<T>>(std::move(name), value);
        };
    }

    /**
     * @brief Get the list of registered input field constructors.
     * @returns A reference to the map of registered input field constructors.
     */
    const auto& GetRegisteredInputTypes() { return _input_field_contructors; }

  private:
    template<NodeViewType ViewType>
    static NodeView* NodeViewConstructorHelper(flow::SharedNode node)
    {
        return new ViewType(std::move(node));
    }

  private:
    std::unordered_map<std::string, NodeViewConstructorCallback> _constructors;

    using InputFieldConstructor_t =
        std::function<std::shared_ptr<widgets::InputInterface>(std::string name, const SharedNodeData&)>;

    std::unordered_map<std::string, InputFieldConstructor_t> _input_field_contructors;
};

FLOW_UI_NAMESPACE_END
