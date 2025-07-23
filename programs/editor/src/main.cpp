#include <cxxopts.hpp>
#include <flow/ui/Config.hpp>
#include <flow/ui/Editor.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <csignal>
#include <fstream>
#include <iostream>

int main(int argc, char** argv)

try
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
    app.Run();

    return EXIT_SUCCESS;
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
