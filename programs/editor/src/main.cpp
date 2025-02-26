#include <cxxopts.hpp>
#include <flow/ui/Config.hpp>
#include <flow/ui/Editor.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <csignal>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    std::string filename;

#ifndef FLOW_WINDOWS
    // clang-format off
    cxxopts::Options options("FlowEditor");
    options.add_options()
        ("f,flow", "Flow file to open", cxxopts::value<std::string>())
        ("l,log_level", "Logging level [trace = 0, debug = 1, info = 2, warn = 3, err = 4, critical = 5, off = 6]", cxxopts::value<int>())
        ("h,help", "Print usage");
    // clang-format on

    cxxopts::ParseResult result;

    try
    {
        result = options.parse(argc, argv);
    }
    catch (const cxxopts::exceptions::exception& e)
    {
        std::cerr << "Caught exception while parsing arguments: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (result.count("help"))
    {
        std::cerr << options.help() << std::endl;
        return EXIT_SUCCESS;
    }

    if (result.count("flow"))
    {
        filename = result["flow"].as<std::string>();
    }

    if (result.count("log_level"))
    {
        spdlog::set_level(static_cast<spdlog::level::level_enum>(result["log_level"].as<int>()));
    }
#endif

    flow::ui::Editor app(filename);

    app.LoadFonts = [](flow::ui::Config& config) {
        config.DefaultFont    = flow::ui::LoadFont("fonts/DroidSans.ttf", 18.f);
        config.NodeHeaderFont = flow::ui::LoadFont("fonts/DroidSans.ttf", 20.f);
        config.IconFont       = flow::ui::LoadFont("fonts/fontawesome-webfont.ttf", 18.f);
    };

    app.SetupStyle = [](flow::ui::Style& style) {
        style.WindowBorderSize = 5.f;
        style.FrameBorderSize  = 2.f;
        style.TabRounding      = 8.f;
        style.TabBarBorderSize = 0.f;
        style.CellPadding      = {.Width = 7.f, .Height = 7.f};

        auto& imgui_colours = style.Colours.BaseColours;
        using BaseColour    = flow::ui::Style::BaseColour;

        imgui_colours[BaseColour::WindowBg]          = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::PopupBg]           = flow::ui::Colour(15, 15, 15, 175);
        imgui_colours[BaseColour::Border]            = flow::ui::Colour(15, 15, 15);
        imgui_colours[BaseColour::PopupBg]           = imgui_colours[BaseColour::WindowBg];
        imgui_colours[BaseColour::FrameBg]           = flow::ui::Colour(15, 15, 15);
        imgui_colours[BaseColour::MenuBarBg]         = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::TitleBg]           = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::TitleBgActive]     = imgui_colours[BaseColour::TitleBg];
        imgui_colours[BaseColour::Tab]               = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::TabDimmed]         = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::TabHovered]        = flow::ui::Colour(47, 47, 47);
        imgui_colours[BaseColour::TabSelected]       = flow::ui::Colour(3, 98, 195);
        imgui_colours[BaseColour::TabDimmedSelected] = imgui_colours[BaseColour::TabSelected];
        imgui_colours[BaseColour::Button]            = flow::ui::Colour(32, 32, 32);
        imgui_colours[BaseColour::ButtonHovered]     = flow::ui::Colour(3, 98, 195);
        imgui_colours[BaseColour::ButtonActive]      = flow::ui::Colour(13, 39, 77);
        imgui_colours[BaseColour::ScrollbarBg]       = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::ScrollbarGrab]     = flow::ui::Colour(86, 86, 86);
        imgui_colours[BaseColour::TableBorderLight]  = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::TableBorderStrong] = flow::ui::Colour(21, 21, 21);
        imgui_colours[BaseColour::TableRowBg]        = flow::ui::Colour(36, 36, 36);
        imgui_colours[BaseColour::TableRowBgAlt]     = flow::ui::Colour(36, 36, 36);
        imgui_colours[BaseColour::Header]            = flow::ui::Colour(47, 47, 47);
        imgui_colours[BaseColour::HeaderHovered]     = flow::ui::Colour(50, 50, 50);
        imgui_colours[BaseColour::CheckMark]         = flow::ui::Colour(3, 98, 195);

        auto& colours      = style.Colours.EditorColours;
        using EditorColour = flow::ui::Style::EditorColour;

        colours[EditorColour::Bg]                  = flow::ui::Colour(38, 38, 38);
        colours[EditorColour::Grid]                = flow::ui::Colour(52, 52, 52);
        colours[EditorColour::NodeBg]              = flow::ui::Colour(15, 17, 15, 240);
        colours[EditorColour::NodeBorder]          = flow::ui::Colour(0, 0, 0);
        colours[EditorColour::SelNodeBorder]       = flow::ui::Colour(255, 255, 255);
        colours[EditorColour::Flow]                = flow::ui::Colour(32, 191, 85);
        colours[EditorColour::FlowMarker]          = flow::ui::Colour(32, 191, 85);
        colours[EditorColour::HighlightLinkBorder] = flow::ui::Colour(0, 188, 235);
        colours[EditorColour::SelLinkBorder]       = flow::ui::Colour(0, 188, 235);
    };

    try
    {
        app.Run();
    }
    catch (const std::exception& e)
    {
        SPDLOG_CRITICAL("Exiting with error: {0}", e.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        SPDLOG_CRITICAL("Exiting with unknown error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
