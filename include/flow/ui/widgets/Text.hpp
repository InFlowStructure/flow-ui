#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"
#include "flow/ui/Widget.hpp"

FLOW_UI_SUBNAMESPACE_START(widgets)

/**
 * @brief Text widget to display strings in a window.
 */
class Text : public Widget
{
  public:
    /**
     * @brief Horizontal alignment type.
     */
    enum class HorizontalAlignment
    {
        Left,
        Centre,
        Right
    };

    /**
     * @brief Vertical alignment type.
     */
    enum class VerticalAlignment
    {
        Top,
        Centre,
        Bottom
    };

    /**
     * @brief Alignment rules for ttext widget.
     */
    struct Alignment
    {
        /// Horizontal alignment of the text.
        HorizontalAlignment Horizontal;

        /// Vertical alignment of the text.
        VerticalAlignment Vertical;
    };

  public:
    /**
     * @brief Constructs a text widget.
     * @param text The text to display.
     * @param colour The colour of the displayed text.
     * @param align The alignment of the text in the window.
     */
    Text(const std::string& text, const Colour& colour = Colour(),
         const Alignment& align = {HorizontalAlignment::Left, VerticalAlignment::Top});

    virtual ~Text() = default;

    /**
     * @brief Renders the text to the window.
     */
    virtual void operator()() noexcept override;

    /**
     * @brief Sets the text colour.
     * @param new_colour The new colour of the text.
     * @returns A reference to the text widget.
     */
    Text& SetColour(const Colour& new_colour) noexcept;

    /**
     * @brief Sets the alignment of the text in the window.
     * @param new_align The new alignment of the text.
     * @returns A reference to the text widget.
     */
    Text& SetAlignment(const Alignment& new_align) noexcept;

    /**
     * @brief Sets the font size of the text.
     * @param new_size The new size of the text.
     * @returns A reference to the text widget.
     */
    Text& SetFontSize(float new_size) noexcept;

  private:
    std::string _text;
    float _font_size = 18.f;
    Colour _colour;
    Alignment _align;
};

FLOW_UI_SUBNAMESPACE_END