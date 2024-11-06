// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#include "FileStorage.hpp"

#include <nfd.h>
#include <spdlog/spdlog.h>

#include <fstream>

FLOW_UI_NAMESPACE_START

FileStorage::Dialog::Dialog() { NFD_Init(); }
FileStorage::Dialog::~Dialog() { NFD_Quit(); }

std::string FileStorage::Dialog::Load(const std::filesystem::path& default_path)
{
    nfdchar_t* outPath;
    nfdfilteritem_t filterItem[1] = {{"Flow files", "flow"}};
    nfdresult_t nfd_result =
        NFD_OpenDialog(&outPath, filterItem, 1, reinterpret_cast<const nfdu8char_t*>(default_path.c_str()));
    if (nfd_result == NFD_OKAY)
    {
        std::string result = outPath;
        NFD_FreePath(outPath);
        return result;
    }
    else if (nfd_result == NFD_CANCEL)
    {
        return "";
    }

    SPDLOG_ERROR("Error opening flow: {0}", NFD_GetError());
    return "";
}

std::string FileStorage::Dialog::LoadLib(const std::filesystem::path& default_path)
{
    nfdchar_t* outPath;
    nfdfilteritem_t filterItem[3] = {{"Modules", "so,dll,dylib"}, {"Dynamic Libs", "dll"}, {"Dynamic Libs", "dylib"}};
    nfdresult_t nfd_result =
        NFD_OpenDialog(&outPath, filterItem, 3, reinterpret_cast<const nfdu8char_t*>(default_path.c_str()));
    if (nfd_result == NFD_OKAY)
    {
        std::string result = outPath;
        NFD_FreePath(outPath);
        return result;
    }
    else if (nfd_result == NFD_CANCEL)
    {
        return "";
    }

    SPDLOG_ERROR("Error opening flow: {0}", NFD_GetError());
    return "";
}

std::string FileStorage::Dialog::Save(std::string_view name, const std::filesystem::path& default_path)
{
    nfdchar_t* outPath;
    nfdfilteritem_t filterItem[1] = {{"Flow files", "flow"}};
    nfdresult_t nfd_result        = NFD_SaveDialog(&outPath, filterItem, 1,
                                                   reinterpret_cast<const nfdu8char_t*>(default_path.c_str()), name.data());
    if (nfd_result == NFD_OKAY)
    {
        std::string result = outPath;
        NFD_FreePath(outPath);
        return result;
    }
    else if (nfd_result == NFD_CANCEL)
    {
        return "";
    }

    SPDLOG_ERROR("Error opening flow: {0}", NFD_GetError());
    return "";
}

FileStorage::FileStorage(std::filesystem::path lib_dir, std::filesystem::path save_dir)
    : _base_lib_path{lib_dir}, _base_save_path{save_dir}
{
    if (!std::filesystem::create_directories(_base_lib_path))
    {
        SPDLOG_TRACE("Lib directory '{0}' already exists, not creating", _base_lib_path.string());
    }

    if (!std::filesystem::create_directories(_base_save_path))
    {
        SPDLOG_TRACE("Save directory '{0}' already exists, not creating", _base_save_path.string());
    }
}

json FileStorage::Load(std::string& filename)
{
    if (filename.empty())
    {
        Dialog d;

        filename = d.Load(GetSavePath() / filename);
        if (filename.empty()) return {};
    }

    json j;
    try
    {
        std::ifstream i;
        i.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        i.open(filename);
        i >> j;
        i.close();
    }
    catch (const std::exception& e)
    {
        SPDLOG_ERROR("Failed to load file '{0}: {1}", std::filesystem::path(filename).filename().string(), e.what());
        return {};
    }

    return j;
}

std::string FileStorage::Save(std::string_view data, std::string_view default_save_name, bool new_save)
{
    std::string filename = std::string{default_save_name} + ".flow";

    if (!new_save)
    {
        new_save = !std::filesystem::exists(GetSavePath() / filename);
    }

    if (new_save)
    {
        Dialog d;

        filename = d.Save(filename, GetSavePath());
        if (filename.empty()) return std::string{default_save_name};
    }

    try
    {
        std::ofstream ofs(_base_save_path / filename);
        ofs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        ofs << data;
        ofs.close();

        const auto pos = filename.find_last_of("/") + 1;
        return filename.substr(pos, filename.length() - pos - 5);
    }
    catch (const std::ios_base::failure& fail)
    {
        SPDLOG_ERROR("Failed to save file '{0}: {1}", filename, fail.what());
        return std::string{default_save_name};
    }
}

#if defined(_WIN32) || defined(WIN32)
std::filesystem::path FileStorage::GetHomePath()
{

    const char* dir = std::getenv("USERPROFILE");
    if (!dir)
    {
        return std::filesystem::current_path();
    }

    return std::filesystem::path(dir);
}
#else
std::filesystem::path FileStorage::GetHomePath()
{

    const char* dir = std::getenv("HOME");
    if (!dir)
    {
        return std::filesystem::current_path();
    }

    return std::filesystem::path(dir);
}
#endif

std::filesystem::path FileStorage::GetDocumentsPath() { return GetHomePath() / "Documents"; }

std::filesystem::path FileStorage::GetDownloadsPath() { return GetHomePath() / "Downloads"; }

#ifdef _WIN32
#undef GetTempPath
#endif
std::filesystem::path FileStorage::GetTempPath() { return std::filesystem::temp_directory_path(); }

std::filesystem::path FileStorage::GetPathByType(FileLocationType type) const
{
    switch (type)
    {
    case FileLocationType::Documents:
        return GetDocumentsPath();
    case FileLocationType::Downloads:
        return GetDownloadsPath();
    case FileLocationType::Temp:
        return GetTempPath();
    default:
        return GetHomePath();
    }
}

FLOW_UI_NAMESPACE_END
