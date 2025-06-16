// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <flow/core/Event.hpp>

#include <map>

FLOW_UI_NAMESPACE_BEGIN

class InputManager
{
  public:
    void Handle();

    void AddInputEvent(int key_chord, Event<>&& event);

  private:
    std::map<int, Event<>> _input_events;
};

FLOW_UI_NAMESPACE_END