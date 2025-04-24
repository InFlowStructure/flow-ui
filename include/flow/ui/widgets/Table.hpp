#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Widget.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

FLOW_UI_SUBNAMESPACE_START(widgets)

/**
 * @brief Table widget for displaying widgets in an organised table view.
 */
class Table : public Widget
{
  public:
    /**
     * @brief Constructs a table with a name and number of columns.
     * @param name The name of the table.
     * @param columns The number of columns.
     */
    Table(const std::string& name, std::size_t columns, std::size_t outer_width = 0, std::size_t outer_height = 0);

    virtual ~Table() = default;

    /**
     * @brief Renders the table to the window.
     */
    virtual void operator()() noexcept override;

    /**
     * @brief Adds widget entry to the table in the next available column.
     * @param widget THe widget to add to the next column.
     */
    void AddEntry(std::shared_ptr<Widget> widget);

    const std::shared_ptr<Widget>& GetEntry(std::size_t i) const { return _widgets.at(i); }

  private:
    std::string _name;
    std::size_t _columns      = 0;
    std::size_t _outer_width  = 0;
    std::size_t _outer_height = 0;
    std::vector<std::shared_ptr<Widget>> _widgets;
};

FLOW_UI_SUBNAMESPACE_END
