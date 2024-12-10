#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"
#include "flow/ui/Widget.hpp"

#include <map>
#include <memory>
#include <vector>

FLOW_UI_SUBNAMESPACE_START(widgets)

class PropertyTree : public Widget
{
  public:
    PropertyTree(const std::string& name, std::size_t columns = 1);

    void AddProperty(const std::string& name, std::shared_ptr<Widget> widget, const std::string& category_name);
    void AddProperty(const std::string& name, const std::vector<std::shared_ptr<Widget>>& widgets,
                     const std::string& category_name);

    virtual void operator()() noexcept override;

  private:
    std::map<std::string, std::map<std::string, std::vector<std::shared_ptr<Widget>>>> _properties;
    std::string _name;
    std::size_t _columns = 1;
};

FLOW_UI_SUBNAMESPACE_END