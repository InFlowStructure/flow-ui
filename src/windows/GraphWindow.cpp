// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "GraphWindow.hpp"

#include "Config.hpp"
#include "ConnectionView.hpp"
#include "NodeView.hpp"
#include "PortView.hpp"
#include "ViewFactory.hpp"
#include "utilities/Conversions.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/NodeFactory.hpp>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>
#include <imgui_stdlib.h>
#include <spdlog/spdlog.h>

#include <set>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ImVec2, x, y);

FLOW_UI_NAMESPACE_START

using namespace ax;
namespace ed = ax::NodeEditor;

void ContextMenu::operator()() noexcept
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 5));
    ImGui::PushStyleColor(ImGuiCol_PopupBg, IM_COL32(15, 15, 15, 240));

    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(315, 400));
    if (!ImGui::BeginPopup("Create New Node", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking))
    {
        is_focused  = false;
        node_lookup = "";
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        return;
    }

    if (node_lookup.empty() && !is_focused)
    {
        ImGui::SetKeyboardFocusHere(0);
        is_focused = true;
    }

    ImGui::BeginHorizontal("search");

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 20.f);
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(36, 36, 36, 255));

    ImGui::SetNextItemAllowOverlap();
    ImGui::InputText("##Search", &node_lookup, 0);

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImGui::SetCursorPosX((ImGui::GetItemRectMax() - ImGui::GetItemRectMin()).x - 18);

    ImGui::PushFont(std::bit_cast<ImFont*>(GetConfig().IconFont.get()));
    ImGui::TextUnformatted(ICON_FA_MAGNIFYING_GLASS);
    ImGui::PopFont();

    ImGui::EndHorizontal();

    auto registered_nodes = _factory->GetCategories();

    if (!node_lookup.empty())
    {
        auto partial_match_func = [&](const auto& entry) -> bool {
            auto [_, class_name]     = entry;
            std::string display_name = _factory->GetFriendlyName(class_name);
            std::string filter       = node_lookup;

            const auto& to_lower = [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); };
            std::transform(filter.begin(), filter.end(), filter.begin(), to_lower);
            std::transform(class_name.begin(), class_name.end(), class_name.begin(), to_lower);
            std::transform(display_name.begin(), display_name.end(), display_name.begin(), to_lower);

            return std::string_view(display_name).find(filter) == std::string_view::npos &&
                   std::string_view(class_name).find(filter) == std::string_view::npos;
        };

        std::erase_if(registered_nodes, partial_match_func);
    }

    if (ImGui::BeginChild("Categories"))
    {
        std::set<std::string> categories;
        for (const auto& [category, _] : registered_nodes)
        {
            categories.insert(category);
        }

        for (const auto& category : categories)
        {
            DrawPopupCategory(category, registered_nodes);
        }

        ImGui::EndChild();
    }

    ImGui::EndPopup();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void ContextMenu::DrawPopupCategory(const std::string& category, const flow::CategoryMap& registered_nodes)
{
    if (!ImGui::TreeNodeEx(category.c_str(), node_lookup.empty() ? 0 : ImGuiTreeNodeFlags_DefaultOpen)) return;

    auto [begin_it, end_it] = registered_nodes.equal_range(category);
    for (auto it = begin_it; it != end_it; ++it)
    {
        auto class_name   = it->second;
        auto display_name = _factory->GetFriendlyName(class_name);

        ImGui::Bullet();
        if (ImGui::MenuItem(display_name.c_str()))
        {
            OnSelection(class_name, display_name);
            break;
        }
    }
    ImGui::TreePop();
}

namespace
{
inline void DrawLabel(const char* label, ImColor color)
{
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
    auto size = ImGui::CalcTextSize(label);

    auto padding = ImGui::GetStyle().FramePadding;
    auto spacing = ImGui::GetStyle().ItemSpacing;

    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

    auto rectMin = ImGui::GetCursorScreenPos() - padding;
    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

    auto drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
    ImGui::TextUnformatted(label);
};

inline std::pair<ImVec2, ImVec2> GetContainerNodeBounds()
{
    ed::NodeId ids[128];
    int result = ed::GetSelectedNodes(ids, 128);

    std::vector<std::pair<ImVec2, ImVec2>> node_bounds;
    std::for_each_n(std::begin(ids), result, [&](const auto& id) {
        node_bounds.emplace_back(ed::GetNodePosition(id), ed::GetNodePosition(id) + ed::GetNodeSize(id));
    });

    if (node_bounds.empty()) return {{}, {}};

    const auto min_x_id = std::min_element(node_bounds.begin(), node_bounds.end(),
                                           [](auto&& a, auto&& b) { return a.first.x < b.first.x; });
    const auto min_y_id = std::min_element(node_bounds.begin(), node_bounds.end(),
                                           [](auto&& a, auto&& b) { return a.first.y < b.first.y; });
    const auto max_x_id = std::max_element(node_bounds.begin(), node_bounds.end(),
                                           [](auto&& a, auto&& b) { return a.second.x < b.second.x; });
    const auto max_y_id = std::max_element(node_bounds.begin(), node_bounds.end(),
                                           [](auto&& a, auto&& b) { return a.second.y < b.second.y; });

    const auto min_x = min_x_id->first.x;
    const auto min_y = min_y_id->first.y;
    const auto max_x = max_x_id->second.x;
    const auto max_y = max_y_id->second.y;

    const auto size = ImVec2(std::abs(max_x - min_x), std::abs(max_y - min_y));

    return {ImVec2(min_x, min_y) - ImVec2(15, 40), size + ImVec2(30, 30)};
}

bool AcceptUndo() { return ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Z); }

bool AcceptRedo()
{
    return (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_Y) ||
            ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z));
}

bool AcceptComment() { return ImGui::IsKeyChordPressed(ImGuiMod_Alt | ImGuiKey_C); }

ed::Detail::EditorContext* GetEditorDetailContext(const std::unique_ptr<EditorContext>& p)
{
    return std::bit_cast<ed::Detail::EditorContext*>(p.get());
}
} // namespace

constexpr GraphWindow::ActionType operator&(const GraphWindow::ActionType& a, const GraphWindow::ActionType& b)
{
    return static_cast<GraphWindow::ActionType>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

GraphWindow::GraphWindow(std::shared_ptr<flow::Graph> graph)
    : Window(graph->GetName()), _graph{std::move(graph)}, node_context_menu{GetEnv()->GetFactory()}
{
    ed::Config config;
    config.UserPointer      = this;
    config.EnableSmoothZoom = true;
    config.SettingsFile     = "";

    config.SaveNodeSettings = []([[maybe_unused]] ed::NodeId nodeId, [[maybe_unused]] const char* data,
                                 [[maybe_unused]] std::size_t size, ed::SaveReasonFlags reason,
                                 void* userPointer) -> bool {
        if (reason == ed::SaveReasonFlags::None)
        {
            SPDLOG_TRACE("Nothing happened during node save.");
            return true;
        }

        GraphWindow* self = std::bit_cast<GraphWindow*>(userPointer);

        if ((reason & ed::SaveReasonFlags::Position) == ed::SaveReasonFlags::Position ||
            (reason & ed::SaveReasonFlags::AddNode) == ed::SaveReasonFlags::AddNode ||
            (reason & ed::SaveReasonFlags::RemoveNode) == ed::SaveReasonFlags::RemoveNode ||
            (reason & ed::SaveReasonFlags::Size) == ed::SaveReasonFlags::Size ||
            (reason & ed::SaveReasonFlags::User) == ed::SaveReasonFlags::User)
        {
            self->MarkDirty(true);
        }

        return true;
    };

    _editor_ctx = std::unique_ptr<EditorContext>(std::bit_cast<EditorContext*>(ed::CreateEditor(&config)));

    auto& canvas_view    = const_cast<ImGuiEx::CanvasView&>(GetEditorDetailContext(_editor_ctx)->GetView());
    canvas_view.Origin   = {0, 0};
    canvas_view.InvScale = 75;

    auto& ed_style           = GetEditorDetailContext(GetEditorContext())->GetStyle();
    ed_style.NodeBorderWidth = 0.5f;
    ed_style.FlowDuration    = 1.f;

    auto& ed_colours = ed_style.Colors;
    auto& colours    = GetStyle().Colours.EditorColours;

    std::for_each(std::begin(colours), std::end(colours), [&](const auto& p) {
        const auto& [i, c]                       = p;
        ed_colours[utility::to_EdStyleColour(i)] = utility::to_ImColor(c);
    });

    _graph->Visit([](const auto& node) { return node->Start(); });

    node_context_menu.OnSelection = [this, factory = std::dynamic_pointer_cast<ViewFactory>(GetEnv()->GetFactory())](
                                        const auto& class_name, const auto& display_name) {
        CreateNode(class_name, display_name);
        ImGui::CloseCurrentPopup();
    };

    _graph->OnNodeAdded.Bind("CreateNodeView", [this](const auto& n) {
        const auto factory = std::dynamic_pointer_cast<ViewFactory>(GetEnv()->GetFactory());
        auto node_view     = factory->CreateNodeView(n);
        _item_views.emplace(node_view->ID(), node_view);
        ed::SetNodePosition(node_view->ID(), {_open_popup_position.x, _open_popup_position.y});

        if (auto start_pin = _new_node_link_pin)
        {
            auto& pins = start_pin->Kind == PortType::Input ? node_view->Outputs : node_view->Inputs;
            for (auto& pin : pins)
            {
                if (!start_pin->CanLink(pin) && !(factory->IsConvertible(start_pin->Type(), pin->Type()) ||
                                                  factory->IsConvertible(pin->Type(), start_pin->Type())))
                {
                    continue;
                }

                auto end_pin = pin;
                if (start_pin->Kind == PortType::Input) std::swap(start_pin, end_pin);

                const auto& start_node = FindNode(start_pin->NodeID)->Node;
                const auto& end_node   = FindNode(end_pin->NodeID)->Node;
                const auto& conn =
                    _graph->ConnectNodes(start_node->ID(), start_pin->Name(), end_node->ID(), end_pin->Name());

                _links.emplace(std::hash<flow::UUID>{}(conn->ID()),
                               ConnectionView{conn->ID(), start_pin->ID, end_pin->ID, start_pin->GetColour()});
                break;
            }
        }
    });
}

GraphWindow::~GraphWindow()
{
    _graph->Visit([](const auto& node) { return node->Stop(); });
    _graph->Clear();

    _links.clear();

    ed::DestroyEditor(std::bit_cast<ed::EditorContext*>(_editor_ctx.get()));
}

void GraphWindow::SetCurrentGraph()
{
    if (std::bit_cast<ed::EditorContext*>(_editor_ctx.get()) != ed::GetCurrentEditor())
    {
        ed::SetCurrentEditor(std::bit_cast<ed::EditorContext*>(_editor_ctx.get()));
    }
}

void GraphWindow::Draw()
try
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
    if (_dirty) window_flags |= ImGuiWindowFlags_UnsavedDocument;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    _active = ImGui::Begin(_graph->GetName().c_str(), &_open, window_flags);

    if (!_active)
    {
        return EndDraw();
    }

    SetCurrentGraph();
    ed::Begin(_graph->GetName().c_str());

    auto cursorTopLeft = ImGui::GetCursorScreenPos();

    CreateItems();

    if (ImGui::BeginDragDropTarget())
    {
        if (auto payload = ImGui::AcceptDragDropPayload("NewNode"))
        {
            std::string class_name   = reinterpret_cast<const char*>(payload->Data);
            std::string display_name = GetEnv()->GetFactory()->GetFriendlyName(class_name);

            _graph->OnNodeAdded.Bind(
                "SetPos", [](auto&& n) { ed::SetNodePosition(std::hash<UUID>{}(n->ID()), ImGui::GetMousePos()); });
            CreateNode(class_name, display_name);
            _graph->OnNodeAdded.Unbind("SetPos");
        }

        ImGui::EndDragDropTarget();
    }

    CleanupDeadItems();

    for (auto& [_, item] : _item_views)
    {
        item->ShowConnectables(_new_link_pin);
        item->Draw();
    }

    for (auto& [_, link] : _links)
    {
        link.Draw();
    }

    ImGui::SetCursorScreenPos(cursorTopLeft);
    ed::Suspend();
    if (ed::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
        _get_popup_location = true;
        _new_node_link_pin  = nullptr;
    }
    ed::Resume();

    if (_get_popup_location)
    {
        auto pos             = ImGui::GetMousePos();
        _open_popup_position = {pos.x, pos.y};
        _get_popup_location  = false;
    }

    ed::Suspend();
    node_context_menu();

    ed::Resume();

    if (ImGui::IsWindowFocused() && ed::AreShortcutsEnabled())
    {
        if (AcceptUndo())
        {
            UndoChange();
        }
        else if (AcceptRedo())
        {
            RedoChange();
        }
        else if (AcceptComment())
        {
            CreateComment();
        }

        if (ed::BeginShortcut())
        {
            if (ed::AcceptCopy())
            {
                json copied_flow = CopySelection();
                if (!copied_flow.empty()) ImGui::SetClipboardText(copied_flow.dump().c_str());
            }
            else if (ed::AcceptCut())
            {
                // FIXME: This copies, but doesn't delete.
                json copied_flow = CopySelection();
                if (!copied_flow.empty()) ImGui::SetClipboardText(copied_flow.dump().c_str());
            }
            else if (ed::AcceptDuplicate())
            {
                json copied_flow = CopySelection();
                if (!copied_flow.empty()) CreateNodesAction(copied_flow);
            }
            else if (ed::AcceptPaste())
            {
                std::string clipboard = ImGui::GetClipboardText();
                if (!clipboard.empty())
                {
                    json copied_flow = json::parse(clipboard);
                    CreateNodesAction(copied_flow);
                }
            }

            ed::EndShortcut();
        }
    }

    EndDraw();
}
catch (const std::exception& e)
{
    SPDLOG_ERROR("Caught exception while drawing graph: {0}", e.what());
    EndDraw();
}
catch (...)
{
    SPDLOG_ERROR("Caught unknown exception while drawing graph");
    EndDraw();
}

void GraphWindow::EndDraw()
{
    if (_active)
    {
        ed::End();
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

std::shared_ptr<NodeView> GraphWindow::FindNode(std::uint64_t id) const
{
    if (!id) throw std::invalid_argument("Node ID cannot be null");

    if (_item_views.contains(id))
    {
        return std::dynamic_pointer_cast<NodeView>(_item_views.at(id));
    }

    return nullptr;
}

ConnectionView& GraphWindow::FindConnection(std::uint64_t id)
{
    if (!id) throw std::invalid_argument("Link ID cannot be null");

    return _links.at(id);
}

std::shared_ptr<PortView> GraphWindow::FindPort(std::uint64_t id) const
{
    if (!id) throw std::invalid_argument("Pin ID cannot be null");

    for (auto& [_, item] : _item_views)
    {
        auto node = std::dynamic_pointer_cast<NodeView>(item);
        if (!node) continue;

        for (auto& pin : node->Inputs)
            if (pin->ID == id)
            {
                return pin;
            }

        for (auto& pin : node->Outputs)
            if (pin->ID == id)
            {
                return pin;
            }
    }

    return nullptr;
}

std::shared_ptr<CommentView> GraphWindow::FindComment(std::uint64_t id) const
{
    if (!id) throw std::invalid_argument("Comment ID cannot be null");

    if (_item_views.contains(id))
    {
        return std::dynamic_pointer_cast<CommentView>(_item_views.at(id));
    }

    return nullptr;
}

void GraphWindow::DeleteNode(std::uint64_t id)
{
    if (!_item_views.contains(id)) return;

    const auto& node = std::dynamic_pointer_cast<NodeView>(_item_views.at(id));
    if (node)
    {
        std::vector<std::uint64_t> links_to_delete;
        for (const auto& [link_id, link] : _links)
        {
            if (std::any_of(node->Inputs.begin(), node->Inputs.end(),
                            [&, end_pin_id = link.EndPortID](const auto& in) { return in->ID == end_pin_id; }))
            {
                links_to_delete.push_back(link_id);
            }

            if (std::any_of(node->Outputs.begin(), node->Outputs.end(),
                            [&, start_pin_id = link.StartPortID](const auto& out) { return out->ID == start_pin_id; }))
            {
                links_to_delete.push_back(link_id);
            }
        }

        for (const auto& link_id : links_to_delete)
        {
            DeleteLink(link_id);
        }

        _graph->RemoveNode(node->Node);
    }

    _item_views.erase(id);
}

bool GraphWindow::DeleteLink(std::uint64_t id)
{
    if (!_links.contains(id))
    {
        return false;
    }

    const auto& link = _links.at(id);

    auto start_pin = FindPort(link.StartPortID);
    auto end_pin   = FindPort(link.EndPortID);

    auto start_node = FindNode(start_pin->NodeID);
    auto end_node   = FindNode(end_pin->NodeID);

    _graph->DisconnectNodes(start_node->Node->ID(), start_pin->Key(), end_node->Node->ID(), end_pin->Key());

    return _links.erase(id) != 0;
}

void GraphWindow::ShowLinkFlowing(const flow::UUID& node_id, const IndexableName& key)
{
    auto&& conns = _graph->GetConnections().FindConnections(node_id, key);
    for (const auto& conn : conns)
    {
        _links.at(std::hash<flow::UUID>{}(conn->ID())).SetFlowing(true);
    }
}

void GraphWindow::CreateItems()
{
    if (!ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
    {
        _new_link_pin = nullptr;
        return;
    }

    std::uint64_t start_pin_id = 0, end_pin_id = 0;
    if (ed::QueryNewLink(std::bit_cast<ed::PinId*>(&start_pin_id), std::bit_cast<ed::PinId*>(&end_pin_id)))
    {
        auto start_pin = FindPort(start_pin_id);
        auto end_pin   = FindPort(end_pin_id);

        if (start_pin && end_pin)
        {
            _new_link_pin = start_pin;

            if (start_pin->Kind == PortType::Input)
            {
                std::swap(start_pin, end_pin);
                std::swap(start_pin_id, end_pin_id);
            }

            if (&end_pin == &start_pin || &start_pin->NodeID == &end_pin->NodeID || end_pin->IsConnected())
            {
                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            }
            else if (end_pin->Kind == start_pin->Kind)
            {
                DrawLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            }
            else if (!GetEnv()->GetFactory()->IsConvertible(start_pin->Type(), end_pin->Type()))
            {
                DrawLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
            }
            else
            {
                std::string label = "+ Create Link";
                if (start_pin->Type() != end_pin->Type() &&
                    GetEnv()->GetFactory()->IsConvertible(start_pin->Type(), end_pin->Type()))
                {
                    label = "+ Create Converting Link";
                }

                DrawLabel(label.c_str(), ImColor(32, 45, 32, 180));
                if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                {
                    const auto& start_node = FindNode(start_pin->NodeID)->Node;
                    const auto& end_node   = FindNode(end_pin->NodeID)->Node;
                    const auto& conn =
                        _graph->ConnectNodes(start_node->ID(), start_pin->Name(), end_node->ID(), end_pin->Name());

                    _links.emplace(std::hash<flow::UUID>{}(conn->ID()),
                                   ConnectionView{conn->ID(), start_pin->ID, end_pin->ID, start_pin->GetColour()});
                }
            }
        }
    }

    std::uint64_t pinId = 0;
    if (ed::QueryNewNode(std::bit_cast<ed::PinId*>(&pinId)))
    {
        _new_link_pin = FindPort(pinId);
        if (_new_link_pin)
        {
            DrawLabel("+ Create Node", ImColor(32, 45, 32, 180));
        }

        if (ed::AcceptNewItem())
        {
            _new_node_link_pin = FindPort(pinId);
            _new_link_pin      = nullptr;
            ed::Suspend();
            ImGui::OpenPopup("Create New Node");
            _get_popup_location = true;
            ed::Resume();
        }
    }

    ed::EndCreate();
}

void GraphWindow::CleanupDeadItems()
{
    if (!ed::BeginDelete()) return ed::EndDelete();

    std::uint64_t link_id = 0;
    while (ed::QueryDeletedLink(std::bit_cast<ed::LinkId*>(&link_id)))
    {
        if (!ed::AcceptDeletedItem()) continue;

        DeleteLink(link_id);
    }

    std::uint64_t node_id = 0;
    while (ed::QueryDeletedNode(std::bit_cast<ed::NodeId*>(&node_id)))
    {
        if (!ed::AcceptDeletedItem()) continue;

        DeleteNode(node_id);
    }

    ed::EndDelete();
}

void GraphWindow::OnLoadNode(const flow::SharedNode& node, const json& position_json)
{
    auto node_view = FindNode(std::hash<flow::UUID>{}(node->ID()));
    if (!node_view)
    {
        auto view_factory = std::dynamic_pointer_cast<ViewFactory>(GetEnv()->GetFactory());
        node_view         = view_factory->CreateNodeView(node);
        node_view->Node->OnSetOutput.Bind("ShowLinkFlowing", [=, this, node_id = node->ID()](const IndexableName& key) {
            ShowLinkFlowing(node_id, key);
        });

        _item_views.emplace(node_view->ID(), node_view);
    }

    const ImVec2 location(position_json["x"], position_json["y"]);
    ed::SetNodePosition(node_view->ID(), location);
}

void GraphWindow::OnLoadConnection(const flow::SharedConnection& connection)
{
    const auto start_node_id = std::hash<flow::UUID>{}(connection->StartNodeID());
    const auto end_node_id   = std::hash<flow::UUID>{}(connection->EndNodeID());

    auto start_node = FindNode(start_node_id);
    auto end_node   = FindNode(end_node_id);

    auto start_pin = std::find_if(start_node->Outputs.begin(), start_node->Outputs.end(),
                                  [&](auto&& pin) { return pin->Name() == connection->StartPortKey(); });
    auto end_pin   = std::find_if(end_node->Inputs.begin(), end_node->Inputs.end(),
                                  [&](auto&& pin) { return pin->Name() == connection->EndPortKey(); });

    _links.emplace(std::hash<flow::UUID>{}(connection->ID()),
                   ConnectionView{connection->ID(), (*start_pin)->ID, (*end_pin)->ID, (*start_pin)->GetColour()});
}

flow::SharedNode GraphWindow::CreateNode(const std::string& class_name, const std::string& display_name)
{
    auto new_node = GetEnv()->GetFactory()->CreateNode(class_name, flow::UUID{}, display_name, GetEnv());
    if (!new_node)
    {
        throw std::runtime_error("Failed to create node: " + display_name);
    }

    new_node->OnSetOutput.Bind("ShowLinkFlowing", [=, this, node_id = new_node->ID()](const IndexableName& key) {
        ShowLinkFlowing(node_id, key);
    });

    _graph->AddNode(new_node);
    GetEnv()->AddTask([=] { new_node->Start(); });

    return new_node;
}

json GraphWindow::SaveFlow()
{
    json graph_json = *_graph;

    std::vector<json>& nodes_json = graph_json["nodes"].get_ref<std::vector<json>&>();
    for (json& node_json : nodes_json)
    {
        flow::UUID id = std::string{node_json["id"]};
        ImVec2 pos    = ed::GetNodePosition(std::hash<flow::UUID>{}(id));

        node_json["position"] = {
            {"x", pos.x},
            {"y", pos.y},
        };
    }

    std::vector<json> comments_json;
    for (const auto& [ID, item] : _item_views)
    {
        if (auto comment = std::dynamic_pointer_cast<CommentView>(item))
        {
            comments_json.emplace_back(json{
                {"position", ed::GetNodePosition(ID)},
                {"size", ImVec2{comment->Size.Width, comment->Size.Height}},
                {"comment", comment->Name},
            });
        }
    }

    MarkDirty(false);

    // TODO(trigaux): Don't breakout the graph json, but instead save editor data to another file.
    return {
        {"nodes", graph_json["nodes"]},
        {"connections", graph_json["connections"]},
        {"comments", comments_json},
    };
}

void GraphWindow::LoadFlow(const json& j)
{
    if (_dirty) MarkDirty(false);

    j.get_to(*_graph);
    _graph->Visit([](const auto& node) { node->Start(); });

    std::erase_if(_item_views,
                  [](const auto& item) { return std::dynamic_pointer_cast<NodeView>(item.second) == nullptr; });

    const std::vector<json>& nodes_json = j["nodes"].get_ref<const std::vector<json>&>();
    for (const auto& node_json : nodes_json)
    {
        if (auto node = GetGraph()->GetNode(flow::UUID{node_json["id"]}))
        {
            OnLoadNode(node, node_json["position"]);
        };
    }

    for (const auto& [_, conn] : GetGraph()->GetConnections())
    {
        OnLoadConnection(conn);
    }

    if (!j.contains("comments")) return;

    const std::vector<json>& comments_json = j["comments"].get_ref<const std::vector<json>&>();
    for (const auto& comment_json : comments_json)
    {
        ImVec2 size(comment_json["size"]);
        auto comment   = std::make_shared<CommentView>(CommentView::CommentSize{size.x, size.y},
                                                     comment_json["comment"].get_ref<const std::string&>());
        auto [view, _] = _item_views.emplace(comment->ID(), std::move(comment));
        ed::SetNodePosition(view->second->ID(), comment_json["position"]);
    }
}

json GraphWindow::CopySelection()
{
    ed::NodeId ids[128];
    int result = ed::GetSelectedNodes(ids, 128);

    std::map<ed::NodeId, flow::UUID> node_ids;
    std::for_each_n(ids, result, [&](const auto& id) { node_ids.emplace(id, flow::UUID{}); });

    std::vector<json> nodes_json;
    std::vector<json> connections_json;

    auto&& connections = _graph->GetConnections();
    _graph->Visit([&](auto node) {
        const std::uint64_t id = std::hash<flow::UUID>{}(node->ID());

        if (!node_ids.contains(id))
        {
            return;
        }

        auto&& conns = connections.FindConnections(node->ID());
        for (const auto& connection : conns)
        {
            if (!node_ids.contains(std::hash<flow::UUID>{}(connection->EndNodeID())))
            {
                continue;
            }

            json conn_json = connection->Save();
            connections_json.push_back(std::move(conn_json));
        }

        json node_json        = node->Save();
        node_json["id"]       = std::string(node->ID());
        node_json["position"] = ed::GetNodePosition(id);

        nodes_json.push_back(std::move(node_json));
    });

    return {
        {"nodes", nodes_json},
        {"connections", connections_json},
    };
}

void GraphWindow::CreateNodesAction(const json& SPDLOG_json)
{
    json new_diff = SPDLOG_json;
    if (!new_diff.contains("nodes")) return;

    std::map<std::string, flow::UUID> remap_node_ids;
    auto&& nodes = new_diff["nodes"].get_ref<std::vector<json>&>();
    for (auto& node : nodes)
    {
        flow::UUID new_id{};
        remap_node_ids.emplace(node["id"], new_id);
        node["id"] = std::string(new_id);
    }

    auto&& conns = new_diff["connections"].get_ref<std::vector<json>&>();
    for (auto& conn : conns)
    {
        std::string in_id  = conn["in_id"];
        std::string out_id = conn["out_id"];
        conn["in_id"]      = remap_node_ids[in_id];
        conn["out_id"]     = remap_node_ids[out_id];
    }

    const ImVec2 first_pos = nodes.front()["position"];
    const auto load_node   = [&](const flow::SharedNode& node, const json& position_json) {
        const auto factory = std::dynamic_pointer_cast<ViewFactory>(GetEnv()->GetFactory());
        auto node_view     = factory->CreateNodeView(node);

        node->OnSetOutput.Bind("ShowLinkFlowing",
                                 [=, this, id = node->ID()](const auto& key) { ShowLinkFlowing(id, key); });

        const ImVec2 pos     = position_json;
        const ImVec2 new_pos = ImGui::GetMousePos() + (pos - first_pos);
        ed::SetNodePosition(node_view->ID(), new_pos);

        _item_views.emplace(std::hash<flow::UUID>{}(node->ID()), std::move(node_view));

        node->Start();
    };

    const auto load_conns = [&](const flow::SharedConnection& connection) {
        const auto start_node_id = std::hash<flow::UUID>{}(connection->StartNodeID());
        const auto end_node_id   = std::hash<flow::UUID>{}(connection->EndNodeID());

        auto start_node = FindNode(start_node_id);
        auto end_node   = FindNode(end_node_id);

        auto start_pin = std::find_if(start_node->Outputs.begin(), start_node->Outputs.end(),
                                      [&](auto&& pin) { return pin->Name() == connection->StartPortKey(); });
        auto end_pin   = std::find_if(end_node->Inputs.begin(), end_node->Inputs.end(),
                                      [&](auto&& pin) { return pin->Name() == connection->EndPortKey(); });

        _links.emplace(std::hash<flow::UUID>{}(connection->ID()),
                       ConnectionView{connection->ID(), (*start_pin)->ID, (*end_pin)->ID, (*start_pin)->GetColour()});
    };

    new_diff.get_to(*_graph);

    for (const auto& node_json : nodes)
    {
        if (auto node = GetGraph()->GetNode(flow::UUID{node_json["id"]})) load_node(node, node_json["position"]);
    }

    for (const auto& [_, conn] : GetGraph()->GetConnections())
    {
        OnLoadConnection(conn);
    }

    _undo_history.push(Action{ActionType::Create, new_diff});
}

void GraphWindow::DeleteNodesAction(Action& action)
{
    auto&& nodes = action.Info["nodes"].get_ref<std::vector<json>&>();
    for (auto& node : nodes)
    {
        std::uint64_t id = std::hash<flow::UUID>{}(flow::UUID{node["id"]});
        ImVec2 pos       = ed::GetNodePosition(id);
        node["position"] = pos;

        ed::BreakLinks(static_cast<ed::NodeId>(id));
        ed::DeleteNode(id);
    }

    _undo_history.push(Action{ActionType::Delete, action.Info});
}

void GraphWindow::UndoChange()
{
    if (_undo_history.empty()) return;

    Action last_action = _undo_history.top();
    if ((last_action.Type & ActionType::Create) == ActionType::Create)
    {
        DeleteNodesAction(last_action);
    }
    else if ((last_action.Type & ActionType::Delete) == ActionType::Delete)
    {
        CreateNodesAction(last_action.Info);
    }

    _redo_history.push(last_action);
    _undo_history.pop();
}

void GraphWindow::RedoChange()
{
    if (_redo_history.empty()) return;

    Action last_action = _redo_history.top();
    if ((last_action.Type & ActionType::Create) == ActionType::Create)
    {
        CreateNodesAction(last_action.Info);
    }
    else if ((last_action.Type & ActionType::Delete) == ActionType::Delete)
    {
        DeleteNodesAction(last_action);
    }

    _undo_history.push(last_action);
    _redo_history.pop();
}

void GraphWindow::CreateComment()
{
    const auto [min_pos, size] = GetContainerNodeBounds();
    if (min_pos == size || size == ImVec2(0.f, 0.f)) return;

    auto comment   = std::make_shared<CommentView>(CommentView::CommentSize{size.x, size.y});
    auto [view, _] = _item_views.emplace(comment->ID(), std::move(comment));
    ed::SetNodePosition(view->second->ID(), min_pos);
}

FLOW_UI_NAMESPACE_END
