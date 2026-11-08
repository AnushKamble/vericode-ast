#include "vericode/rules/cwe_401_memory_leak.hpp"
#include <sstream>
#include <regex>

namespace vericode {

std::vector<Diagnostic> CWE401MemoryLeakRule::analyze(const std::shared_ptr<ASTNode>& /*root*/,
                                                    const std::string& raw_source) {
    std::vector<Diagnostic> diagnostics;
    std::istringstream stream(raw_source);
    std::string line;
    size_t line_num = 0;

    // Check if modern RAII headers/types are used
    bool has_raii_wrapper = (raw_source.find("std::unique_ptr") != std::string::npos) ||
                            (raw_source.find("std::make_unique") != std::string::npos) ||
                            (raw_source.find("std::shared_ptr") != std::string::npos);

    // Track allocations vs deallocations
    size_t alloc_count = 0;
    size_t dealloc_count = 0;

    std::vector<std::pair<size_t, std::string>> raw_alloc_lines;

    while (std::getline(stream, line)) {
        line_num++;
        auto comment_pos = line.find("//");
        std::string clean_line = (comment_pos != std::string::npos) ? line.substr(0, comment_pos) : line;

        // Detect raw malloc/calloc
        if (clean_line.find("malloc(") != std::string::npos || clean_line.find("calloc(") != std::string::npos) {
            alloc_count++;
            raw_alloc_lines.push_back({line_num, line});
        }

        // Detect raw new (excluding placement new or smart pointer make_unique)
        if (clean_line.find("new ") != std::string::npos && clean_line.find("make_unique") == std::string::npos) {
            alloc_count++;
            raw_alloc_lines.push_back({line_num, line});
        }

        // Detect deallocations
        if (clean_line.find("free(") != std::string::npos || clean_line.find("delete ") != std::string::npos || clean_line.find("delete[]") != std::string::npos) {
            dealloc_count++;
        }
    }

    // If raw allocations exist without RAII or with unmatched deallocations
    if (alloc_count > dealloc_count && !has_raii_wrapper) {
        for (const auto& alloc : raw_alloc_lines) {
            Diagnostic diag;
            diag.rule_id = rule_id();
            diag.rule_name = rule_name();
            diag.severity = Severity::ERROR;
            diag.line_start = alloc.first;
            diag.col_start = 1;
            diag.line_end = alloc.first;
            diag.col_end = alloc.second.length();
            diag.node_type = "raw_heap_allocation";
            diag.context_snippet = alloc.second;
            diag.message = "Raw heap memory allocation detected without guaranteed deterministic deallocation on all exit paths.";
            diag.repair_hint = "Refactor raw heap pointer to modern C++ RAII: replace raw pointer and 'new/malloc' with 'std::unique_ptr<T>' and 'std::make_unique<T>()' to ensure exception-safe automatic deallocation.";
            diagnostics.push_back(diag);
        }
    }

    return diagnostics;
}

} // namespace vericode
