#include "Text.hpp"
#include "utilities/Conversions.hpp"

FLOW_UI_SUBNAMESPACE_BEGIN(widgets)

Text::Text(const std::string& text)
    : _text(text), _font(reinterpret_cast<void*>(ImGui::GetFont())), _font_size(ImGui::GetFontSize())
{
}

void Text::Draw() noexcept
{
    const auto original_cursor_pos = ImGui::GetCursorPos();
    const auto region_size         = original_cursor_pos + ImGui::GetContentRegionAvail();
    const auto text_size           = ImGui::CalcTextSize(_text.c_str());

    ImVec2 pos = ImGui::GetCursorPos();
    switch (_align.Horizontal)
    {
    case HorizontalAlignment::Left:
        break;
    case HorizontalAlignment::Centre:
        pos.x = (region_size.x - text_size.x) * 0.5f;
        break;
    case HorizontalAlignment::Right:
        pos.x = region_size.x - text_size.x;
        break;
    }

    switch (_align.Vertical)
    {
    case VerticalAlignment::Top:
        break;
    case VerticalAlignment::Middle:
        pos.y = (region_size.y - text_size.y) * 0.5f;
        break;
    case VerticalAlignment::Bottom:
        pos.y = region_size.y - text_size.y;
        break;
    }

    ImGui::SetCursorPos(pos);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(_colour.R, _colour.G, _colour.B, _colour.A));

    auto* font = reinterpret_cast<ImFont*>(_font);

    float old_font_size = ImGui::GetFont()->Scale;
    font->Scale         = _font_size / font->FontSize;
    ImGui::PushFont(font);

    ImGui::TextUnformatted(_text.c_str());

    font->Scale = old_font_size;
    ImGui::PopFont();

    ImGui::PopStyleColor();

    ImGui::SetCursorPos(original_cursor_pos);
}

Text& Text::SetColour(const Colour& new_colour) noexcept
{
    _colour = new_colour;
    return *this;
}

Text& Text::SetAlignment(const Alignment& new_align) noexcept
{
    _align = new_align;
    return *this;
}

Text& Text::SetAlignment(HorizontalAlignment new_halign, VerticalAlignment new_valign) noexcept
{
    _align = {new_halign, new_valign};
    return *this;
}

Text& Text::SetHorizontalAlignment(HorizontalAlignment new_halign) noexcept
{
    _align.Horizontal = new_halign;
    return *this;
}

Text& Text::SetVerticalAlignment(VerticalAlignment new_valign) noexcept
{
    _align.Vertical = new_valign;
    return *this;
}

Text& Text::SetFont(const std::unique_ptr<Font>& font)
{
    if (font)
    {
        _font = reinterpret_cast<void*>(font.get());
    }

    return *this;
}

Text& Text::SetFontSize(float new_size) noexcept
{
    _font_size = new_size;
    return *this;
}

FLOW_SUBNAMESPACE_END
