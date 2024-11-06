// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Window.hpp"

#include <spdlog/spdlog.h>

#include <string>
#include <utility>

FLOW_UI_NAMESPACE_START

class LogWindow : public Window
{
  public:
    LogWindow();
    virtual ~LogWindow() = default;

    virtual void Draw() override;

    static std::string Name;

  private:
    std::vector<std::pair<spdlog::details::log_msg, std::string>> _messages;
};

FLOW_UI_NAMESPACE_END
