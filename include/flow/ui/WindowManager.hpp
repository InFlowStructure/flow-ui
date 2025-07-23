// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"
#include "Window.hpp"
#include "windows/GraphWindow.hpp"

#include <memory>
#include <string>
#include <vector>

FLOW_UI_NAMESPACE_BEGIN

class WindowManager
{
  public:
    void Teardown();

    /**
     * @brief Add a new custom window to the editor.
     *
     * @param new_window The new window to add to the editor.
     * @param dockspace The default dockspace to attach the window to.
     * @param visible_by_default Flag to show or hide the window by default.
     */
    void AddWindow(std::shared_ptr<Window> new_window, const std::string& dockspace, bool visible_by_default = true);

    const std::shared_ptr<GraphWindow>& CreateGraphWindow(std::string name, const std::shared_ptr<Env>& env);

    const std::vector<std::shared_ptr<Window>>& GetWindows() const noexcept { return _windows; }

    const std::unordered_map<UUID, std::shared_ptr<GraphWindow>>& GetGraphWindows() const noexcept
    {
        return _graph_windows;
    }

    std::shared_ptr<GraphWindow> GetActiveGraphWindow() const noexcept;

    void CleanupDeadWindows();

    void CloseActiveGraphWindow();

    void RemoveUnloadedModuleNode(std::string_view class_name);

  public:
    /// Event dispatcher that is run every time a new graph is marked as the active graph.
    EventDispatcher<const std::shared_ptr<Graph>&> OnActiveGraphChanged;

  private:
    std::vector<std::shared_ptr<Window>> _windows;
    std::unordered_map<UUID, std::shared_ptr<GraphWindow>> _graph_windows;
};

FLOW_UI_NAMESPACE_END