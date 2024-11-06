// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include <flow/core/Core.hpp>

#ifdef _WIN32
#ifdef FLOW_UI_EXPORT
#define FLOW_UI_API __declspec(dllexport)
#else
#define FLOW_UI_API __declspec(dllimport)
#endif
#else
#define FLOW_UI_API
#endif

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