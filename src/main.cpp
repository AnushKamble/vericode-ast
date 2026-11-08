#include "vericode/verifier_engine.hpp"
#include <iostream>
#include <string>
#include <vector>

void print_usage(const char* prog_name) {
    std::cout << "VeriCode-AST Static Analysis & Invariant Verifier (v1.0.0)\n";
    std::cout << "Usage: " << prog_name << " [options] <target_file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --json         Output diagnostic report strictly in structured JSON format\n";
    std::cout << "  --help, -h     Show this help message\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    bool output_json = false;
    std::string target_file;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--json") {
            output_json = true;
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else if (target_file.empty()) {
            target_file = arg;
        }
    }

    if (target_file.empty()) {
        std::cerr << "Error: No input file specified.\n";
        return 1;
    }

    vericode::VerifierEngine engine;
    auto result = engine.verify_file(target_file);

    if (output_json) {
        std::cout << result.to_json() << std::endl;
    } else {
        std::cout << "========================================================\n";
        std::cout << "VeriCode-AST Invariant Verification Report\n";
        std::cout << "Target File    : " << result.file_path << "\n";
        std::cout << "Lines Analyzed : " << result.total_lines_analyzed << "\n";
        std::cout << "Latency (us)   : " << result.analysis_time_microseconds << " us\n";
        std::cout << "Status         : " << (result.passed ? "[PASSED] - NO DEFECTS" : "[FAILED] - VIOLATIONS DETECTED") << "\n";
        std::cout << "========================================================\n";

        if (!result.passed) {
            std::cout << "\nFound " << result.diagnostics.size() << " invariant violation(s):\n";
            for (size_t i = 0; i < result.diagnostics.size(); ++i) {
                const auto& d = result.diagnostics[i];
                std::cout << "\n[" << (i + 1) << "] " << d.rule_id << ": " << d.rule_name << "\n";
                std::cout << "    Location : Line " << d.line_start << ", Col " << d.col_start << "\n";
                std::cout << "    Snippet  : " << d.context_snippet << "\n";
                std::cout << "    Message  : " << d.message << "\n";
                std::cout << "    Remedy   : " << d.repair_hint << "\n";
            }
        }
    }

    return result.passed ? 0 : 2;
}
