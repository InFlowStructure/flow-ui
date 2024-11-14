// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include <flow/core/Core.hpp>

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