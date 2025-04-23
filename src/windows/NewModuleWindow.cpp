#include "NewModuleWindow.hpp"

#include "FileExplorer.hpp"
#include "widgets/InputField.hpp"
#include "widgets/Table.hpp"
#include "widgets/Text.hpp"

#include <imgui.h>

#include <filesystem>
#include <fstream>

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

void GenerateProjectFiles(std::filesystem::path project_dir, const std::string& name, const std::string version,
                          const std::vector<std::string>& dependencies)
{
    if (std::filesystem::exists(project_dir))
    {
        return;
    }

    std::filesystem::create_directories(project_dir);
    std::filesystem::create_directories(project_dir / "src");
    std::filesystem::create_directories(project_dir / "include");

    std::string namespace_str = name;
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

        const std::string find_lib_str  = "find_library({{lib}} NAMES {{lib}} REQUIRED)";
        const std::string find_libs_str = std::accumulate(dependencies.begin(), dependencies.end(), std::string(""),
                                                          [&](const std::string& a, const std::string& b) {
                                                              return a + replace_all(find_lib_str, "{{lib}}", b) + "\n";
                                                          });

        std::ifstream cmake_lists_template_fs(FileExplorer::GetExecutablePath() / "templates" /
                                              "ModuleCMakeLists.txt.in");
        std::stringstream buffer;
        buffer << cmake_lists_template_fs.rdbuf();

        std::string cmake_lists_template = buffer.str();
        cmake_lists_template             = replace_all(cmake_lists_template, "{{name}}", name);
        cmake_lists_template             = replace_all(cmake_lists_template, "{{version}}", version);
        cmake_lists_template             = replace_all(cmake_lists_template, "{{find_libs}}", find_libs_str);
        cmake_lists_template             = replace_all(cmake_lists_template, "{{dependencies}}", dependencies_str);
        cmake_lists_template             = replace_all(cmake_lists_template, "{{export}}", export_str);
        cmake_lists_template             = replace_all(cmake_lists_template, "{{flow_dir}}",
                                                       replace_all(FileExplorer::GetExecutablePath().string(), "\\", "/"));

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
}

void BuildProject(std::filesystem::path project_dir)
{
    const std::filesystem::path build_dir = project_dir / "build";

    std::string configure_cmd = "cmake -S {{project_dir}} -B {{build_dir}}";
    configure_cmd             = replace_all(configure_cmd, "{{project_dir}}", project_dir.string());
    configure_cmd             = replace_all(configure_cmd, "{{build_dir}}", build_dir.string());

    std::string build_cmd = "cmake --build {{build_dir}} -j";
    build_cmd             = replace_all(build_cmd, "{{build_dir}}", build_dir.string());

    std::system(configure_cmd.c_str());
    std::system(build_cmd.c_str());
}

NewModuleWindow::NewModuleWindow() : Window(Name) {}

void NewModuleWindow::Draw()
try
{
    static auto name_input        = std::make_shared<widgets::Input<std::string>>("name", _name);
    static auto version_input     = std::make_shared<widgets::Input<std::string>>("version", _version);
    static auto author_input      = std::make_shared<widgets::Input<std::string>>("author", _author);
    static auto description_input = std::make_shared<widgets::Input<std::string>>("description", _description);
    static auto dependencies      = std::make_shared<widgets::Table>("dependencies", 2);

    static auto flow_core_dep = std::make_shared<widgets::Input<bool>>("flow-core", true);
    if (flow_core_dep.use_count() == 1)
    {
        dependencies->AddEntry(flow_core_dep);
        dependencies->AddEntry(std::make_shared<widgets::Text>("flow-core"));
    }

    static auto flow_ui_dep = std::make_shared<widgets::Input<bool>>("flow-ui", false);
    if (flow_ui_dep.use_count() == 1)
    {
        dependencies->AddEntry(flow_ui_dep);
        dependencies->AddEntry(std::make_shared<widgets::Text>("flow-ui"));
    }

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

    if (auto _ = name_input->GetData())
    {
        _name = name_input->GetValue();
    }

    ImGui::BeginHorizontal("buttons");
    ImGui::Spring();
    if (ImGui::Button("Create"))
    {
        const auto& project_dir = FileExplorer::GetDocumentsPath() / _name;

        GenerateProjectFiles(project_dir, _name, _version, {"flow-core", "flow-ui"});
        BuildProject(project_dir);

        try
        {
            // std::filesystem::copy_file(project_dir / "build" / "Debug" / "cmath.dll", _modules_path / "cmath.dll",
            //                            std::filesystem::copy_options::overwrite_existing);
        }
        catch (...)
        {
        }

        ImGui::CloseCurrentPopup();
    }

    ImGui::SetItemDefaultFocus();
    if (ImGui::Button("Cancel"))
    {
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndHorizontal();
}
catch (...)
{
    Window::Draw();
}

FLOW_UI_NAMESPACE_END
