#pragma once

#include "Config.hpp"
#include "Core.hpp"
#include "Texture.hpp"
#include "utilities/Conversions.hpp"
#include "utilities/NodeBuilder.hpp"
#include "views/NodeView.hpp"
#include "widgets/Text.hpp"

#include <flow/core/Node.hpp>
#include <flow/core/NodeFactory.hpp>
#include <hello_imgui/icons_font_awesome_6.h>
#include <imgui.h>
#include <nlohmann/json.hpp>

#include <any>

FLOW_UI_NAMESPACE_BEGIN

struct PreviewNodeView : NodeView
{
    PreviewNodeView(flow::SharedNode node) : NodeView(node), Node(node)
    {
        for (const auto& input : Inputs)
        {
            input->SetShowLabel(false);
        }
    }

    virtual ~PreviewNodeView() = default;

    void Draw() override
    {
        _builder->Begin(this->ID());

        DrawHeader();

        auto input_it     = std::find_if(Inputs.begin(), Inputs.end(), [](const auto& in) { return in->Name == "in"; });
        const auto& input = *input_it;

        input->Draw(_builder);

        _builder->Middle();

        auto data = input->GetData();
        if (!data)
        {
            _builder->End();
            return;
        }

        const auto& factory = Node->GetEnv()->GetFactory();
        if (auto texture_data = factory->Convert<Texture>(data))
        {
            Texture& texture = texture_data->Get();
            ImGui::Image(texture.ID,
                         ImVec2(static_cast<float>(texture.Size.Width), static_cast<float>(texture.Size.Height)));

            _builder->End();
            return;
        }

        std::string data_str = data->ToString();
        if (!data_str.empty())
        {
            if (ImGui::Selectable(data_str.c_str(), false, 0, ImGui::CalcTextSize(data_str.c_str())))
            {
                ImGui::SetClipboardText(data_str.c_str());
            }
        }

        _builder->End();
    }

    SharedNode Node;
};

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
