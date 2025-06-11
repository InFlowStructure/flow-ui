#include "ModuleManagerWindow.hpp"

#include "FileExplorer.hpp"
#include "InputField.hpp"
#include "Text.hpp"
#include "Widget.hpp"
#include "utilities/Conversions.hpp"

#include <flow/core/Env.hpp>
#include <flow/core/Module.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

FLOW_UI_NAMESPACE_START

const std::string module_file_extension = "flowmod";
#ifdef FLOW_WINDOWS
const std::string module_binary_extension = ".dll";
#elif defined(FLOW_APPLE)
const std::string module_binary_extension = ".dylib";
#else
const std::string module_binary_extension = ".so";
#endif

class ModuleView : public Widget
{
  public:
    ModuleView(const std::filesystem::path& name, std::shared_ptr<Env> env)
        : _binary_path(name), _enabled(name.filename().replace_extension("").string(), true)
    {
        _module = std::make_shared<Module>(_binary_path, env->GetFactory());
    }

    virtual void operator()() noexcept
    {
        const std::string& name    = _module->GetName();
        const std::string& version = "Version: " + _module->GetVersion();
        const std::string& author  = _module->GetAuthor();

        ImGui::TableNextColumn();

        _enabled();

        ImGui::TableNextColumn();

        ImGui::BeginHorizontal(("module_" + name).c_str());
        widgets::Text{name}.SetFontSize(20.f)();

        auto pos_x = (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -
                      ImGui::CalcTextSize(author.length() > version.length() ? author.c_str() : version.c_str()).x +
                      -ImGui::GetScrollX() - 10 * ImGui::GetStyle().ItemSpacing.x);

        if (pos_x > ImGui::GetCursorPosX())
        {
            ImGui::SetCursorPosX(pos_x);
        }

        ImGui::BeginVertical("version/author");
        constexpr Colour version_author_colour{150, 150, 150};
        widgets::Text{version}.SetFontSize(20.f).SetColour(version_author_colour)();
        widgets::Text{author}.SetFontSize(18.f).SetColour(version_author_colour)();
        ImGui::EndVertical();
        ImGui::EndHorizontal();

        if (auto data = _enabled.GetData())
        {
            if (_enabled.GetValue())
            {
                _module->Load(_binary_path);
            }
            else
            {
                _module->Unload();
            }
        }
    }

  private:
    std::filesystem::path _binary_path;
    std::shared_ptr<Module> _module;
    widgets::Input<bool> _enabled;
};

ModuleManagerWindow::ModuleManagerWindow(std::shared_ptr<Env> env, const std::filesystem::path& modules_path)
    : Window("Module Manager"), _env(std::move(env)), _modules_path(modules_path),
      _new_module_window(std::make_unique<NewModuleWindow>())
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
        std::filesystem::create_directory(_modules_path);

        const auto filename =
            FileExplorer::Load(FileExplorer::GetDocumentsPath(), "Flow Module (flowmod)", module_file_extension);

        {
            _widgets[filename.string()] = std::make_shared<ModuleView>(filename, _env);
        }
    }

    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Create Module", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize))
    {
        if (_new_module_window->DrawAndCreate())
        {
            _new_module_window.reset(new NewModuleWindow());
        }

        ImGui::EndPopup();
    }
    ImGui::EndHorizontal();

    if (_widgets.empty())
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

    for (const auto& [_, w] : _widgets)
    {
        (*w)();
    }

    ImGui::EndTable();
    ImGui::PopStyleVar(5);

    ImGui::EndChild();
}

FLOW_UI_NAMESPACE_END
