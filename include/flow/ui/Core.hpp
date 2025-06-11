// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include <flow/core/Core.hpp>

#include <memory>

// clang-format off
#define FLOW_UI_NAMESPACE_START namespace flow::ui {
#define FLOW_UI_SUBNAMESPACE_START(nested) namespace flow::ui { namespace nested {
#define FLOW_UI_NAMESPACE_END }
#define FLOW_UI_SUBNAMESPACE_END } }
// clang-format on

FLOW_UI_NAMESPACE_START

class EditorContext;

FLOW_UI_NAMESPACE_END

template<>
struct std::default_delete<flow::ui::EditorContext>
{
    void operator()(flow::ui::EditorContext*) const {}
};

FLOW_UI_NAMESPACE_START

/**
 * @brief Get the current context for the graph editor.
 * @returns A wrapper around the editor context.
 */
std::unique_ptr<EditorContext> GetEditorContext() noexcept;

FLOW_UI_NAMESPACE_END
