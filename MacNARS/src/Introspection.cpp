#include "Introspection.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

std::string Introspection::load_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<OptimizationSuggestion> Introspection::evaluate_file(const std::string& path) {
    std::vector<OptimizationSuggestion> suggestions;
    std::ifstream file(path);
    if (!file.is_open()) return suggestions;

    std::string line;
    int line_number = 1;
    
    // NAL-9 proof of concept evaluation rules:
    std::regex push_back_pattern("push_back");
    std::regex copy_map_pattern("auto\\s+\\w+\\s*=\\s*\\w+\\.find"); // auto instead of const auto& (though auto it = find() is okay, we'll look for copy loops)
    std::regex pass_by_value("void\\s+\\w+\\s*\\(\\s*Term\\s+\\w+\\s*\\)"); // passing Term by value
    std::regex cycle_empty("void\\s+Memory::cycle\\(\\)\\s*\\{\\s*//.*\\s*\\}"); // empty cycle method

    while (std::getline(file, line)) {
        if (std::regex_search(line, push_back_pattern)) {
            suggestions.push_back({path, line_number, "NAL-9: Consider using 'emplace_back' instead of 'push_back' to avoid unnecessary object copies and improve memory performance.", line});
        }
        if (std::regex_search(line, pass_by_value)) {
            suggestions.push_back({path, line_number, "NAL-9: 'Term' is passed by value. Pass by 'const reference' to optimize reasoning speed.", line});
        }
        if (std::regex_search(line, cycle_empty)) {
            suggestions.push_back({path, line_number, "NAL-9: 'Memory::cycle' is empty. Recommend implementing task selection and inference logic.", line});
        }
        line_number++;
    }
    return suggestions;
}

void Introspection::run_introspection(const std::string& src_directory) {
    std::cout << "[NAL-9 Introspection] Starting self-reflection on directory: " << src_directory << std::endl;
    for (const auto& entry : fs::directory_iterator(src_directory)) {
        if (entry.is_regular_file() && (entry.path().extension() == ".cpp" || entry.path().extension() == ".mm" || entry.path().extension() == ".h")) {
            std::cout << "[NAL-9] Evaluating " << entry.path().filename().string() << "..." << std::endl;
            auto suggestions = evaluate_file(entry.path().string());
            for (const auto& sug : suggestions) {
                std::cout << "\n  -> OPTIMIZATION FOUND [" << fs::path(sug.file_path).filename().string() << ":" << sug.line_number << "]" << std::endl;
                std::cout << "     Original: " << sug.original_code << std::endl;
                std::cout << "     Suggestion: " << sug.suggestion << std::endl;
            }
        }
    }
    std::cout << "[NAL-9 Introspection] Self-reflection complete." << std::endl;
}
