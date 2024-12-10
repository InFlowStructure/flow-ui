// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <string>

FLOW_UI_NAMESPACE_START

using json = nlohmann::json;

enum class FileLocationType
{
    Documents,
    Downloads,
    Temp,
    Home,
};

class FileStorage
{
  public:
    struct Dialog
    {
        Dialog();
        ~Dialog();

        std::string Load(const std::filesystem::path& default_path);
        std::string LoadLib(const std::filesystem::path& default_path);
        std::string Save(std::string_view name, const std::filesystem::path& default_path);
    };

    FileStorage(std::filesystem::path lib_dir, std::filesystem::path save_dir);

    json Load(std::string& filename);
    std::string Save(std::string_view data, std::string_view default_save_name, bool new_save = false);

    std::filesystem::path GetModulesPath() const { return _base_lib_path; }
    std::filesystem::path GetSavePath() const { return _base_save_path; }

    static std::filesystem::path GetHomePath();
    static std::filesystem::path GetDocumentsPath();
    static std::filesystem::path GetDownloadsPath();
    static std::filesystem::path GetTempPath();

  protected:
    std::filesystem::path GetPathByType(FileLocationType type) const;

  private:
    std::filesystem::path _base_lib_path;
    std::filesystem::path _base_save_path;
};

FLOW_UI_NAMESPACE_END
