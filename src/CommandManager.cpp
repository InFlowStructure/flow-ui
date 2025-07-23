#include "CommandManager.hpp"

#include <imgui.h>

FLOW_UI_NAMESPACE_BEGIN

void CommandManager::Handle()
{
    for (const auto& [chord, event] : _input_events)
    {
        if (ImGui::IsKeyChordPressed(chord))
        {
            event();
        }
    }
}

void CommandManager::AddCommand(int key_chord, Event<>&& event) { _input_events.emplace(key_chord, std::move(event)); }

FLOW_UI_NAMESPACE_END
