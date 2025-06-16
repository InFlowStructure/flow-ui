#include "ShortcutsWindow.hpp"

#include "widgets/Table.hpp"
#include "widgets/Text.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <memory>

FLOW_UI_NAMESPACE_BEGIN

std::shared_ptr<widgets::Text> ShorcutTextBuilder(ImGuiKeyChord chord)
{
    return std::make_shared<widgets::Text>(ImGui::GetKeyChordName(chord));
}

void AddShortcutText(widgets::Table& table, const std::string& name, ImGuiKeyChord keys,
                     const Colour& colour = Colour())
{
    table.AddEntry(std::make_shared<widgets::Text>(widgets::Text(name).SetColour(colour)));
    table.AddEntry(ShorcutTextBuilder(keys));
}

auto window_shortcuts = widgets::Table("Window Shortcuts", 2);
auto graph_shortcuts  = widgets::Table("Graph Shortcuts", 2);

ShortcutsWindow::ShortcutsWindow() : Window("Shortcuts")
{
    AddShortcutText(window_shortcuts, "New Flow", ImGuiMod_Ctrl | ImGuiKey_N);
    AddShortcutText(window_shortcuts, "Open Flow", ImGuiMod_Ctrl | ImGuiKey_O);
    AddShortcutText(window_shortcuts, "Save Flow", ImGuiMod_Ctrl | ImGuiKey_S);
    AddShortcutText(window_shortcuts, "Save Flow As", ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_S);
    AddShortcutText(window_shortcuts, "Close Flow", ImGuiMod_Ctrl | ImGuiKey_W);

    AddShortcutText(graph_shortcuts, "Break Link", ImGuiMod_Alt | ImGuiKey_MouseLeft);
    AddShortcutText(graph_shortcuts, "Copy", ImGuiMod_Ctrl | ImGuiKey_C);
    AddShortcutText(graph_shortcuts, "Cut (Experimental)", ImGuiMod_Ctrl | ImGuiKey_X, Colour(244, 129, 36));
    AddShortcutText(graph_shortcuts, "Duplicate", ImGuiMod_Ctrl | ImGuiKey_D);
    AddShortcutText(graph_shortcuts, "Paste", ImGuiMod_Ctrl | ImGuiKey_V);
    AddShortcutText(graph_shortcuts, "Delete Selection", ImGuiKey_Delete);
    AddShortcutText(graph_shortcuts, "Focus", ImGuiKey_F);
    AddShortcutText(graph_shortcuts, "Undo (Experimental)", ImGuiMod_Ctrl | ImGuiKey_Z, Colour(244, 129, 36));
    AddShortcutText(graph_shortcuts, "Redo (Experimental)", ImGuiMod_Ctrl | ImGuiKey_Y, Colour(244, 129, 36));
}

void ShortcutsWindow::Draw()
{
    widgets::Text("Window Shortcuts").Draw();
    ImGui::Separator();
    window_shortcuts.Draw();

    widgets::Text("Graph Shortcuts").Draw();
    ImGui::Separator();
    graph_shortcuts.Draw();
}

FLOW_UI_NAMESPACE_END
