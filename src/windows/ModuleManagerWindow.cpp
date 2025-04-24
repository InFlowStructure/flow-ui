#include "ModuleManagerWindow.hpp"

#include "FileExplorer.hpp"
#include "InputField.hpp"
#include "ModuleInfo.hpp"
#include "Text.hpp"
#include "Widget.hpp"
#include "utilities/Conversions.hpp"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

FLOW_UI_NAMESPACE_START

const std::string module_file_extension = "flowmod";
#ifdef FLOW_WINDOWS
const std::string module_binary_extension = ".dll";
#elif FLOW_APPLE
const std::string module_binary_extension = ".dylib";
#else
const std::string module_binary_extension = ".so";
#endif

class ModuleView : public Widget
{
  public:
    ModuleView(const std::filesystem::path& name, std::shared_ptr<Env> env, const ModuleInfo& info)
        : _binary_path(name), _env(std::move(env)), _info(info),
          _enabled(name.filename().replace_extension("").string(), true)
    {
        if (_enabled.GetValue())
        {
            _env->LoadModule(_binary_path);
        }
        else
        {
            _env->UnloadModule(_binary_path);
        }
    }

    ModuleView(const std::filesystem::path& dir, std::shared_ptr<Env> env)
        : _env(std::move(env)), _enabled(dir.filename().replace_extension("").string(), true)
    {
        std::filesystem::path module_path;
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
        {
            if (!std::filesystem::is_regular_file(entry))
            {
                continue;
            }

            if (entry.path().extension() == "." + module_file_extension)
            {
                module_path = entry;
            }

            if (entry.path().extension() == module_binary_extension)
            {
                _binary_path = entry;
            }
        }

        std::ifstream module_fs(module_path);
        json module_j = json::parse(module_fs);
        _info         = module_j;

        if (_enabled.GetValue())
        {
            _env->LoadModule(_binary_path);
        }
        else
        {
            _env->UnloadModule(_binary_path);
        }
    }

    virtual void operator()() noexcept
    {
        const std::string& name    = _binary_path.filename().replace_extension("").string();
        const std::string& version = _info.Version;
        const std::string& author  = _info.Author;

        ImGui::TableNextColumn();

        _enabled();

        ImGui::TableNextColumn();

        ImGui::BeginHorizontal(("module_" + name).c_str());
        widgets::Text{name}.SetFontSize(20.f)();

        auto posX =
            (ImGui::GetCursorPosX() + ImGui::GetColumnWidth() -
             ImGui::CalcTextSize(author.length() > ("Version: " + version).length() ? author.c_str()
                                                                                    : ("Version: " + version).c_str())
                 .x +
             -ImGui::GetScrollX() - 10 * ImGui::GetStyle().ItemSpacing.x);
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
                _env->LoadModule(_binary_path);
            }
            else
            {
                _env->UnloadModule(_binary_path);
            }
        }
    }

  private:
    std::filesystem::path _binary_path;
    std::shared_ptr<Env> _env;
    ModuleInfo _info;
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
            FileExplorer::Load(FileExplorer::GetDocumentsPath(), "Flow Modules (flowmod)", module_file_extension);

        try
        {
            std::ifstream module_fs(filename);
            ModuleInfo module_info = json::parse(module_fs);

            const std::string module_file_name = module_info.Name + module_binary_extension;
            std::filesystem::path module_binary_path;
            for (const auto& entry : std::filesystem::recursive_directory_iterator(filename.parent_path()))
            {
                if (!std::filesystem::is_regular_file(entry) || entry.path().filename() != module_file_name)
                {
                    continue;
                }

                module_binary_path = entry;
                break;
            }

            std::filesystem::path module_path = _modules_path / module_info.Name;
            std::filesystem::create_directory(module_path);

            std::filesystem::copy_file(filename, module_path / filename.filename(),
                                       std::filesystem::copy_options::overwrite_existing);
            std::filesystem::copy_file(module_binary_path, module_path / module_binary_path.filename(),
                                       std::filesystem::copy_options::overwrite_existing);
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
        if (_new_module_window->DrawAndCreate())
        {
            _new_module_window.reset(new NewModuleWindow());
        }

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
        if (!std::filesystem::is_directory(module))
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
