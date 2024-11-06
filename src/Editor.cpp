// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "Editor.hpp"

#include "Config.hpp"
#include "ImageInspectorWindow.hpp"
#include "LogWindow.hpp"
#include "PropertyWindow.hpp"
#include "Texture.hpp"
#include "ViewFactory.hpp"
#include "Widgets.hpp"

#include <flow/core/Node.hpp>
#include <flow/core/NodeFactory.hpp>
#include <flow/core/Port.hpp>
#include <flow/core/UUID.hpp>
#include <hello_imgui/icons_font_awesome_6.h>
#include <hello_imgui/internal/image_abstract.h>
#include <hello_imgui/internal/image_dx11.h>
#include <hello_imgui/internal/image_metal.h>
#include <hello_imgui/internal/image_opengl.h>
#include <hello_imgui/internal/image_vulkan.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

FLOW_UI_NAMESPACE_START
struct OutputNodeView : flow::ui::NodeView
{
    using flow::ui::NodeView::NodeView;

    void Draw() override
    {
        _builder->Begin(this->ID());

        auto input_it = std::find_if(Inputs.begin(), Inputs.end(), [](const auto& in) { return in->Name() == "in"; });
        const auto& input = *input_it;

        input->Draw();

        ImGui::SameLine();

        if (flow::ui::GetConfig().IconFont)
        {
            ImGui::PushFont(GetConfig().IconFont);
        }

        const bool should_copy = ImGui::Button(ICON_FA_COPY);

        if (flow::ui::GetConfig().IconFont)
        {
            ImGui::PopFont();
        }

        if (auto data = input->GetData())
        {
            const auto& factory = Node->GetEnv()->GetFactory();
            if (auto texture_data = factory->Convert<flow::ui::Texture>(data))
            {
                flow::ui::Texture& texture = texture_data->Get();
                ImGui::Image(texture.ID, texture.Size);

                if (should_copy)
                {
                    // TODO: Copy image data not implemented
                }
                _builder->End();
                return;
            }

            std::string data_str = data->ToString();
            if (!data_str.empty())
            {
                if (should_copy)
                {
                    ImGui::SetClipboardText(data_str.c_str());
                }

                ImGui::TextUnformatted(data_str.c_str());
            }
        }

        _builder->End();
    }
};

struct PreviewNode : public flow::Node
{
    explicit PreviewNode(const std::string& uuid_str, const std::string& name, std::shared_ptr<flow::Env> env)
        : flow::Node(uuid_str, flow::TypeName_v<PreviewNode>, name, std::move(env))
    {
        AddInput<std::any>("in", "");
    }

    virtual ~PreviewNode() = default;

    void Compute() override {}
};

static std::optional<Texture> InspectedTexture;
struct TextureInspectorNodeView : NodeView
{
    using NodeView::NodeView;

    void Draw() override
    {
        NodeView::Draw();

        if (auto data = Node->GetEnv()->GetFactory()->Convert<Texture>(Inputs.at(0)->GetData()))
        {
            InspectedTexture = data->Get();
            return;
        }
        InspectedTexture = std::nullopt;
    }
};

struct TextureInspectorNode : public flow::Node
{
    explicit TextureInspectorNode(const std::string& uuid_str, const std::string& name, std::shared_ptr<flow::Env> env)
        : flow::Node(uuid_str, flow::TypeName_v<TextureInspectorNode>, name, std::move(env))
    {
        AddInput<std::any>("in", "");
    }

    virtual ~TextureInspectorNode() { InspectedTexture = std::nullopt; }

    void Compute() override {}
};

Editor::Editor(const std::string& initial_file)
{
    _windows.emplace_back(std::make_unique<flow::ui::LogWindow>());
    _windows.emplace_back(std::make_unique<flow::ui::PropertyWindow>(_env));
    _windows.emplace_back(std::make_unique<flow::ui::ImageInspectorWindow>(InspectedTexture));

    _params.appWindowParams.windowTitle             = "Flow Code";
    _params.appWindowParams.borderless              = false;
    _params.appWindowParams.restorePreviousGeometry = true;
    _params.iniFolderType                           = HelloImGui::IniFolderType::TempFolder;
    _params.appWindowParams.windowGeometry.size     = {1920, 1080};

    HelloImGui::DockingParams dockingParams;
    dockingParams.dockingSplits   = SplitDockSpace();
    dockingParams.dockableWindows = BuildDockableWindows();

    const std::size_t initial_window_count = _windows.size();

    _params.callbacks.PostInit = [&] {
        flow::ui::GetConfig().RenderBackend = _params.rendererBackendType;
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

    _params.dockingParams = std::move(dockingParams);

    _params.callbacks.PreNewFrame = [=, this] {
        HandleInput();

        auto& graphs           = GetGraphWindows();
        auto& dockable_windows = _params.dockingParams.dockableWindows;

        auto it = std::remove_if(
            std::next(dockable_windows.begin(), initial_window_count), dockable_windows.end(), [&](auto&& dw) {
                return std::none_of(graphs.begin(), graphs.end(),
                                    [&](auto& g) { return dw.label == g.second->GetGraph()->GetName(); });
            });

        if (it != dockable_windows.end())
        {
            dockable_windows.erase(it, dockable_windows.end());
        }

        for (auto& [_, graph_view] : graphs)
        {
            const auto& name = graph_view->GetGraph()->GetName();
            if (!!_params.dockingParams.dockableWindowOfName(name)) continue;

            HelloImGui::DockableWindow graph_window;
            graph_window.label                  = name;
            graph_window.dockSpaceName          = flow::ui::DefaultDockspaces::Main;
            graph_window.GuiFunction            = [gv = graph_view]() { gv->Draw(); };
            graph_window.includeInViewMenu      = false;
            graph_window.callBeginEnd           = false;
            graph_window.focusWindowAtNextFrame = true;
            graph_window.imGuiWindowFlags       = ImGuiWindowFlags_NoCollapse;

            dockable_windows.push_back(graph_window);

            // TODO(trigaux): This will reset all windows, which may be undesirable.
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
    _env->GetFactory()->RegisterNodeClass<TextureInspectorNode>("Editor", "Inspector");

    _factory->RegisterNodeView<OutputNodeView, PreviewNode>();
    _factory->RegisterNodeView<TextureInspectorNodeView, TextureInspectorNode>();

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
        SetPropertyWindowGraph(std::move(graph));
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

void flow::ui::Editor::DrawMainMenuBar()
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

std::vector<HelloImGui::DockingSplit> Editor::SplitDockSpace() const
{
    HelloImGui::DockingSplit split_main_misc;
    split_main_misc.initialDock = DefaultDockspaces::Main;
    split_main_misc.newDock     = DefaultDockspaces::Misc;
    split_main_misc.direction   = ImGuiDir_Down;
    split_main_misc.ratio       = 0.25f;

    HelloImGui::DockingSplit split_main_property;
    split_main_property.initialDock = DefaultDockspaces::Main;
    split_main_property.newDock     = DefaultDockspaces::Property;
    split_main_property.direction   = ImGuiDir_Left;
    split_main_property.ratio       = 0.2f;

    return {split_main_misc, split_main_property};
}

std::vector<HelloImGui::DockableWindow> Editor::BuildDockableWindows()
{
    std::vector<HelloImGui::DockableWindow> dockable_windows;
    for (const auto& window : _windows)
    {
        HelloImGui::DockableWindow dockable_window;
        dockable_window.label            = window->GetName();
        dockable_window.dockSpaceName    = window->GetDockspaceName();
        dockable_window.GuiFunction      = [&] { window->Draw(); };
        dockable_window.imGuiWindowFlags = ImGuiWindowFlags_NoCollapse;

        dockable_windows.push_back(std::move(dockable_window));
    }

    return dockable_windows;
}

std::shared_ptr<GraphWindow>& Editor::CreateFlow(std::string name)
{
    auto found = std::find_if(_graph_windows.begin(), _graph_windows.end(),
                              [&](const auto& entry) { return entry.second->GetName() == name; });
    if (found != _graph_windows.end()) return found->second;

    auto graph           = std::make_shared<flow::Graph>(std::move(name), _env);
    auto [graph_view, _] = _graph_windows.emplace(graph->ID(), std::make_shared<GraphWindow>(graph));

    SetPropertyWindowGraph(graph);

    return graph_view->second;
}

void Editor::LoadFlow(std::string file)
{
    json j = _filestorage.Load(file);
    if (j.empty()) return;

    const std::string name = std::filesystem::path(file).filename().replace_extension("").string();

    SetShouldReloadWindows(true);

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

void Editor::SetPropertyWindowGraph(std::shared_ptr<flow::Graph> graph)
{
    auto property_window_it =
        std::find_if(_windows.begin(), _windows.end(), [](auto&& w) { return w->GetName() == PropertyWindow::Name; });
    if (property_window_it != _windows.end())
    {
        if (auto* property_window = static_cast<PropertyWindow*>(property_window_it->get()))
        {
            property_window->SetCurrentGraph(std::move(graph));
        }
    }
}
FLOW_UI_NAMESPACE_END
