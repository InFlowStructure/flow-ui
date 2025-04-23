#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Window.hpp"

FLOW_UI_NAMESPACE_START

class NewModuleWindow : public Window
{
  public:
    NewModuleWindow();
    virtual ~NewModuleWindow() = default;

    virtual void Draw() override;

  public:
    static inline const std::string Name = "Create New Module";

  private:
    std::string _name;
    std::string _version;
    std::string _author;
    std::string _description;
};

FLOW_UI_NAMESPACE_END
