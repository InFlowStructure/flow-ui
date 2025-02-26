#pragma once

#include "Config.hpp"
#include "Core.hpp"
#include "Texture.hpp"
#include "utilities/Builders.hpp"
#include "utilities/Conversions.hpp"
#include "views/NodeView.hpp"

#include <flow/core/Node.hpp>
#include <flow/core/NodeFactory.hpp>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui.h>

FLOW_UI_NAMESPACE_START

struct PreviewNodeView : NodeView
{
    using NodeView::NodeView;

    virtual ~PreviewNodeView() = default;

    void Draw() override
    {
        _builder->Begin(this->ID());

        _builder->Header(utility::to_ImColor(HeaderColour));
        ImGui::Spring(0);

        const auto& name = Node->GetName().c_str();
        if (GetConfig().NodeHeaderFont)
        {
            ImGui::PushFont(std::bit_cast<ImFont*>(GetConfig().NodeHeaderFont.get()));
            ImGui::TextUnformatted(name);
            ImGui::PopFont();
        }
        else
        {
            ImGui::TextUnformatted(name);
        }

        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));
        ImGui::Spring(0);

        if (GetConfig().IconFont)
        {
            ImGui::PushFont(std::bit_cast<ImFont*>(GetConfig().IconFont.get()));
        }

        const bool should_copy = ImGui::Button(ICON_FA_COPY);

        if (GetConfig().IconFont)
        {
            ImGui::PopFont();
        }

        _builder->EndHeader();

        auto input_it = std::find_if(Inputs.begin(), Inputs.end(), [](const auto& in) { return in->Name() == "in"; });
        const auto& input = *input_it;
        input->SetShowLabel(false);

        input->Draw();

        _builder->Middle();

        if (auto data = input->GetData())
        {
            const auto& factory = Node->GetEnv()->GetFactory();
            if (auto texture_data = factory->Convert<Texture>(data))
            {
                Texture& texture = texture_data->Get();
                ImGui::Image(texture.ID,
                             ImVec2(static_cast<float>(texture.Size.Width), static_cast<float>(texture.Size.Height)));

                if (should_copy)
                {
                    // TODO: Copy image data not implemented
                }
                _builder->End();
                return;
            }

            std::string data_str = data->ToString();
            if (!data_str.empty())
            {
                if (should_copy)
                {
                    ImGui::SetClipboardText(data_str.c_str());
                }

                ImGui::TextUnformatted(data_str.c_str());
            }
        }

        _builder->End();
    }
};

template<typename F>
struct FunctionTraits;

template<typename R, typename... Args>
struct FunctionTraits<R(Args...)>
{
    using ReturnType = std::invoke_result_t<R(Args...), Args...>;
    using ArgTypes   = std::tuple<Args...>;
};

template<typename F, std::add_pointer_t<F> Func>
class FunctionWrapperNode : public Node
{
  protected:
    using FuncTraits = FunctionTraits<F>;
    using OutputType = typename FuncTraits::ReturnType;

  private:
    template<int... Idx>
    void AddInputs(std::integer_sequence<int, Idx...>)
    {
        (AddInput<std::tuple_element_t<Idx, typename FuncTraits::ArgTypes>>(
             {input_names[Idx] = "in" + std::to_string(Idx)}, ""),
         ...);
    }

    template<int... Idx>
    auto GetInputs(std::integer_sequence<int, Idx...>)
    {
        return std::make_tuple(
            GetInputData<std::tuple_element_t<Idx, typename FuncTraits::ArgTypes>>(IndexableName{input_names[Idx]})...);
    }

    template<int... Idx>
    json SaveInputs(std::integer_sequence<int, Idx...>) const
    {
        json inputs_json;
        (
            [&, this] {
                const auto& key = input_names[Idx];
                if (auto x = GetInputData<std::tuple_element_t<Idx, typename FuncTraits::ArgTypes>>(IndexableName{key}))
                {
                    inputs_json[key] = x->Get();
                }
            }(),
            ...);

        return inputs_json;
    }

    template<int... Idx>
    void RestoreInputs(json& j, std::integer_sequence<int, Idx...>)
    {
        (
            [&, this] {
                const auto& key = input_names[Idx];
                if (!j.contains(key))
                {
                    return;
                }

                SetInputData(IndexableName{key},
                             MakeNodeData<std::tuple_element_t<Idx, typename FuncTraits::ArgTypes>>(j[key]), false);
            }(),
            ...);
    }

  public:
    explicit FunctionWrapperNode(const std::string& uuid_str, const std::string& name, std::shared_ptr<Env> env)
        : Node(uuid_str, TypeName_v<FunctionWrapperNode<F, Func>>, name, std::move(env)), _func{Func}
    {
        AddInputs(std::make_integer_sequence<int, std::tuple_size_v<typename FuncTraits::ArgTypes>>{});

        AddOutput<OutputType>("result", "result");
    }

    virtual ~FunctionWrapperNode() = default;

  protected:
    void Compute() override
    {
        auto inputs = GetInputs(std::make_integer_sequence<int, std::tuple_size_v<typename FuncTraits::ArgTypes>>{});

        if (std::apply([](auto&&... args) { return (!args || ...); }, inputs)) return;

        auto result = std::apply([&](auto&&... args) { return _func(args->Get()...); }, inputs);
        this->SetOutputData("result", MakeNodeData(std::move(result)));
    }

    json SaveInputs() const override
    {
        return SaveInputs(std::make_integer_sequence<int, std::tuple_size_v<typename FuncTraits::ArgTypes>>{});
    }

    void RestoreInputs(const json& j) override
    {
        RestoreInputs(const_cast<json&>(j),
                      std::make_integer_sequence<int, std::tuple_size_v<typename FuncTraits::ArgTypes>>{});
    }

  private:
    std::add_pointer_t<F> _func;
    std::array<std::string, std::tuple_size_v<typename FuncTraits::ArgTypes>> input_names{""};
};

#ifndef FLOW_WINDOWS
#define WRAP_FUNCTION_IN_NODE_TYPE(func, ...) FunctionWrapperNode<decltype(func), func __VA_OPT__(, ) __VA_ARGS__>
#else
#define WRAP_FUNCTION_IN_NODE_TYPE(func, ...) FunctionWrapperNode<decltype(func), func, __VA_ARGS__>
#endif

FLOW_UI_NAMESPACE_END

struct PreviewNode : public flow::Node
{
    explicit PreviewNode(const std::string& uuid_str, const std::string& name, std::shared_ptr<flow::Env> env)
        : flow::Node(uuid_str, flow::TypeName_v<PreviewNode>, name, std::move(env))
    {
        AddInput<std::any>("in", "");
    }

    virtual ~PreviewNode() = default;

    void Compute() override {}
};
