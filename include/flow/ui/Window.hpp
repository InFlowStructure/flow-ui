// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <string>

FLOW_UI_NAMESPACE_START

/// Default name of the main dockspace.
static inline const std::string DefaultDockspace = "MainDockSpace";

/**
 * @brief Base Windows class.
 */
class Window
{
  public:
    /**
     * @brief Constructs a window with a name.
     * @param name The name of the window.
     */
    Window(std::string name);
    virtual ~Window() = default;

    /**
     * @brief Gets the name of the window.
     * @returns The name of the window.
     */
    const std::string& GetName() const { return _name; }

    /**
     * @brief Function that is run before the window is ever drawn.
     */
    virtual void Init() {}

    /**
     * @brief Function that is run when the window is removed.
     */
    virtual void Teardown() {}

    /**
     * @brief Draw function of the window.
     *
     * @note By default, if this is not overridden, then the window will simply display the message "Nothing to show".
     */
    virtual void Draw();

  protected:
    std::string _name;
};

FLOW_UI_NAMESPACE_END
