#include "Core.hpp"

#include <imgui.h>
#include <imgui_node_editor.h>

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

void* GetEditorContext() noexcept { return reinterpret_cast<void*>(ed::GetCurrentEditor()); }

FLOW_UI_NAMESPACE_END
