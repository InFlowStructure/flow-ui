#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Widget.hpp"
#include "flow/ui/Window.hpp"

#include <flow/core/Env.hpp>

#include <filesystem>
#include <map>
#include <memory>

FLOW_UI_NAMESPACE_START

class ModuleManagerWindow : public Window
{
  public:
    ModuleManagerWindow(std::shared_ptr<Env> env, const std::filesystem::path& modules_path);
    virtual ~ModuleManagerWindow() = default;

    virtual void Draw() override;

  private:
    std::shared_ptr<Env> _env;
    std::filesystem::path _modules_path;
    std::map<std::string, std::shared_ptr<Widget>> _widgets;
};

FLOW_UI_NAMESPACE_END
