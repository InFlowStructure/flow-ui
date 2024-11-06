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
#include "windows/PropertyWindow.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Event.hpp>
#include <hello_imgui/hello_imgui.h>
#include <imgui-node-editor/imgui_node_editor.h>

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace ax;
namespace ed = ax::NodeEditor;

FLOW_UI_NAMESPACE_START

class Editor
{
  public:
    Editor(const std::string& initial_file);

    void Run();

    flow::Event<ImGuiIO&, Config&> LoadFonts    = [](auto, auto) {};
    flow::Event<ImGuiStyle&, Style&> SetupStyle = [](auto, auto) {};

  protected:
    void Init(const std::string& initial_file);
    void Teardown();

    void HandleInput();

    void DrawMainMenuBar();

    std::vector<HelloImGui::DockingSplit> SplitDockSpace() const;
    std::vector<HelloImGui::DockableWindow> BuildDockableWindows();

    auto& GetGraphWindows() { return _graph_windows; }

    bool ShouldReloadWindows() const { return _reload_windows; }
    void SetShouldReloadWindows(bool value) { _reload_windows = value; }

    std::shared_ptr<GraphWindow>& CreateFlow(std::string name);
    void LoadFlow(std::string file = "");
    void SaveFlow(bool save_as);

  private:
    void SetPropertyWindowGraph(std::shared_ptr<flow::Graph> graph);

  private:
    FileStorage _filestorage{
        std::filesystem::current_path() / "modules",
        FileStorage::GetDocumentsPath() / "flows",
    };

    std::shared_ptr<ViewFactory> _factory = std::make_shared<ViewFactory>();
    std::shared_ptr<Env> _env             = Env::Create(_factory);

    std::vector<std::unique_ptr<flow::ui::Window>> _windows;
    std::unordered_map<flow::UUID, std::shared_ptr<GraphWindow>> _graph_windows;

    HelloImGui::RunnerParams _params;

    bool _init           = false;
    bool _reload_windows = true;
};

FLOW_UI_NAMESPACE_END
