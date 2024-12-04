// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Editor.hpp"

#include "Config.hpp"
#include "EditorNodes.hpp"
#include "ViewFactory.hpp"
#include "Widgets.hpp"
#include "Window.hpp"

#include <flow/core/Node.hpp>
#include <flow/core/NodeFactory.hpp>
#include <flow/core/Port.hpp>
#include <flow/core/UUID.hpp>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

FLOW_UI_NAMESPACE_START

Editor::Editor(const std::string& initial_file)
{
    _params.appWindowParams.windowTitle             = "Flow Code";
    _params.appWindowParams.borderless              = false;
    _params.appWindowParams.restorePreviousGeometry = true;
    _params.iniFolderType                           = HelloImGui::IniFolderType::TempFolder;
    _params.appWindowParams.windowGeometry.size     = {1920, 1080};

    _params.callbacks.PostInit = [&] {
        GetConfig().RenderBackend = _params.rendererBackendType;
        SetupStyle(ImGui::GetStyle(), GetStyle());
        Init(initial_file);
    };
    _params.callbacks.BeforeExit = [&] { Teardown(); };

    _params.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
    _params.imGuiWindowParams.enableViewports        = true;
    _params.imGuiWindowParams.showMenuBar            = true;
    _params.imGuiWindowParams.showMenu_App           = true;
    _params.imGuiWindowParams.showMenu_View          = false;

    _params.dpiAwareParams.fontRenderingScale  = 1.f;
    _params.dpiAwareParams.dpiWindowSizeFactor = 1.f;

    _params.dockingParams.mainDockSpaceNodeFlags = ImGuiDockNodeFlags_AutoHideTabBar;

    _params.callbacks.PreNewFrame = [=, this] {
        HandleInput();

        auto& dockable_windows      = _params.dockingParams.dockableWindows;
        auto graph_windows_begin_it = GetDockableGraphWindowBegin();

        auto it = std::remove_if(graph_windows_begin_it, dockable_windows.end(), [&, this](auto&& dw) {
            return std::none_of(_graph_windows.begin(), _graph_windows.end(),
                                [&](auto& g) { return dw.label == g.second->GetGraph()->GetName(); });
        });

        if (it != dockable_windows.end())
        {
            dockable_windows.erase(it, dockable_windows.end());
        }

        for (auto& [_, graph_view] : _graph_windows)
        {
            const auto& name = graph_view->GetGraph()->GetName();
            if (!!_params.dockingParams.dockableWindowOfName(name))
            {
                continue;
            }

            HelloImGui::DockableWindow graph_window;
            graph_window.label         = name;
            graph_window.dockSpaceName = DefaultDockspaces::Main;
            graph_window.GuiFunction   = [this, gv = graph_view]() {
                if (gv->IsActive())
                {
                    OnActiveGraphChanged.Broadcast(gv->GetGraph());
                }
                gv->Draw();
            };
            graph_window.includeInViewMenu      = false;
            graph_window.callBeginEnd           = false;
            graph_window.focusWindowAtNextFrame = true;
            graph_window.imGuiWindowFlags       = ImGuiWindowFlags_NoCollapse;

            dockable_windows.push_back(graph_window);

            // FIXME(trigaux): This will reset all windows, but it is currently the only way to dock new graph windows.
            _params.dockingParams.layoutReset = true;
        };
    };

    _params.imGuiWindowParams.showMenu_View_Themes = false;
    _params.fpsIdling.enableIdling                 = false;

    _params.callbacks.SetupImGuiStyle     = [&] { SetupStyle(ImGui::GetStyle(), GetStyle()); };
    _params.callbacks.LoadAdditionalFonts = [&] { LoadFonts(ImGui::GetIO(), GetConfig()); };
    _params.callbacks.ShowMenus           = [&] {
        DrawMainMenuBar();
        HelloImGui::ShowViewMenu(_params);
    };
}

void Editor::Init(const std::string& initial_file)
{
    _env->LoadModules(_filestorage.GetExtensionPath().string());

    _env->GetFactory()->RegisterNodeClass<PreviewNode>("Editor", "Preview");
    _factory->RegisterNodeView<PreviewNodeView, PreviewNode>();

    _factory->RegisterInputType<bool>(false);
    _factory->RegisterInputType<float>(0.f);
    _factory->RegisterInputType<double>(0.0);
    _factory->RegisterInputType<std::int8_t>(0);
    _factory->RegisterInputType<std::int16_t>(0);
    _factory->RegisterInputType<std::int32_t>(0);
    _factory->RegisterInputType<std::int64_t>(0);
    _factory->RegisterInputType<std::uint8_t>(0);
    _factory->RegisterInputType<std::uint16_t>(0);
    _factory->RegisterInputType<std::uint32_t>(0);
    _factory->RegisterInputType<std::uint64_t>(0);
    _factory->RegisterInputType<std::string>("");
    _factory->RegisterInputType<std::chrono::nanoseconds>(std::chrono::nanoseconds::zero());
    _factory->RegisterInputType<std::chrono::microseconds>(std::chrono::microseconds::zero());
    _factory->RegisterInputType<std::chrono::milliseconds>(std::chrono::milliseconds::zero());
    _factory->RegisterInputType<std::chrono::seconds>(std::chrono::seconds::zero());
    _factory->RegisterInputType<std::chrono::minutes>(std::chrono::minutes::zero());
    _factory->RegisterInputType<std::chrono::hours>(std::chrono::hours::zero());
    _factory->RegisterInputType<std::chrono::days>(std::chrono::days::zero());
    _factory->RegisterInputType<std::chrono::months>(std::chrono::months::zero());
    _factory->RegisterInputType<std::chrono::years>(std::chrono::years::zero());

    if (!initial_file.empty())
    {
        LoadFlow(initial_file);
    }
    else
    {
        auto graph                  = std::make_shared<flow::Graph>("untitled##0", _env);
        _graph_windows[graph->ID()] = std::make_shared<GraphWindow>(graph);
    }

    for (auto& window : _windows)
    {
        window->Init();
    }
}

void Editor::Teardown()
{
    for (auto& window : _windows)
    {
        window->Teardown();
    }
}

void Editor::Run() { HelloImGui::Run(_params); }

void Editor::AddWindow(std::shared_ptr<Window> new_window, const std::string& dockspace)
{
    auto& window = _windows.emplace_back(std::move(new_window));

    HelloImGui::DockableWindow dockable_window;
    dockable_window.label            = window->GetName();
    dockable_window.dockSpaceName    = dockspace;
    dockable_window.GuiFunction      = [&] { window->Draw(); };
    dockable_window.imGuiWindowFlags = ImGuiWindowFlags_NoCollapse;

    _params.dockingParams.dockableWindows.push_back(std::move(dockable_window));
}

void Editor::AddDockspace(std::string name, std::string initial_dockspace_name, float ratio,
                          DockspaceSplitDirection direction)
{
    HelloImGui::DockingSplit split;
    split.initialDock = std::move(initial_dockspace_name);
    split.newDock     = std::move(name);
    split.direction   = static_cast<ImGuiDir>(direction);
    split.ratio       = ratio;

    _params.dockingParams.dockingSplits.push_back(std::move(split));
}

void Editor::HandleInput()
{
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_N))
    {
        CreateFlow("untitled##" + std::to_string(_graph_windows.size()));
    }
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_O))
    {
        LoadFlow();
    }
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S))
    {
        SaveFlow(false);
    }
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_W))
    {
        for (auto it = _graph_windows.begin(); it != _graph_windows.end();)
        {
            if (it->second->IsOpen() && it->second->IsActive())
            {
                it = _graph_windows.erase(it);
                break;
            }
            ++it;
            OnActiveGraphChanged.Broadcast(it->second->GetGraph());
        }
    }
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_S))
    {
        SaveFlow(true);
    }

    for (auto it = _graph_windows.begin(); it != _graph_windows.end();)
    {
        if (it->second->IsOpen())
        {
            ++it;
        }
        else
        {
            it = _graph_windows.erase(it);
        }
    }
}

void Editor::DrawMainMenuBar()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New Flow"))
        {
            CreateFlow("untitled##" + std::to_string(_graph_windows.size()));
        }

        if (ImGui::MenuItem("Load Flow"))
        {
            LoadFlow();
        }

        if (ImGui::MenuItem("Save"))
        {
            SaveFlow(false);
        }

        if (ImGui::MenuItem("Save As"))
        {
            SaveFlow(true);
        }

        if (ImGui::MenuItem("Add Extension"))
        {
            FileStorage::Dialog d;
            std::string filename;
            filename = d.LoadLib(_filestorage.GetExtensionPath());
            _env->LoadModule(filename);
        }

        ImGui::EndMenu();
    }

    if (ed::GetCurrentEditor())
    {
        if (ImGui::BeginMenu("Graph"))
        {
            if (ImGui::MenuItem("Zoom to Content"))
            {
                ed::NavigateToContent();
            }

            ImGui::EndMenu();
        }
    }
}

std::shared_ptr<GraphWindow>& Editor::CreateFlow(std::string name)
{
    auto found = std::find_if(_graph_windows.begin(), _graph_windows.end(),
                              [&](const auto& entry) { return entry.second->GetName() == name; });
    if (found != _graph_windows.end()) return found->second;

    auto graph           = std::make_shared<flow::Graph>(std::move(name), _env);
    auto [graph_view, _] = _graph_windows.emplace(graph->ID(), std::make_shared<GraphWindow>(graph));

    return graph_view->second;
}

void Editor::LoadFlow(std::string file)
{
    json j = _filestorage.Load(file);
    if (j.empty()) return;

    const std::string name = std::filesystem::path(file).filename().replace_extension("").string();

    auto& graph_view = CreateFlow(name);

    graph_view->SetCurrentGraph();
    graph_view->LoadFlow(j);
    graph_view->MarkDirty(false);
    graph_view->GetGraph()->Run();
}

void Editor::SaveFlow(bool save_as)
{
    auto graph_window_it = std::find_if(_graph_windows.begin(), _graph_windows.end(), [](auto& gw) {
        return ed::GetCurrentEditor() == gw.second->GetEditorContext();
    });
    if (graph_window_it == _graph_windows.end()) return;

    auto& graph      = graph_window_it->second->GetGraph();
    auto& graph_view = graph_window_it->second;

    std::string_view name = graph->GetName();
    if (name.find("##") != std::string_view::npos)
    {
        name = name.substr(0, name.find("##"));
    }

    json saved_json = graph_view->SaveFlow();
    auto new_name   = _filestorage.Save(saved_json.dump(4), name, save_as);
    if (!new_name.empty() && name != new_name)
    {
        graph->SetName(new_name);
    }

    graph_view->MarkDirty(false);
}

std::vector<HelloImGui::DockableWindow>::iterator Editor::GetDockableGraphWindowBegin() noexcept
{
    return std::find_if(_params.dockingParams.dockableWindows.begin(), _params.dockingParams.dockableWindows.end(),
                        [this](const auto& w) {
                            auto graph = std::find_if(_graph_windows.begin(), _graph_windows.end(),
                                                      [&](const auto& gw) { return gw.second->GetName() == w.label; });
                            if (graph == _graph_windows.end()) return false;

                            return std::dynamic_pointer_cast<GraphWindow>(graph->second) != nullptr;
                        });
}

FLOW_UI_NAMESPACE_END
