#include "Text.hpp"
#include "utilities/Conversions.hpp"

FLOW_UI_SUBNAMESPACE_START(widgets)

Text::Text(const std::string& text, const Alignment& align, const Colour& colour)
    : _text(text), _colour(colour), _align(align)
{
}

void Text::operator()() noexcept
{
    const auto window_size = ImGui::GetWindowSize();
    const auto text_size   = ImGui::CalcTextSize(_text.c_str());

    ImVec2 pos = ImGui::GetCursorPos();
    switch (_align.Horizontal)
    {
    case HorizontalAlignment::Left:
        break;
    case HorizontalAlignment::Centre:
        pos.x = (window_size.x - text_size.x) * 0.5f;
        break;
    case HorizontalAlignment::Right:
        pos.x = window_size.x - text_size.x;
        break;
    }

    switch (_align.Vertical)
    {
    case VerticalAlignment::Top:
        break;
    case VerticalAlignment::Centre:
        pos.y = (window_size.y - text_size.y) * 0.5f;
        break;
    case VerticalAlignment::Bottom:
        pos.y = window_size.y - (text_size.y * 2.f);
        break;
    }
    ImGui::SetCursorPos(pos);

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(_colour.R, _colour.G, _colour.B, _colour.A));

    float old_font_size     = ImGui::GetFont()->Scale;
    ImGui::GetFont()->Scale = _font_size / ImGui::GetFont()->FontSize;
    ImGui::PushFont(ImGui::GetFont());

    ImGui::TextUnformatted(_text.c_str());

    ImGui::GetFont()->Scale = old_font_size;
    ImGui::PopFont();

    ImGui::PopStyleColor();
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

Text& Text::SetFontSize(float new_size) noexcept
{
    _font_size = new_size;
    return *this;
}

FLOW_SUBNAMESPACE_END
