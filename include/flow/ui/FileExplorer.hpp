// Copyright (c) 2024, Cisco Systems, Inc.
// All rights reserved.

#pragma once

#include "Core.hpp"

#include <filesystem>
#include <string>

FLOW_UI_NAMESPACE_START

/**
 * @brief Native file dialog.
 */
class FileExplorer
{
  public:
    /**
     * @brief Open a native file dialog and get a file path to load.
     *
     * @param default_path The initial path to open the dialog into.
     * @param filter_name The name of the file type filter.
     * @param filter_types The list of comma separated file types to filter for.
     *
     * @returns The path to file to open.
     *
     * @note This merely gets the correct path of the file to be loaded. Actually opening the file using fstream is up
     *       to the user.
     */
    static std::filesystem::path Load(const std::filesystem::path& default_path, std::string filter_name,
                                      std::string filter_types);

    /**
     * @brief Opens a native file dialog with a default save name.
     *
     * @param default_path The default path to open the dialog to.
     * @param data The data to save.
     *
     * @returns The new path of the saved file.
     */
    static std::filesystem::path Save(std::filesystem::path path, std::string_view data);

    /**
     * @brief Gets the user's Home path.
     * @returns The HOME path of the user.
     */
    static std::filesystem::path GetHomePath();

    /**
     * @brief Gets the user's documents path.
     * @returns The HOME/Documents path of the user.
     */
    static std::filesystem::path GetDocumentsPath();

    /**
     * @brief Gets the user's downloads path.
     * @returns The HOME/Downloads path of the user.
     */
    static std::filesystem::path GetDownloadsPath();

    /**
     * @brief Get the system's temp path.
     * @returns The TEMP path of the system.
     */
    static std::filesystem::path GetTempPath();

    /**
     * @brief Get the current executable's path.
     * @returns The path of the current executable.
     */
    static std::filesystem::path GetExecutablePath();
};

FLOW_UI_NAMESPACE_END
