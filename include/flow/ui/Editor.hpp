// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Config.hpp"
#include "FileStorage.hpp"
#include "Style.hpp"
#include "ViewFactory.hpp"
#include "Window.hpp"
#include "views/LinkView.hpp"
#include "views/NodeView.hpp"
#include "views/PortView.hpp"
#include "windows/GraphWindow.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Event.hpp>
#include <hello_imgui/hello_imgui.h>
#include <imgui_node_editor.h>

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace ax;
namespace ed = ax::NodeEditor;

FLOW_UI_NAMESPACE_START

enum class DockspaceSplitDirection
{
    Up    = ImGuiDir_Up,
    Down  = ImGuiDir_Down,
    Left  = ImGuiDir_Left,
    Right = ImGuiDir_Right,
};

class Editor
{
  public:
    Editor(const std::string& initial_file);

    void Run();

    void AddWindow(std::shared_ptr<Window> new_window, const std::string& dockspace);
    void AddDockspace(std::string name, std::string initial_dockspace_name, float ratio,
                      DockspaceSplitDirection direction);

    std::shared_ptr<Env> GetEnv() const noexcept { return _env; }
    std::shared_ptr<ViewFactory> GetFactory() const noexcept { return _factory; }

    flow::Event<ImGuiIO&, Config&> LoadFonts    = [](auto, auto) {};
    flow::Event<ImGuiStyle&, Style&> SetupStyle = [](auto, auto) {};

    flow::EventDispatcher<const std::shared_ptr<Graph>&> OnActiveGraphChanged;

  protected:
    void Init(const std::string& initial_file);
    void Teardown();

    void HandleInput();

    void DrawMainMenuBar();

    std::shared_ptr<GraphWindow>& CreateFlow(std::string name);
    void LoadFlow(std::string file = "");
    void SaveFlow(bool save_as);

    std::vector<HelloImGui::DockableWindow>::iterator GetDockableGraphWindowBegin() noexcept;

  private:
    FileStorage _filestorage{
        std::filesystem::current_path() / "modules",
        FileStorage::GetDocumentsPath() / "flows",
    };

    std::shared_ptr<ViewFactory> _factory = std::make_shared<ViewFactory>();
    std::shared_ptr<Env> _env             = Env::Create(_factory);

    std::vector<std::shared_ptr<Window>> _windows;
    std::unordered_map<flow::UUID, std::shared_ptr<GraphWindow>> _graph_windows;

    HelloImGui::RunnerParams _params;
};

FLOW_UI_NAMESPACE_END
