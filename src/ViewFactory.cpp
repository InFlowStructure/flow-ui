// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "ViewFactory.hpp"

#include "Widgets.hpp"

#ifdef FLOW_WINDOWS
#undef GetClassName
#endif

FLOW_UI_NAMESPACE_START

std::shared_ptr<NodeView> ViewFactory::CreateNodeView(flow::SharedNode node)
{
    auto found = _constructors.find(std::string{node->GetClass()});
    if (found == _constructors.end())
    {
        return std::make_shared<NodeView>(std::move(node));
    }

    return std::shared_ptr<NodeView>(reinterpret_cast<NodeView*>(found->second(std::move(node))));
}

FLOW_UI_NAMESPACE_END
