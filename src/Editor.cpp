// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Editor.hpp"

#include "Config.hpp"
#include "EditorNodes.hpp"
#include "FileExplorer.hpp"
#include "ViewFactory.hpp"
#include "Window.hpp"
#include "WindowManager.hpp"
#include "utilities/Conversions.hpp"
#include "windows/ModuleManagerWindow.hpp"
#include "windows/NodeExplorerWindow.hpp"
#include "windows/PropertyWindow.hpp"
#include "windows/ShortcutsWindow.hpp"

#include <flow/core/FunctionNode.hpp>
#include <flow/core/Node.hpp>
#include <flow/core/NodeFactory.hpp>
#include <flow/core/Port.hpp>
#include <flow/core/UUID.hpp>
#include <hello_imgui/hello_imgui.h>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

FLOW_UI_NAMESPACE_BEGIN

using namespace ax;
namespace ed = ax::NodeEditor;
HelloImGui::RunnerParams _params;

void AddDockspace(std::string name, std::string initial_dockspace_name, float ratio, ImGuiDir direction)
{
    HelloImGui::DockingSplit split;
    split.initialDock = std::move(initial_dockspace_name);
    split.newDock     = std::move(name);
    split.direction   = direction;
    split.ratio       = ratio;

    _params.dockingParams.dockingSplits.push_back(std::move(split));
}

const std::filesystem::path default_save_path    = FileExplorer::GetDocumentsPath() / "flows";
const std::filesystem::path default_modules_path = FileExplorer::GetExecutablePath() / "modules";

Editor::Editor(const std::string& initial_file)
    : _window_manager(std::make_unique<WindowManager>()), _input_manager(std::make_unique<CommandManager>())
{
    SetupParams(initial_file);
}

void Editor::Init(const std::string& initial_file)
{
    RegisterInputs();
    RegisterNodes();
    RegisterInputFieldTypes();

    AddDockspace(PropertyDockspace, DefaultDockspace, 0.25f, ImGuiDir_Left);
    AddDockspace("PropertySubSpace", PropertyDockspace, 0.5f, ImGuiDir_Down);
    AddDockspace("ToolbarSpace", DefaultDockspace, 0.1f, ImGuiDir_Up);
    AddDockspace("MiscSpace", DefaultDockspace, 0.25f, ImGuiDir_Down);

    auto node_explorer = std::make_shared<NodeExplorerWindow>(GetEnv());
    _window_manager->OnActiveGraphChanged.Bind(flow::IndexableName{node_explorer->GetName()},
                                               [window = node_explorer](const auto& g) { window->SetActiveGraph(g); });

    auto property_window = std::make_shared<PropertyWindow>();
    _window_manager->OnActiveGraphChanged.Bind(flow::IndexableName{property_window->GetName()},
                                               [=](const auto& g) { property_window->SetCurrentGraph(g); });

    _window_manager->AddWindow(std::move(property_window), PropertyDockspace);
    _window_manager->AddWindow(std::move(node_explorer), "PropertySubSpace");
    _window_manager->AddWindow(std::make_shared<ModuleManagerWindow>(GetEnv(), default_modules_path),
                               "PropertySubSpace", false);
    _window_manager->AddWindow(std::make_shared<ShortcutsWindow>(), PropertyDockspace, false);

    if (!initial_file.empty())
    {
        LoadFlow(initial_file);
    }
    else
    {
        CreateFlow();
    }
}

void Editor::Teardown() { _window_manager->Teardown(); }

void Editor::SetupParams(const std::string& initial_file)
{
#pragma region AppWindowParams
    _params.appWindowParams.windowTitle             = "Flow Editor";
    _params.appWindowParams.borderless              = false;
    _params.appWindowParams.restorePreviousGeometry = true;
    _params.appWindowParams.windowGeometry.size     = {1920, 1080};
#pragma endregion

#pragma region ImGuiWindowParams
    _params.imGuiWindowParams.defaultImGuiWindowType = HelloImGui::DefaultImGuiWindowType::ProvideFullScreenDockSpace;
    _params.imGuiWindowParams.enableViewports        = true;
    _params.imGuiWindowParams.showMenuBar            = true;
    _params.imGuiWindowParams.showMenu_App           = true;
    _params.imGuiWindowParams.showMenu_View          = false;
    _params.imGuiWindowParams.showMenu_View_Themes   = false;
#pragma endregion

#pragma region MiscellaneousParams
    _params.dpiAwareParams.fontRenderingScale  = 1.f;
    _params.dpiAwareParams.dpiWindowSizeFactor = 1.f;

    _params.iniFolderType = HelloImGui::IniFolderType::TempFolder;

    _params.dockingParams.mainDockSpaceNodeFlags = ImGuiDockNodeFlags_AutoHideTabBar;

    _params.fpsIdling.enableIdling = false;
#pragma endregion

#pragma region Callbacks
    _params.callbacks.PostInit = [&] { Init(initial_file); };

    _params.callbacks.BeforeExit = [&] { Teardown(); };

    _params.callbacks.PreNewFrame = [=, this] {
        _input_manager->Handle();
        _window_manager->CleanupDeadWindows();
    };

    _params.callbacks.SetupImGuiStyle = [&] {
        auto& style = ImGui::GetStyle();

        style.WindowBorderSize           = 5.f;
        style.FrameBorderSize            = 2.f;
        style.TabRounding                = 8.f;
        style.TabBarBorderSize           = 0.f;
        style.CellPadding                = ImVec2{7.f, 7.f};
        style.CircleTessellationMaxError = 0.1f;
        style.CurveTessellationTol       = 0.1f;

        auto& imgui_colours = style.Colors;

        imgui_colours[ImGuiCol_WindowBg]          = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_PopupBg]           = ImColor(15, 15, 15, 175);
        imgui_colours[ImGuiCol_Border]            = ImColor(15, 15, 15);
        imgui_colours[ImGuiCol_PopupBg]           = imgui_colours[ImGuiCol_WindowBg];
        imgui_colours[ImGuiCol_FrameBg]           = ImColor(15, 15, 15);
        imgui_colours[ImGuiCol_MenuBarBg]         = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_TitleBg]           = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_TitleBgActive]     = imgui_colours[ImGuiCol_TitleBg];
        imgui_colours[ImGuiCol_Tab]               = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_TabDimmed]         = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_TabHovered]        = ImColor(47, 47, 47);
        imgui_colours[ImGuiCol_TabSelected]       = ImColor(3, 98, 195);
        imgui_colours[ImGuiCol_TabDimmedSelected] = imgui_colours[ImGuiCol_TabSelected];
        imgui_colours[ImGuiCol_Button]            = ImColor(32, 32, 32);
        imgui_colours[ImGuiCol_ButtonHovered]     = ImColor(3, 98, 195);
        imgui_colours[ImGuiCol_ButtonActive]      = ImColor(13, 39, 77);
        imgui_colours[ImGuiCol_ScrollbarBg]       = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_ScrollbarGrab]     = ImColor(86, 86, 86);
        imgui_colours[ImGuiCol_TableBorderLight]  = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_TableBorderStrong] = ImColor(21, 21, 21);
        imgui_colours[ImGuiCol_TableRowBg]        = ImColor(36, 36, 36);
        imgui_colours[ImGuiCol_TableRowBgAlt]     = ImColor(36, 36, 36);
        imgui_colours[ImGuiCol_Header]            = ImColor(47, 47, 47);
        imgui_colours[ImGuiCol_HeaderHovered]     = ImColor(50, 50, 50);
        imgui_colours[ImGuiCol_CheckMark]         = ImColor(3, 98, 195);
    };

    _params.callbacks.LoadAdditionalFonts = [&] {
        auto& config = GetConfig();

        config.DefaultFont    = flow::ui::LoadFont("fonts/DroidSans.ttf", 18.f);
        config.NodeHeaderFont = flow::ui::LoadFont("fonts/DroidSans.ttf", 20.f);
        config.IconFont       = flow::ui::LoadFont("fonts/fontawesome-webfont.ttf", 18.f);

        ImGui::GetIO().FontDefault = std::bit_cast<ImFont*>(config.DefaultFont.get());
    };

    _params.callbacks.ShowMenus = [&] {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Flow"))
            {
                CreateFlow("untitled##" + std::to_string(_window_manager->GetGraphWindows().size()));
            }

            if (ImGui::MenuItem("Load Flow"))
            {
                LoadFlow();
            }

            if (ImGui::MenuItem("Save"))
            {
                SaveFlow();
            }

            ImGui::EndMenu();
        }

        if (!ed::GetCurrentEditor())
        {
            return;
        }

        if (ImGui::BeginMenu("Graph"))
        {
            if (ImGui::MenuItem("Zoom to Content"))
            {
                ed::NavigateToContent();
            }

            ImGui::EndMenu();
        }

        HelloImGui::ShowViewMenu(_params);
    };
#pragma endregion
}

void Editor::RegisterInputs()
{
    // New Graph (Ctrl + N)
    _input_manager->AddCommand(ImGuiMod_Ctrl | ImGuiKey_N, [&] { CreateFlow(); });

    // Open Flow file (Ctrl + O)
    _input_manager->AddCommand(ImGuiMod_Ctrl | ImGuiKey_O, [&] { LoadFlow(); });

    // Save current flow file (Ctrl + S)
    _input_manager->AddCommand(ImGuiMod_Ctrl | ImGuiKey_S, [&] { SaveFlow(); });

    // Save current flow file as (Ctrl + Alt + S)
    _input_manager->AddCommand(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_S, [&] { SaveFlow(); });

    // Close current active graph window (Ctrl + W)
    _input_manager->AddCommand(ImGuiMod_Ctrl | ImGuiKey_W, [&] { _window_manager->CloseActiveGraphWindow(); });
}

void Editor::RegisterNodes()
{
    GetFactory()->OnNodeClassUnregistered.Bind(
        "Unregister", [&](std::string_view class_name) { _window_manager->RemoveUnloadedModuleNode(class_name); });

    GetFactory()->RegisterNodeClass<PreviewNode>("Editor", "Preview");
    GetFactory()->RegisterNodeView<PreviewNodeView, PreviewNode>();

    GetFactory()->RegisterFunction<double(double, double), std::fmod>("Math", "fmod (double)");
    GetFactory()->RegisterFunction<float(float, float), std::fmod>("Math", "fmod (float)");
}

void Editor::RegisterInputFieldTypes()
{
    GetFactory()->RegisterInputType<bool>(false);
    GetFactory()->RegisterInputType<float>(0.f);
    GetFactory()->RegisterInputType<double>(0.0);
    GetFactory()->RegisterInputType<std::int8_t>(0);
    GetFactory()->RegisterInputType<std::int16_t>(0);
    GetFactory()->RegisterInputType<std::int32_t>(0);
    GetFactory()->RegisterInputType<std::int64_t>(0);
    GetFactory()->RegisterInputType<std::uint8_t>(0);
    GetFactory()->RegisterInputType<std::uint16_t>(0);
    GetFactory()->RegisterInputType<std::uint32_t>(0);
    GetFactory()->RegisterInputType<std::uint64_t>(0);
    GetFactory()->RegisterInputType<std::string>("");
    GetFactory()->RegisterInputType<std::chrono::nanoseconds>(std::chrono::nanoseconds::zero());
    GetFactory()->RegisterInputType<std::chrono::microseconds>(std::chrono::microseconds::zero());
    GetFactory()->RegisterInputType<std::chrono::milliseconds>(std::chrono::milliseconds::zero());
    GetFactory()->RegisterInputType<std::chrono::seconds>(std::chrono::seconds::zero());
    GetFactory()->RegisterInputType<std::chrono::minutes>(std::chrono::minutes::zero());
    GetFactory()->RegisterInputType<std::chrono::hours>(std::chrono::hours::zero());
    GetFactory()->RegisterInputType<std::chrono::days>(std::chrono::days::zero());
    GetFactory()->RegisterInputType<std::chrono::months>(std::chrono::months::zero());
    GetFactory()->RegisterInputType<std::chrono::years>(std::chrono::years::zero());
    GetFactory()->RegisterInputType<std::filesystem::path>(std::filesystem::path(""));
}

void Editor::Run() { HelloImGui::Run(_params); }

const std::shared_ptr<GraphWindow>& Editor::CreateFlow(const std::string& name)
{
    return _window_manager->CreateGraphWindow(name, GetEnv());
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
    auto graph_view = _window_manager->GetActiveGraphWindow();
    auto& graph     = graph_view->GetGraph();

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
