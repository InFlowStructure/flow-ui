#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Widget.hpp"
#include "flow/ui/Window.hpp"
#include "flow/ui/windows/NewModuleWindow.hpp"

#include <flow/core/Env.hpp>

#include <filesystem>
#include <map>
#include <memory>

FLOW_UI_NAMESPACE_START

/**
 * @brief Windows for displaying available modules and which of them are loaded/unloaded.
 */
class ModuleManagerWindow : public Window
{
  public:
    /**
     * @brief Constructs a module manager window with a shared environment and a specified modules path.
     * @param env The shared environment.
     * @param modules_path The path to the modules directory.
     */
    ModuleManagerWindow(std::shared_ptr<Env> env, const std::filesystem::path& modules_path);

    virtual ~ModuleManagerWindow() = default;

    /**
     * @brief Renders the window to the screen.
     */
    virtual void Draw() override;

  private:
    std::shared_ptr<Env> _env;
    std::filesystem::path _modules_path;
    std::map<std::string, std::shared_ptr<Widget>> _widgets;
    std::unique_ptr<NewModuleWindow> _new_module_window;
};

FLOW_UI_NAMESPACE_END
