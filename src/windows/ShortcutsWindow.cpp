#include "ShortcutsWindow.hpp"

#include "widgets/Table.hpp"
#include "widgets/Text.hpp"

#include <imgui.h>

#include <memory>

FLOW_UI_NAMESPACE_START

std::shared_ptr<widgets::Text> ShorcutTextBuilder(std::initializer_list<ImGuiKey> keys)
{
    std::string text = "";
    for (auto it = keys.begin(); it != std::prev(keys.end()); ++it)
    {
        text += std::string(ImGui::GetKeyName(*it)) + " + ";
    }
    text += ImGui::GetKeyName(*std::prev(keys.end()));

    return std::make_shared<widgets::Text>(text);
}

void AddShortcutText(widgets::Table& table, const std::string& name, std::initializer_list<ImGuiKey> keys,
                     const Colour& colour = Colour())
{
    table.AddEntry(std::make_shared<widgets::Text>(name, colour));
    table.AddEntry(ShorcutTextBuilder(keys));
}

auto window_shortcuts = widgets::Table("Window Shortcuts", 2);
auto graph_shortcuts  = widgets::Table("Graph Shortcuts", 2);

ShortcutsWindow::ShortcutsWindow() : Window("Shortcuts")
{
    AddShortcutText(window_shortcuts, "New Flow", {ImGuiKey_LeftCtrl, ImGuiKey_N});
    AddShortcutText(window_shortcuts, "Open Flow", {ImGuiKey_LeftCtrl, ImGuiKey_O});
    AddShortcutText(window_shortcuts, "Save Flow", {ImGuiKey_LeftCtrl, ImGuiKey_S});
    AddShortcutText(window_shortcuts, "Save Flow As", {ImGuiKey_LeftCtrl, ImGuiKey_LeftAlt, ImGuiKey_S});
    AddShortcutText(window_shortcuts, "Close Flow", {ImGuiKey_LeftCtrl, ImGuiKey_W});

    AddShortcutText(graph_shortcuts, "Break Link", {ImGuiKey_LeftAlt, ImGuiKey_MouseLeft});
    AddShortcutText(graph_shortcuts, "Copy", {ImGuiKey_LeftCtrl, ImGuiKey_C});
    AddShortcutText(graph_shortcuts, "Cut (Experimental)", {ImGuiKey_LeftCtrl, ImGuiKey_X}, Colour(244, 129, 36));
    AddShortcutText(graph_shortcuts, "Duplicate", {ImGuiKey_LeftCtrl, ImGuiKey_D});
    AddShortcutText(graph_shortcuts, "Paste", {ImGuiKey_LeftCtrl, ImGuiKey_V});
    AddShortcutText(graph_shortcuts, "Delete Selection", {ImGuiKey_Delete});
    AddShortcutText(graph_shortcuts, "Focus", {ImGuiKey_F});
    AddShortcutText(graph_shortcuts, "Undo (Experimental)", {ImGuiKey_LeftCtrl, ImGuiKey_Z}, Colour(244, 129, 36));
    AddShortcutText(graph_shortcuts, "Redo (Experimental)", {ImGuiKey_LeftCtrl, ImGuiKey_Y}, Colour(244, 129, 36));
}

void ShortcutsWindow::Draw()
{
    widgets::Text("Window Shortcuts")();
    ImGui::Separator();
    window_shortcuts();

    widgets::Text("Graph Shortcuts")();
    ImGui::Separator();
    graph_shortcuts();
}

FLOW_UI_NAMESPACE_END
