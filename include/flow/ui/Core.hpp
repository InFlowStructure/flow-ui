// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include <flow/core/Core.hpp>

#include <memory>

#define FLOW_UI_NAMESPACE FLOW_NAMESPACE::ui
#define FLOW_UI_NAMESPACE_START                                                                                        \
    /** The UI namespace. */                                                                                           \
    namespace FLOW_UI_NAMESPACE                                                                                        \
    {
#define FLOW_UI_SUBNAMESPACE_START(n)                                                                                  \
    namespace FLOW_UI_NAMESPACE                                                                                        \
    {                                                                                                                  \
    namespace n                                                                                                        \
    {
#define FLOW_UI_NAMESPACE_END }
#define FLOW_UI_SUBNAMESPACE_END                                                                                       \
    }                                                                                                                  \
    }

FLOW_UI_NAMESPACE_START

class EditorContext;

FLOW_UI_NAMESPACE_END

template<>
struct std::default_delete<FLOW_UI_NAMESPACE::EditorContext>
{
    void operator()(FLOW_UI_NAMESPACE::EditorContext*) const {}
};

FLOW_UI_NAMESPACE_START

/**
 * @brief Get the current context for the graph editor.
 * @returns A wrapper around the editor context.
 */
std::unique_ptr<EditorContext> GetEditorContext() noexcept;

FLOW_UI_NAMESPACE_END
