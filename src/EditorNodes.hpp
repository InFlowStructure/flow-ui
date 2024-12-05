#pragma once

#include "Config.hpp"
#include "Core.hpp"
#include "Texture.hpp"
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

        _builder->Header(Color);
        ImGui::Spring(0);

        const auto& name = Node->GetName().c_str();
        if (GetConfig().NodeHeaderFont)
        {
            ImGui::PushFont(GetConfig().NodeHeaderFont);
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
            ImGui::PushFont(GetConfig().IconFont);
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
                ImGui::Image(texture.ID, texture.Size);

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
