#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Widget.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

FLOW_UI_SUBNAMESPACE_START(widgets)

class Table : public Widget
{
  public:
    Table(const std::string& name, std::size_t columns);
    virtual ~Table() = default;

    virtual void operator()() noexcept override;

    void AddEntry(std::shared_ptr<Widget> widget);

  private:
    std::string _name;
    std::size_t _columns = 0;
    std::vector<std::shared_ptr<Widget>> _widgets;
};

FLOW_UI_SUBNAMESPACE_END
