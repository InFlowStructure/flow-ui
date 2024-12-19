#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"
#include "flow/ui/Widget.hpp"

#include <map>
#include <memory>
#include <vector>

FLOW_UI_SUBNAMESPACE_START(widgets)

/**
 * @brief Property tree widget for showing user defined properties in a collapsable tree table.
 */
class PropertyTree : public Widget
{
  public:
    /**
     * @brief Construct a property tree with a name and number of columns.
     * @param name The name of the tree.
     * @param columns The number of columns for the property table.
     */
    PropertyTree(const std::string& name, std::size_t columns = 1);

    virtual ~PropertyTree() = default;

    /**
     * @brief Add a property and a widget to represent it.
     * @param name THe name of the property.
     * @param widget The widget to represent property.
     * @param category_name The category to list the property under.
     */
    void AddProperty(const std::string& name, std::shared_ptr<Widget> widget, const std::string& category_name);

    /**
     * @brief Add a property and a list of widgets to represent it.
     * @param name The name of the property.
     * @param widgets The list of widgets to represent the property.
     * @param category_name The category to list the property under.
     */
    void AddProperty(const std::string& name, const std::vector<std::shared_ptr<Widget>>& widgets,
                     const std::string& category_name);

    /**
     * @brief Renders the property tree widget to the windows.
     */
    virtual void operator()() noexcept override;

  private:
    std::map<std::string, std::map<std::string, std::vector<std::shared_ptr<Widget>>>> _properties;
    std::string _name;
    std::size_t _columns = 1;
};

FLOW_UI_SUBNAMESPACE_END