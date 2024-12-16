// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "FileExplorer.hpp"

#include <nfd.h>
#include <spdlog/spdlog.h>

#include <fstream>
#ifdef FLOW_WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

FLOW_UI_NAMESPACE_START

std::filesystem::path FileExplorer::Load(const std::filesystem::path& filename, std::string filter_name,
                                         std::string filter_types)
{
    const auto& default_path = filename;
    NFD_Init();
    nfdchar_t* outPath;
    nfdfilteritem_t filterItem[1] = {{filter_name.c_str(), filter_types.c_str()}};
    nfdresult_t nfd_result =
        NFD_OpenDialog(&outPath, filterItem, 1, reinterpret_cast<const nfdu8char_t*>(default_path.c_str()));
    if (nfd_result == NFD_OKAY)
    {
        std::string result = outPath;
        NFD_FreePath(outPath);
        NFD_Quit();
        return result;
    }
    else if (nfd_result == NFD_CANCEL)
    {
        NFD_Quit();
        return "";
    }
    NFD_Quit();
    SPDLOG_ERROR("Error opening file: {0}", NFD_GetError());
    return "";
}

std::filesystem::path FileExplorer::Save(std::filesystem::path save_path, std::string_view data)
{
    if (!std::filesystem::exists(save_path))
    {

        nfdchar_t* outPath;
        nfdfilteritem_t filterItem[1] = {{reinterpret_cast<const nfdu8char_t*>(save_path.extension().c_str())}};
        nfdresult_t nfd_result =
            NFD_SaveDialog(&outPath, filterItem, 1, reinterpret_cast<const nfdu8char_t*>(save_path.c_str()),
                           reinterpret_cast<const nfdu8char_t*>(save_path.replace_extension("").filename().c_str()));
        if (nfd_result == NFD_OKAY)
        {
            std::string result = outPath;
            NFD_FreePath(outPath);
            return result;
        }
        else if (nfd_result == NFD_CANCEL)
        {
            return save_path;
        }

        SPDLOG_ERROR("Error opening file: {0}", NFD_GetError());
        return save_path;
    }

    try
    {
        std::ofstream ofs(save_path);
        ofs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        ofs << data;
        ofs.close();

        return save_path;
    }
    catch (const std::ios_base::failure& fail)
    {
        SPDLOG_ERROR("Failed to save file '{0}: {1}", save_path.string(), fail.what());
        return save_path;
    }
}

std::filesystem::path FileExplorer::GetHomePath()
{

#if defined(_WIN32) || defined(WIN32)
    const char* dir = std::getenv("USERPROFILE");
#else
    const char* dir = std::getenv("HOME");
#endif
    if (!dir)
    {
        return std::filesystem::current_path();
    }

    return std::filesystem::path(dir);
}

std::filesystem::path FileExplorer::GetDocumentsPath() { return GetHomePath() / "Documents"; }

std::filesystem::path FileExplorer::GetDownloadsPath() { return GetHomePath() / "Downloads"; }

#ifdef _WIN32
#undef GetTempPath
#endif
std::filesystem::path FileExplorer::GetTempPath() { return std::filesystem::temp_directory_path(); }

std::filesystem::path FileExplorer::GetExecutablePath()
{
#if defined(FLOW_WINDOWS)
    wchar_t path[FILENAME_MAX] = {0};
    GetModuleFileNameW(nullptr, path, FILENAME_MAX);
    return std::filesystem::path(path).parent_path().string();
#else
    char path[FILENAME_MAX];
    ssize_t count = readlink("/proc/self/exe", path, FILENAME_MAX);
    return std::filesystem::path(std::string(path, (count > 0) ? count : 0)).parent_path().string();
#endif
}

FLOW_UI_NAMESPACE_END
