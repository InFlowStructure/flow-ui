#include "NewModuleWindow.hpp"

#include "FileExplorer.hpp"
#include "ModuleInfo.hpp"
#include "widgets/Text.hpp"

#include <imgui.h>

#include <filesystem>
#include <fstream>
#include <string>

using namespace std::string_literals;

FLOW_UI_NAMESPACE_START

std::string replace_all(std::string str, const std::string& from, const std::string& to)
{
    std::size_t start_pos = 0;
    while ((start_pos = str.find(from)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return str;
}

void GenerateProjectFiles(std::filesystem::path project_dir, const ModuleInfo& info,
                          const std::vector<std::string>& dependencies)
{
    if (std::filesystem::exists(project_dir))
    {
        return;
    }

    std::filesystem::create_directories(project_dir);
    std::filesystem::create_directories(project_dir / "src");
    std::filesystem::create_directories(project_dir / "include");

    std::string namespace_str = info.Name;
    namespace_str             = replace_all(namespace_str, "-", "_");
    namespace_str             = replace_all(namespace_str, " ", "_");

    std::string upper_case_ns_str = namespace_str;

    const auto& to_upper = [](unsigned char c) { return static_cast<unsigned char>(std::toupper(c)); };
    std::transform(upper_case_ns_str.begin(), upper_case_ns_str.end(), upper_case_ns_str.begin(), to_upper);

    const std::string export_str = upper_case_ns_str + "_EXPORT";
    const std::string api_str    = upper_case_ns_str + "_API";

    // CMakeLists.txt
    {
        const std::string dependencies_str = std::accumulate(
            dependencies.begin(), dependencies.end(), std::string(""), [](const std::string& a, const std::string& b) {
                return a.empty() ? "${" + b + "}" : a + " " + "${" + b + "}";
            });

        const std::string find_lib_str = "find_library({{lib}} NAMES {{lib}} PATH_SUFFIXES lib REQUIRED)\n"s +
                                         "get_filename_component({{lib}}_PATH ${{{lib}}} DIRECTORY)\n"s +
                                         "get_filename_component({{lib}}_PATH ${{{lib}}_PATH} DIRECTORY)\n"s +
                                         "set({{lib}}_INCLUDE_DIR \"${{{lib}}_PATH}/include\")\n"s +
                                         "list(APPEND FLOW_INCLUDE_DIR ${{{lib}}_INCLUDE_DIR})"s;

        const std::string find_libs_str = std::accumulate(dependencies.begin(), dependencies.end(), std::string(""),
                                                          [&](const std::string& a, const std::string& b) {
                                                              return a + replace_all(find_lib_str, "{{lib}}", b) + "\n";
                                                          });

        std::ifstream cmake_lists_template_fs(FileExplorer::GetExecutablePath() / "templates" /
                                              "ModuleCMakeLists.txt.in");
        std::stringstream buffer;
        buffer << cmake_lists_template_fs.rdbuf();

        std::string cmake_lists_template = buffer.str();

        cmake_lists_template = replace_all(cmake_lists_template, "{{name}}", info.Name);
        cmake_lists_template = replace_all(cmake_lists_template, "{{version}}", info.Version);
        cmake_lists_template = replace_all(cmake_lists_template, "{{find_libs}}", find_libs_str);
        cmake_lists_template = replace_all(cmake_lists_template, "{{dependencies}}", dependencies_str);
        cmake_lists_template = replace_all(cmake_lists_template, "{{export}}", export_str);

        std::ofstream cmake_lists_fs(project_dir / "CMakeLists.txt");
        cmake_lists_fs << cmake_lists_template;
    }

    // register.cpp
    {
        std::ifstream register_source_template_fs(FileExplorer::GetExecutablePath() / "templates" / "register.cpp.in");
        std::stringstream buffer;
        buffer << register_source_template_fs.rdbuf();

        std::string register_source_template = buffer.str();
        register_source_template             = replace_all(register_source_template, "{{namespace}}", namespace_str);
        register_source_template             = replace_all(register_source_template, "{{export}}", export_str);
        register_source_template             = replace_all(register_source_template, "{{api}}", api_str);

        std::ofstream register_source_fs(project_dir / "src" / "register.cpp");
        register_source_fs << register_source_template;
    }

    // module.flowmod
    {
        std::ofstream module_file_fs(project_dir / (info.Name + ".flowmod"));
        module_file_fs << json(info).dump(4);
    }
}

bool BuildProject(std::filesystem::path project_dir)
{
    const std::filesystem::path build_dir = project_dir / "build";

    std::string configure_cmd = "cmake -S {{project_dir}} -B {{build_dir}}";
    configure_cmd             = replace_all(configure_cmd, "{{project_dir}}", project_dir.string());
    configure_cmd             = replace_all(configure_cmd, "{{build_dir}}", build_dir.string());

    std::string build_cmd = "cmake --build {{build_dir}} -j";
    build_cmd             = replace_all(build_cmd, "{{build_dir}}", build_dir.string());

    if (std::system(configure_cmd.c_str()))
    {
        return false;
    }

    if (std::system(build_cmd.c_str()))
    {
        return false;
    }

    return true;
}

NewModuleWindow::NewModuleWindow() : Window(Name)
{
    name_input        = std::make_shared<widgets::Input<std::string>>("name", "");
    version_input     = std::make_shared<widgets::Input<std::string>>("version", "");
    author_input      = std::make_shared<widgets::Input<std::string>>("author", "");
    description_input = std::make_shared<widgets::Input<std::string>>("description", "");
    dependencies      = std::make_shared<widgets::Table>("dependencies", 2);

    auto flow_core_dep = std::make_shared<widgets::Input<bool>>("flow-core", true);
    dependencies->AddEntry(flow_core_dep);
    dependencies->AddEntry(std::make_shared<widgets::Text>("flow-core"));

    auto flow_ui_dep = std::make_shared<widgets::Input<bool>>("flow-ui", false);
    dependencies->AddEntry(flow_ui_dep);
    dependencies->AddEntry(std::make_shared<widgets::Text>("flow-ui"));
}

void NewModuleWindow::Clear()
{
    name_input.reset(new widgets::Input<std::string>("name", ""));
    version_input.reset(new widgets::Input<std::string>("version", ""));
    author_input.reset(new widgets::Input<std::string>("author", ""));
    description_input.reset(new widgets::Input<std::string>("description", ""));
    dependencies.reset(new widgets::Table("dependencies", 2));

    auto flow_core_dep = std::make_shared<widgets::Input<bool>>("flow-core", true);
    dependencies->AddEntry(flow_core_dep);
    dependencies->AddEntry(std::make_shared<widgets::Text>("flow-core"));

    auto flow_ui_dep = std::make_shared<widgets::Input<bool>>("flow-ui", false);
    dependencies->AddEntry(flow_ui_dep);
    dependencies->AddEntry(std::make_shared<widgets::Text>("flow-ui"));
}

bool NewModuleWindow::DrawAndCreate()
try
{
    widgets::Table new_module_form("new_module_form", 2);
    new_module_form.AddEntry(std::make_shared<widgets::Text>("Name"));
    new_module_form.AddEntry(name_input);
    new_module_form.AddEntry(std::make_shared<widgets::Text>("Version"));
    new_module_form.AddEntry(version_input);
    new_module_form.AddEntry(std::make_shared<widgets::Text>("Author"));
    new_module_form.AddEntry(author_input);
    new_module_form.AddEntry(std::make_shared<widgets::Text>("Description"));
    new_module_form.AddEntry(description_input);
    new_module_form.AddEntry(std::make_shared<widgets::Text>("Dependencies"));
    new_module_form.AddEntry(dependencies);

    new_module_form();

    bool created = false;
    ImGui::BeginHorizontal("buttons");
    ImGui::Spring();
    if (ImGui::Button("Create"))
    {
        ImGui::CloseCurrentPopup();

        const auto& name        = name_input->GetValue();
        const auto& project_dir = FileExplorer::GetDocumentsPath() / name;

        const auto& flow_core_dep = std::static_pointer_cast<widgets::Input<bool>>(dependencies->GetEntry(0));
        const auto& flow_ui_dep   = std::static_pointer_cast<widgets::Input<bool>>(dependencies->GetEntry(2));
        std::vector<std::string> chosen_deps;

        if (flow_core_dep->GetValue())
        {
            chosen_deps.push_back("flow-core");
        }

        if (flow_ui_dep->GetValue())
        {
            chosen_deps.push_back("flow-ui");
        }

        GenerateProjectFiles(project_dir,
                             ModuleInfo{
                                 .Name        = name,
                                 .Version     = version_input->GetValue(),
                                 .Author      = author_input->GetValue(),
                                 .Description = description_input->GetValue(),
                             },
                             chosen_deps);

        created = BuildProject(project_dir);
    }

    ImGui::SetItemDefaultFocus();
    if (ImGui::Button("Cancel"))
    {
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndHorizontal();

    return created;
}
catch (...)
{
    Window::Draw();
    return false;
}

FLOW_UI_NAMESPACE_END
