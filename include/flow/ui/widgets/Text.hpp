#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"
#include "flow/ui/Widget.hpp"

FLOW_UI_SUBNAMESPACE_START(widgets)

class Text : public Widget
{
  public:
    enum class HorizontalAlignment
    {
        Left,
        Centre,
        Right
    };

    enum class VerticalAlignment
    {
        Top,
        Centre,
        Bottom
    };

    struct Alignment
    {
        HorizontalAlignment Horizontal;
        VerticalAlignment Vertical;
    };

  public:
    Text(const std::string& text, const Alignment& align = {HorizontalAlignment::Left, VerticalAlignment::Top},
         const Colour& colour = Colour());

    virtual ~Text() = default;

    virtual void operator()() noexcept override;

    Text& SetColour(const Colour& new_colour) noexcept;

    Text& SetAlignment(const Alignment& new_align) noexcept;

    Text& SetFontSize(float new_size) noexcept;

  private:
    std::string _text;
    float _font_size = 18.f;
    Colour _colour;
    Alignment _align;
};

FLOW_UI_SUBNAMESPACE_END