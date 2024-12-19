// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Editor.hpp"

#include "Config.hpp"
#include "EditorNodes.hpp"
#include "ViewFactory.hpp"
#include "Window.hpp"
#include "utilities/Conversions.hpp"
#include "windows/ModuleManagerWindow.hpp"

#include <flow/core/Node.hpp>
#include <flow/core/NodeFactory.hpp>
#include <flow/core/Port.hpp>
#include <flow/core/UUID.hpp>
#include <hello_imgui/hello_imgui.h>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

const std::filesystem::path default_save_path    = FileExplorer::GetDocumentsPath() / "flows";
const std::filesystem::path default_modules_path = FileExplorer::GetExecutablePath() / "modules";

HelloImGui::RunnerParams _params;

Editor::Editor(const std::string& initial_file)
{
    _params.appWindowParams.windowTitle             = "Flow Code";
    _params.appWindowParams.borderless              = false;
    _params.appWindowParams.restorePreviousGeometry = true;
    _params.iniFolderType                           = HelloImGui::IniFolderType::TempFolder;
    _params.appWindowParams.windowGeometry.size     = {1920, 1080};

    _params.callbacks.PostInit = [&] {
        GetConfig().RenderBackend = utility::to_RendererBackend(_params.rendererBackendType);

        SetupStyle(GetStyle());
        utility::to_ImGuiStyle(GetStyle());

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

        OnGraphWindowAdded.Broadcast();
        OnGraphWindowAdded.UnbindAll();

        OnGraphWindowRemoved.Broadcast();
        OnGraphWindowRemoved.UnbindAll();
    };

    _params.imGuiWindowParams.showMenu_View_Themes = false;
    _params.fpsIdling.enableIdling                 = false;

    _params.callbacks.SetupImGuiStyle = [&] {
        SetupStyle(GetStyle());
        utility::to_ImGuiStyle(GetStyle());

        auto& imgui_style                      = ImGui::GetStyle();
        imgui_style.CircleTessellationMaxError = 0.1f;
        imgui_style.CurveTessellationTol       = 0.1f;
    };
    _params.callbacks.LoadAdditionalFonts = [&] {
        auto& config = GetConfig();
        LoadFonts(config);
        ImGui::GetIO().FontDefault = std::bit_cast<ImFont*>(config.DefaultFont.get());
    };
    _params.callbacks.ShowMenus = [&] {
        DrawMainMenuBar();
        HelloImGui::ShowViewMenu(_params);
    };
}

void Editor::Init(const std::string& initial_file)
{
    _factory->OnNodeClassUnregistered.Bind("Unregister", [&](std::string_view class_name) {
        for (const auto& [_, gw] : _graph_windows)
        {
            const auto& graph = gw->GetGraph();

            std::set<flow::UUID> nodes_to_remove;
            graph->Visit([&](const auto& node) {
                if (node->GetClass() == class_name)
                {
                    nodes_to_remove.insert(node->ID());
                }
            });

            for (const auto& id : nodes_to_remove)
            {
                graph->RemoveNodeByID(id);
                ed::SetCurrentEditor(std::bit_cast<ed::EditorContext*>(gw->GetEditorContext().get()));
                ed::DeleteNode(std::hash<UUID>{}(id));
            }
        }
    });

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

    AddWindow(std::make_shared<ModuleManagerWindow>(_env, default_modules_path), DefaultDockspace);

    if (!initial_file.empty())
    {
        LoadFlow(initial_file);
    }
    else
    {
        CreateFlow("untitled##0");
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

void Editor::AddWindow(std::shared_ptr<Window> new_window, const std::string& dockspace, bool show)
{
    auto& window = _windows.emplace_back(std::move(new_window));

    HelloImGui::DockableWindow dockable_window;
    dockable_window.label            = window->GetName();
    dockable_window.dockSpaceName    = dockspace;
    dockable_window.GuiFunction      = [=] { window->Draw(); };
    dockable_window.imGuiWindowFlags = ImGuiWindowFlags_NoCollapse;
    dockable_window.isVisible        = show;

    window->Init();
    HelloImGui::AddDockableWindow(std::move(dockable_window));
}

void Editor::AddDockspace(std::string name, std::string initial_dockspace_name, float ratio,
                          DockspaceSplitDirection direction)
{
    HelloImGui::DockingSplit split;
    split.initialDock = std::move(initial_dockspace_name);
    split.newDock     = std::move(name);
    split.direction   = utility::to_ImGuiDir(direction);
    split.ratio       = ratio;

    _params.dockingParams.dockingSplits.push_back(std::move(split));
}

void* Editor::GetContext() const noexcept { return reinterpret_cast<void*>(ImGui::GetCurrentContext()); }

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
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_S) ||
        ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_S))
    {
        SaveFlow();
    }
    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_W))
    {
        for (auto it = _graph_windows.begin(); it != _graph_windows.end();)
        {
            if (it->second->IsOpen() && it->second->IsActive())
            {
                OnGraphWindowRemoved.Bind(IndexableName{it->second->GetName()},
                                          [name = it->second->GetName()] { HelloImGui::RemoveDockableWindow(name); });
                it = _graph_windows.erase(it);
                break;
            }
            ++it;
            OnActiveGraphChanged.Broadcast(it->second->GetGraph());
        }
    }

    for (auto it = _graph_windows.begin(); it != _graph_windows.end();)
    {
        if (it->second->IsOpen())
        {
            ++it;
        }
        else
        {
            OnGraphWindowRemoved.Bind(IndexableName{it->second->GetName()},
                                      [name = it->second->GetName()] { HelloImGui::RemoveDockableWindow(name); });
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
            SaveFlow();
        }

        if (ImGui::MenuItem("Save As"))
        {
            SaveFlow();
        }

        if (ImGui::MenuItem("Import Module"))
        {
            const auto filename        = FileExplorer::Load(default_modules_path, "Flow Modules", "so,dll,dylib");
            const auto new_module_file = default_modules_path / filename.filename();

            if (new_module_file != filename)
            {
                try
                {
                    std::filesystem::create_directory(default_modules_path);
                    std::filesystem::copy_file(filename, new_module_file, std::filesystem::copy_options::skip_existing);
                }
                catch (const std::exception& e)
                {
                    SPDLOG_ERROR("Caught exception while loading module {}: {}", filename.string(), e.what());
                }
            }
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

    auto graph           = std::make_shared<flow::Graph>(name, _env);
    auto [graph_view, _] = _graph_windows.emplace(graph->ID(), std::make_shared<GraphWindow>(graph));
    OnGraphWindowAdded.Bind(IndexableName{name}, [=, this, graph_view = graph_view->second] {
        HelloImGui::DockableWindow graph_window;
        graph_window.label         = name;
        graph_window.dockSpaceName = DefaultDockspace;
        graph_window.GuiFunction   = [this, gv = std::move(graph_view)]() {
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

        HelloImGui::AddDockableWindow(std::move(graph_window));
    });

    return graph_view->second;
}

void Editor::LoadFlow(const std::filesystem::path& filename)
{
    auto file_path = FileExplorer::Load(default_save_path / filename, "Flow files", "flow");

    json j;
    try
    {
        std::ifstream i;
        i.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        i.open(file_path);
        i >> j;
        i.close();
    }
    catch (const std::exception& e)
    {
        SPDLOG_ERROR("Failed to load file '{0}: {1}", file_path.filename().string(), e.what());
        return;
    }

    const std::string name = file_path.filename().replace_extension("").string();

    auto& graph_view = CreateFlow(name);

    graph_view->SetCurrentGraph();
    graph_view->LoadFlow(j);
    graph_view->MarkDirty(false);
    graph_view->GetGraph()->Run();
}

void Editor::SaveFlow()
{
    auto graph_window_it = std::find_if(_graph_windows.begin(), _graph_windows.end(), [](auto& gw) {
        return ed::GetCurrentEditor() == std::bit_cast<ed::EditorContext*>(gw.second->GetEditorContext().get());
    });
    if (graph_window_it == _graph_windows.end()) return;

    auto& graph      = graph_window_it->second->GetGraph();
    auto& graph_view = graph_window_it->second;

    std::string name{graph->GetName()};
    if (name.find("##") != std::string::npos)
    {
        name = name.substr(0, name.find("##"));
    }

    json saved_json     = graph_view->SaveFlow();
    auto new_path       = FileExplorer::Save(default_save_path / (name + ".flow"), saved_json.dump(4));
    const auto new_name = new_path.replace_extension("").filename().string();

    if (!new_name.empty() && name != new_name)
    {
        graph->SetName(new_name);
    }

    graph_view->MarkDirty(false);
}

FLOW_UI_NAMESPACE_END
