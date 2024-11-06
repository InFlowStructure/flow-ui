// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "flow/ui/Core.hpp"
#include "flow/ui/Texture.hpp"
#include "flow/ui/Window.hpp"

#include <optional>
#include <string>

namespace ImGuiTexInspect
{
struct Context;
}

FLOW_UI_NAMESPACE_START

class ImageInspectorWindow : public Window
{
  public:
    ImageInspectorWindow(std::optional<Texture>& texture);
    virtual ~ImageInspectorWindow() = default;

    virtual void Init() override;
    virtual void Teardown() override;
    virtual void Draw() override;

    static std::string Name;

    std::optional<Texture>& InspectedTexture;

  private:
    ImGuiTexInspect::Context* _inspector_context = nullptr;
};

FLOW_UI_NAMESPACE_END
