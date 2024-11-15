// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "LogWindow.hpp"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

FLOW_UI_NAMESPACE_START
std::string LogWindow::Name = "Logs";

LogWindow::LogWindow() : Window(LogWindow::Name, DefaultDockspaces::Misc) {}

void LogWindow::Draw()
try
{
    ImGui::BeginTable("Node", 3, ImGuiTableFlags_SizingFixedFit);

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(5.f, 5.f));
    for (const auto& [msg, text] : _messages)
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        std::time_t time = std::chrono::system_clock::to_time_t(msg.time);
        std::ostringstream time_ss;
        time_ss << std::put_time(std::localtime(&time), "%H:%M:%S");

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
        ImGui::TextUnformatted(time_ss.str().c_str());
        ImGui::PopStyleColor();

        ImGui::TableNextColumn();

        ImGui::Text("[");
        ImGui::SameLine();

        switch (msg.level)
        {
        case spdlog::level::err:
            [[fallthrough]];
        case spdlog::level::critical:
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(194, 1, 20, 255));
            break;
        case spdlog::level::warn:
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(234, 196, 53, 255));
            break;
        case spdlog::level::debug:
            [[fallthrough]];
        case spdlog::level::trace:
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 188, 235, 255));
            break;
        default:
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(200, 200, 200, 255));
            break;
        }

        ImGui::TextUnformatted(spdlog::level::to_string_view(msg.level).data());
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::TextUnformatted("]");

        ImGui::TableNextColumn();

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        ImGui::TextUnformatted(text.c_str());
        ImGui::PopStyleColor();
    }

    ImGui::PopStyleVar();

    ImGui::EndTable();

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) ImGui::SetScrollHereY(1.0f);
}
catch (const std::exception& fail)
{
    SPDLOG_CRITICAL("Failed to load log file: {0}", fail.what());
}

FLOW_UI_NAMESPACE_END
