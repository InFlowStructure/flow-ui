#include "InputField.hpp"
#include "utilities/Widgets.hpp"

#include <imgui.h>

#include <filesystem>

FLOW_UI_SUBNAMESPACE_START(widgets)

template<typename T>
Input<T>::Input(std::string name, const T& initial_value)
    : _name{std::move(name)}, _value{initial_value}, _data{MakeNodeData<T>(_value)}
{
}

template<typename T>
void Input<T>::operator()() noexcept
{
    ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(36, 36, 36, 255));
    if (InputField<typename std::remove_cvref_t<T>>(_name, _value, ImGuiInputTextFlags_AutoSelectAll))
    {
        _data = MakeNodeData<T>(_value);
    }
    ImGui::PopStyleColor();
}

template class Input<std::string>;
template class Input<std::int8_t>;
template class Input<std::int16_t>;
template class Input<std::int32_t>;
template class Input<std::int64_t>;
template class Input<std::uint8_t>;
template class Input<std::uint16_t>;
template class Input<std::uint32_t>;
template class Input<std::uint64_t>;
template class Input<bool>;
template class Input<float>;
template class Input<double>;
template class Input<std::chrono::nanoseconds>;
template class Input<std::chrono::microseconds>;
template class Input<std::chrono::milliseconds>;
template class Input<std::chrono::seconds>;
template class Input<std::chrono::minutes>;
template class Input<std::chrono::hours>;
template class Input<std::chrono::days>;
template class Input<std::chrono::weeks>;
template class Input<std::chrono::months>;
template class Input<std::chrono::years>;
template class Input<std::filesystem::path>;

FLOW_UI_SUBNAMESPACE_END
