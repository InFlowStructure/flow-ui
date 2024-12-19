#include "Core.hpp"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <bit>

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

std::unique_ptr<EditorContext> GetEditorContext() noexcept
{
    return std::unique_ptr<EditorContext>(std::bit_cast<EditorContext*>(ed::GetCurrentEditor()));
}

FLOW_UI_NAMESPACE_END
