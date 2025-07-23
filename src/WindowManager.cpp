#include "WindowManager.hpp"

#include <flow/core/Graph.hpp>
#include <hello_imgui/hello_imgui.h>
#include <imgui_node_editor.h>

FLOW_UI_NAMESPACE_BEGIN

using namespace ax;
namespace ed = ax::NodeEditor;

void WindowManager::Teardown()
{
    for (auto& window : _windows)
    {
        window->Teardown();
    }
}

void WindowManager::AddWindow(std::shared_ptr<Window> new_window, const std::string& dockspace, bool visible_by_default)
{
    auto& window = _windows.emplace_back(std::move(new_window));

    HelloImGui::DockableWindow dockable_window;
    dockable_window.label            = window->GetName();
    dockable_window.dockSpaceName    = dockspace;
    dockable_window.GuiFunction      = [=] { window->Draw(); };
    dockable_window.imGuiWindowFlags = ImGuiWindowFlags_NoCollapse;
    dockable_window.isVisible        = visible_by_default;

    window->Init();
    HelloImGui::AddDockableWindow(std::move(dockable_window));
}

const std::shared_ptr<GraphWindow>& WindowManager::CreateGraphWindow(std::string name, const std::shared_ptr<Env>& env)
{
    if (name.empty())
    {
        name = "untitled##" + std::to_string(_graph_windows.size());
    }

    auto found = std::find_if(_graph_windows.begin(), _graph_windows.end(),
                              [&](const auto& entry) { return entry.second->GetName() == name; });
    if (found != _graph_windows.end()) return found->second;

    auto graph           = std::make_shared<flow::Graph>(name, env);
    auto [graph_view, _] = _graph_windows.emplace(graph->ID(), std::make_shared<GraphWindow>(graph));

    HelloImGui::DockableWindow graph_window;
    graph_window.label         = name;
    graph_window.dockSpaceName = DefaultDockspace;
    graph_window.GuiFunction   = [this, gv = graph_view->second]() {
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

    return graph_view->second;
}

std::shared_ptr<GraphWindow> WindowManager::GetActiveGraphWindow() const noexcept
{
    for (const auto& [_, gw] : _graph_windows)
    {
        if (gw->IsOpen() && gw->IsActive())
        {
            return gw;
        }
    }

    return nullptr;
}

void WindowManager::CleanupDeadWindows()
{
    for (auto it = _graph_windows.begin(); it != _graph_windows.end();)
    {
        if (it->second->IsOpen())
        {
            ++it;
        }
        else
        {
            HelloImGui::RemoveDockableWindow(it->second->GetName());
            it = _graph_windows.erase(it);
        }
    }
}

void WindowManager::CloseActiveGraphWindow()
{
    for (auto it = _graph_windows.begin(); it != _graph_windows.end();)
    {
        if (it->second->IsOpen() && it->second->IsActive())
        {
            HelloImGui::RemoveDockableWindow(it->second->GetName());
            it = _graph_windows.erase(it);
            break;
        }
        ++it;
        OnActiveGraphChanged.Broadcast(it->second->GetGraph());
    }
}

void WindowManager::RemoveUnloadedModuleNode(std::string_view class_name)
{
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
            ed::SetCurrentEditor(reinterpret_cast<ed::EditorContext*>(gw->GetEditorContext().get()));
            ed::DeleteNode(std::hash<UUID>{}(id));
        }
    }
}

FLOW_UI_NAMESPACE_END
