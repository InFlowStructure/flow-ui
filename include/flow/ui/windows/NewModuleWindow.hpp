#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Window.hpp"
#include "widgets/InputField.hpp"
#include "widgets/Table.hpp"

FLOW_UI_NAMESPACE_START

class NewModuleWindow : public Window
{
  public:
    NewModuleWindow();
    virtual ~NewModuleWindow() = default;

    bool DrawAndCreate();

  public:
    static inline const std::string Name = "Create New Module";

  private:
    void Clear();

  private:
    std::shared_ptr<widgets::Input<std::string>> name_input;
    std::shared_ptr<widgets::Input<std::string>> version_input;
    std::shared_ptr<widgets::Input<std::string>> author_input;
    std::shared_ptr<widgets::Input<std::string>> description_input;
    std::shared_ptr<widgets::Table> dependencies;
};

FLOW_UI_NAMESPACE_END
