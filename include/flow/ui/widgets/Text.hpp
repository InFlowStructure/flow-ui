#pragma once

#include "flow/ui/Config.hpp"
#include "flow/ui/Core.hpp"
#include "flow/ui/Style.hpp"
#include "flow/ui/Widget.hpp"

FLOW_UI_SUBNAMESPACE_BEGIN(widgets)

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

    using HAlignment = HorizontalAlignment;

    /**
     * @brief Vertical alignment type.
     */
    enum class VerticalAlignment
    {
        Top,
        Middle,
        Bottom
    };

    using VAlignment = VerticalAlignment;

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
     */
    Text(const std::string& text);

    virtual ~Text() = default;

    /**
     * @brief Renders the text to the window.
     */
    virtual void Draw() noexcept override;

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

    Text& SetAlignment(HorizontalAlignment new_halign, VerticalAlignment new_valign) noexcept;

    Text& SetHorizontalAlignment(HorizontalAlignment new_halign) noexcept;

    Text& SetVerticalAlignment(VerticalAlignment new_valign) noexcept;

    Text& SetFont(const std::unique_ptr<Font>& font);

    /**
     * @brief Sets the font size of the text.
     * @param new_size The new size of the text.
     * @returns A reference to the text widget.
     */
    Text& SetFontSize(float new_size) noexcept;

  private:
    std::string _text;
    Colour _colour;
    Alignment _align = {HorizontalAlignment::Left, VerticalAlignment::Top};
    void* _font;
    float _font_size = 18.f;
};

FLOW_UI_SUBNAMESPACE_END