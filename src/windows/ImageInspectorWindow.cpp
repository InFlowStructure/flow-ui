// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "ImageInspectorWindow.hpp"

#include "Config.hpp"
#include "Widgets.hpp"

#include <imgui.h>
#include <imgui_tex_inspect/imgui_tex_inspect.h>
#ifdef HELLOIMGUI_HAS_OPENGL3
#include <imgui_tex_inspect/backends/tex_inspect_opengl.h>
#elif defined(HELLOIMGUI_HAS_DIRECTX11)
#include <imgui_tex_inspect/backends/tex_inspect_directx11.h>
#endif
#include <spdlog/spdlog.h>

FLOW_UI_NAMESPACE_START

std::string ImageInspectorWindow::Name = "Image Inspector";

ImageInspectorWindow::ImageInspectorWindow(std::optional<Texture>& texture)
    : Window(ImageInspectorWindow::Name, DefaultDockspaces::Property), InspectedTexture{texture}
{
}

void ImageInspectorWindow::Init()
{
    auto renderer_backend_type = flow::ui::GetConfig().RenderBackend;

#ifdef HELLOIMGUI_HAS_OPENGL
    if (renderer_backend_type == HelloImGui::RendererBackendType::OpenGL3) ImGuiTexInspect::ImplOpenGL3_Init();
#endif
#ifdef HELLOIMGUI_HAS_DIRECTX11
    if (renderer_backend_type == HelloImGui::RendererBackendType::DirectX11) ImGuiTexInspect::ImplDirectx11_Init();
#endif
    ImGuiTexInspect::Init();
    _inspector_context = ImGuiTexInspect::CreateContext();
}

void ImageInspectorWindow::Teardown() { ImGuiTexInspect::DestroyContext(_inspector_context); }

void ImageInspectorWindow::Draw()
{
    if (!InspectedTexture)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(175, 175, 175, 255));
        widgets::TextCentered("Nothing to show");
        ImGui::PopStyleColor();
        return;
    }

    ImGuiTexInspect::BeginInspectorPanel("Inspector", InspectedTexture->ID, InspectedTexture->Size);
    ImGuiTexInspect::DrawAnnotations(ImGuiTexInspect::ValueText(ImGuiTexInspect::ValueText::Floats));
    ImGuiTexInspect::EndInspectorPanel();
}

FLOW_UI_NAMESPACE_END
