#include "ShortcutsWindow.hpp"

#include "widgets/Table.hpp"
#include "widgets/Text.hpp"

#include <imgui.h>

#include <memory>

FLOW_UI_NAMESPACE_START

ShortcutsWindow::ShortcutsWindow() : Window("Shortcuts") {}

#ifdef FLOW_APPLE
const std::string ctrl_key_str = "Cmd";
const std::string alt_key_str  = "Option";
#else
const std::string ctrl_key_str = "Ctrl";
const std::string alt_key_str  = "Alt";
#endif

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

void ShortcutsWindow::Draw()
{
    auto window_shortcuts_table = widgets::Table("Shortcuts", 2);

    AddShortcutText(window_shortcuts_table, "New Window", {ImGuiKey_LeftCtrl, ImGuiKey_N});
    AddShortcutText(window_shortcuts_table, "Close Window", {ImGuiKey_LeftCtrl, ImGuiKey_W});

    auto graph_shortcuts_table = widgets::Table("Shortcuts", 2);

    AddShortcutText(graph_shortcuts_table, "Break Link", {ImGuiKey_LeftAlt, ImGuiKey_MouseLeft});

    AddShortcutText(graph_shortcuts_table, "Copy", {ImGuiKey_LeftCtrl, ImGuiKey_C});

    AddShortcutText(graph_shortcuts_table, "Cut (Experimental)", {ImGuiKey_LeftCtrl, ImGuiKey_X}, Colour(244, 129, 36));

    AddShortcutText(graph_shortcuts_table, "Paste", {ImGuiKey_LeftCtrl, ImGuiKey_V});

    AddShortcutText(graph_shortcuts_table, "Duplicate", {ImGuiKey_LeftCtrl, ImGuiKey_D});

    AddShortcutText(graph_shortcuts_table, "Undo (Experimental)", {ImGuiKey_LeftCtrl, ImGuiKey_Z},
                    Colour(244, 129, 36));

    AddShortcutText(graph_shortcuts_table, "Redo (Experimental)", {ImGuiKey_LeftCtrl, ImGuiKey_Y},
                    Colour(244, 129, 36));

    AddShortcutText(graph_shortcuts_table, "Delete Selection", {ImGuiKey_Delete});

    AddShortcutText(graph_shortcuts_table, "Focus", {ImGuiKey_F});

    widgets::Text("Window Shortcuts")();
    ImGui::Separator();
    window_shortcuts_table();

    widgets::Text("Graph Shortcuts")();
    ImGui::Separator();
    graph_shortcuts_table();
}

FLOW_UI_NAMESPACE_END
