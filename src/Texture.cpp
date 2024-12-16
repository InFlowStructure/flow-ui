#include "Texture.hpp"

#include "Config.hpp"

#include <spdlog/spdlog.h>

#include <hello_imgui/internal/image_dx11.h>
#include <hello_imgui/internal/image_metal.h>
#include <hello_imgui/internal/image_opengl.h>
#include <hello_imgui/internal/image_vulkan.h>

namespace
{
HelloImGui::ImageAbstractPtr CreateImage()
{
    auto render_backend = flow::ui::GetConfig().RenderBackend;
#ifdef HELLOIMGUI_HAS_OPENGL
    if (render_backend == flow::ui::RendererBackend::OpenGL3) return std::make_shared<HelloImGui::ImageOpenGl>();
#endif
#if (HELLOIMGUI_HAS_METAL)
    if (render_backend == flow::ui::RendererBackend::Metal) return std::make_shared<HelloImGui::ImageMetal>();
#endif
#if (HELLOIMGUI_HAS_VULKAN)
    if (render_backend == flow::ui::RendererBackend::Vulkan) return std::make_shared<HelloImGui::ImageVulkan>();
#endif
#if (HELLOIMGUI_HAS_DIRECTX11)
    if (render_backend == flow::ui::RendererBackend::DirectX11) return std::make_shared<HelloImGui::ImageDx11>();
#endif

    return nullptr;
}

HelloImGui::ImageAbstractPtr CreateImageFromID(ImTextureID id)
{
    HelloImGui::ImageAbstractPtr concrete_image = CreateImage();
#ifdef HELLOIMGUI_HAS_OPENGL
    if (auto image = std::dynamic_pointer_cast<HelloImGui::ImageOpenGl>(concrete_image))
        image->TextureId = static_cast<unsigned int>(std::bit_cast<intptr_t>(id));
#endif
#if (HELLOIMGUI_HAS_METAL)
    if (auto image = std::dynamic_pointer_cast<HelloImGui::ImageMetal>(concrete_image))
        image->TextureId = static_cast<unsigned int>(std::bit_cast<intptr_t>(id));
#endif
#if (HELLOIMGUI_HAS_VULKAN)
    if (auto image = std::dynamic_pointer_cast<HelloImGui::ImageVulkan>(concrete_image))
        image->DS = static_cast<VkDescriptorSet>(id);
#endif
#if (HELLOIMGUI_HAS_DIRECTX11)
    if (auto image = std::dynamic_pointer_cast<HelloImGui::ImageDx11>(concrete_image))
        image->ShaderResourceView = std::bit_cast<ID3D11ShaderResourceView*>(std::bit_cast<uintptr_t>(id));
#endif

    return concrete_image;
}

std::unordered_map<ImTextureID, HelloImGui::ImageAbstractPtr> CachedImages;
HelloImGui::ImageAbstractPtr CacheImage(unsigned char* data, int width, int height)
{
    if (!data || width == 0 || height == 0) return nullptr;

    HelloImGui::ImageAbstractPtr concrete_image = CreateImage();

    if (concrete_image == nullptr)
    {
        SPDLOG_WARN("CacheImage: not implemented for this rendering backend!");
        return nullptr;
    }

    concrete_image->_impl_StoreTexture(width, height, data);
    CachedImages[concrete_image->TextureID()] = concrete_image;
    return concrete_image;
}
} // namespace

flow::ui::Texture::Texture(std::uint8_t* d, int w, int h)
{
    auto image = CacheImage(d, w, h);
    ID         = image->TextureID();
    Size       = ImVec2(static_cast<float>(w), static_cast<float>(h));
}

flow::ui::Texture::Texture(unsigned int texture_id, int w, int h)
    : Texture(std::bit_cast<ImTextureID>(static_cast<intptr_t>(texture_id)),
              ImVec2(static_cast<float>(w), static_cast<float>(h)))
{
}

flow::ui::Texture::Texture(ImTextureID id, ImVec2 size) : ID{id}, Size{size}
{
    HelloImGui::ImageAbstractPtr concrete_image = CreateImageFromID(id);
    concrete_image->Width                       = static_cast<int>(size.x);
    concrete_image->Height                      = static_cast<int>(size.y);
    CachedImages[concrete_image->TextureID()]   = concrete_image;
}
