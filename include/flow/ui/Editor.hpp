// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Config.hpp"
#include "FileExplorer.hpp"
#include "Style.hpp"
#include "ViewFactory.hpp"
#include "Window.hpp"
#include "views/LinkView.hpp"
#include "views/NodeView.hpp"
#include "views/PortView.hpp"
#include "windows/GraphWindow.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Event.hpp>

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace ax;
namespace ed = ax::NodeEditor;

FLOW_UI_NAMESPACE_START

/**
 * @brief Main application class the holds and runs the graphs and other windows.
 */
class Editor
{
  public:
    /**
     * @brief Initialise the Editor, potentially with an inital flow file to open.
     * @param initial_file The flow file to open up. If empty, then an empty flow graph will be created.
     */
    Editor(const std::string& initial_file);

    /**
     * @brief The main running method which renders the editor and all related windows.
     */
    void Run();

    /**
     * @brief Add a new custom window to the editor.
     *
     * @param new_window The new window to add to the editor.
     * @param dockspace The default dockspace to attach the window to.
     * @param show Flag to show or hide the window by default.
     */
    void AddWindow(std::shared_ptr<Window> new_window, const std::string& dockspace, bool show = true);

    /**
     * @brief Adds a new dockspace that windows can be docked to.
     *
     * @param name The name of the dockspace.
     * @param initial_dockspace_name The dockspace to split off from.
     * @param ratio How much of the original dockspace should be split off for the new dockspace.
     * @param direction The direction for the new split.
     *
     * @note Should be called before adding windows.
     */
    void AddDockspace(std::string name, std::string initial_dockspace_name, float ratio,
                      DockspaceSplitDirection direction);

    /**
     * @brief Get a reference to the shared environment.
     * @returns THe shared ptr to the Env.
     */
    const std::shared_ptr<Env>& GetEnv() const noexcept { return _env; }

    /**
     * @brief Get a reference to the factory for building nodes and node views.
     * @returns The ViewFactory shared ptr.
     */
    const std::shared_ptr<ViewFactory>& GetFactory() const noexcept { return _factory; }

    /**
     * @brief Get a reference to the current editor context.
     * @returns The pointer to the current editor context.
     */
    void* GetContext() const noexcept;

    /**
     * @brief Event that is run to load custom fonts for the editor.
     */
    Event<Config&> LoadFonts = [](auto&) {};

    /**
     * @brief Event that is run to setup a custom appearance of the editor.
     */
    Event<Style&> SetupStyle = [](auto&) {};

    /**
     * @brief Event dispatcher that is run every time a new graph is marked as the active graph.
     */
    EventDispatcher<const std::shared_ptr<Graph>&> OnActiveGraphChanged;

  protected:
    void Init(const std::string& initial_file);
    void Teardown();

    void HandleInput();

    void DrawMainMenuBar();

    std::shared_ptr<GraphWindow>& CreateFlow(std::string name);
    void LoadFlow(const std::filesystem::path& file = "");
    void SaveFlow();

  private:
    std::shared_ptr<ViewFactory> _factory = std::make_shared<ViewFactory>();
    std::shared_ptr<Env> _env             = Env::Create(_factory);

    std::vector<std::shared_ptr<Window>> _windows;
    std::unordered_map<UUID, std::shared_ptr<GraphWindow>> _graph_windows;
    EventDispatcher<> OnGraphWindowAdded;
    EventDispatcher<> OnGraphWindowRemoved;
};

FLOW_UI_NAMESPACE_END
