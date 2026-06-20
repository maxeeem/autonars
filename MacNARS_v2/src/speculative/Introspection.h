#pragma once
#include <string>
#include <vector>

struct OptimizationSuggestion {
    std::string file_path;
    int line_number;
    std::string suggestion;
    std::string original_code;
};

class Introspection {
public:
    // NAL-9 Mental Operation: Read source code
    static std::string load_file(const std::string& path);
    // NAL-9 Mental Operation: Evaluate and reflect on code logic
    static std::vector<OptimizationSuggestion> evaluate_file(const std::string& path);
    // Main entry point for NAL-9 Introspection proof of concept
    static void run_introspection(const std::string& src_directory);
};
