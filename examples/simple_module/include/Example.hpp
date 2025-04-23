// Copyright (c) 2024,Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include <flow/core/Node.hpp>
#include <flow/core/NodeData.hpp>

#include <string>

namespace flow::simple
{
class ExampleNode : public Node
{
  public:
    explicit ExampleNode(const std::string& uuid_str, const std::string& name, std::shared_ptr<Env> env)
        : Node(uuid_str, flow::TypeName_v<ExampleNode>, name, std::move(env))
    {
        AddInput<std::string>("name", "Input name");
        AddOutput<std::string>("greeting", "A greeting to the specified user");
    }

    virtual ~ExampleNode() = default;

  protected:
    void Compute() override
    {
        if (auto name_data = GetInputData<std::string>("name"))
        {
            const std::string& name = name_data->Get();
            SetOutputData("greeting", flow::MakeNodeData("Hello " + name + "!"));
        }
    }

    json SaveInputs() const override
    {
        if (auto name_data = GetInputData<std::string>("name"))
        {
            const std::string& name = name_data->Get();
            return {{"name", name}};
        }

        return {};
    }

    void RestoreInputs(const json& j) override
    {
        if (j.contains("name"))
        {
            SetInputData("name", flow::MakeNodeData(j["name"].get_ref<const std::string&>()));
        }
    }
};
} // namespace flow::simple
