#include "ModuleManagerWindow.hpp"

#include "InputField.hpp"
#include "Text.hpp"
#include "Widget.hpp"

#include <imgui.h>

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
        const std::string& name = _name.filename().replace_extension("").string();

        _enabled();

        ImGui::TableNextColumn();
        widgets::Text{name}.SetFontSize(20.f)();

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
    if (!std::filesystem::exists(_modules_path) || std::filesystem::is_empty(_modules_path))
    {
        return Window::Draw();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10.f, 10.f));
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.f, 10.f));
    ImGui::BeginTable((_name + "##list").c_str(), 2,
                      ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter);

    for (const auto& module : std::filesystem::directory_iterator(_modules_path))
    {
        ImGui::TableNextColumn();

        const auto& module_name = module.path().string();
        if (!_widgets.contains(module_name))
        {
            _widgets[module_name] = std::make_shared<ModuleView>(module.path(), _env);
        }
        (*_widgets.at(module_name))();
    }

    ImGui::EndTable();
    ImGui::PopStyleVar(5);
}

FLOW_UI_NAMESPACE_END
