#include "ModuleManagerWindow.hpp"

#include "FileExplorer.hpp"
#include "InputField.hpp"
#include "NewModuleWindow.hpp"
#include "Text.hpp"
#include "Widget.hpp"
#include "utilities/Conversions.hpp"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

FLOW_UI_NAMESPACE_START

class ModuleView : public Widget
{
  public:
    ModuleView(const std::filesystem::path& name, std::shared_ptr<Env> env)
        : _name(name), _env(std::move(env)), _enabled(name.filename().replace_extension("").string(), true)
    {
        if (_enabled.GetValue())
        {
            _env->LoadModule(_name);
        }
        else
        {
            _env->UnloadModule(_name);
        }
    }

    virtual void operator()() noexcept
    {
        const std::string& name    = _name.filename().replace_extension("").string();
        const std::string& version = "0.0.0";
        const std::string& author  = "Cisco Systems, Inc.";

        ImGui::TableNextColumn();

        _enabled();

        ImGui::TableNextColumn();

        ImGui::BeginHorizontal(("module_" + name).c_str());
        widgets::Text{name}.SetFontSize(20.f)();

        auto posX = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(author.c_str()).x -
                     ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
        if (posX > ImGui::GetCursorPosX()) ImGui::SetCursorPosX(posX);

        ImGui::BeginVertical("version/author");
        constexpr Colour version_author_colour{150, 150, 150};
        widgets::Text{"Version: " + version}.SetFontSize(20.f).SetColour(version_author_colour)();
        widgets::Text{author}.SetFontSize(18.f).SetColour(version_author_colour)();
        ImGui::EndVertical();
        ImGui::EndHorizontal();

        if (auto data = _enabled.GetData())
        {
            if (_enabled.GetValue())
            {
                _env->LoadModule(_name);
            }
            else
            {
                _env->UnloadModule(_name);
            }
        }
    }

  private:
    std::filesystem::path _name;
    std::shared_ptr<Env> _env;
    widgets::Input<bool> _enabled;
};

ModuleManagerWindow::ModuleManagerWindow(std::shared_ptr<Env> env, const std::filesystem::path& modules_path)
    : Window("Module Manager"), _env(std::move(env)), _modules_path(modules_path)
{
    std::filesystem::create_directory(_modules_path);
}

void ModuleManagerWindow::Draw()
{
    ImGui::BeginHorizontal("ModuleButtons");
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));

    if (ImGui::Button("+ New Module"))
    {
        ImGui::OpenPopup("Create Module");
    }

    if (ImGui::Button("Import Module"))
    {
        const auto filename = FileExplorer::Load(FileExplorer::GetDocumentsPath(), "Flow Modules", "so,dll,dylib");

        try
        {
            _env->LoadModule(filename);
        }
        catch (const std::exception& e)
        {
            SPDLOG_ERROR("Caught exception while loading module {}: {}", filename.string(), e.what());
        }
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Create Module", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
    {
        NewModuleWindow().Draw();
        ImGui::EndPopup();
    }
    ImGui::EndHorizontal();

    if (!std::filesystem::exists(_modules_path) || std::filesystem::is_empty(_modules_path))
    {
        return Window::Draw();
    }

    ImGui::BeginChild("ModuleList");

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, 10.f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.f, 10.f));
    ImGui::BeginTable((_name + "##list").c_str(), 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter);

    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

    for (const auto& module : std::filesystem::directory_iterator(_modules_path))
    {
        if (std::filesystem::is_directory(module))
        {
            continue;
        }

        const auto& module_name = module.path().string();
        if (!_widgets.contains(module_name))
        {
            _widgets[module_name] = std::make_shared<ModuleView>(module.path(), _env);
        }
        (*_widgets.at(module_name))();
    }

    ImGui::EndTable();
    ImGui::PopStyleVar(5);

    ImGui::EndChild();
}

FLOW_UI_NAMESPACE_END
