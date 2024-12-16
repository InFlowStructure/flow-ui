// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <filesystem>
#include <string>

FLOW_UI_NAMESPACE_START

/**
 * @brief
 */
class FileExplorer
{
  public:
    FileExplorer() = default;

    static std::filesystem::path Load(const std::filesystem::path& filename, std::string filter_name,
                                      std::string filter_types);
    static std::filesystem::path Save(std::filesystem::path path, std::string_view data);

    static std::filesystem::path GetHomePath();
    static std::filesystem::path GetDocumentsPath();
    static std::filesystem::path GetDownloadsPath();
    static std::filesystem::path GetTempPath();
    static std::filesystem::path GetExecutablePath();
};

FLOW_UI_NAMESPACE_END
