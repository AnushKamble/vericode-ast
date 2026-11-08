#pragma once

#include <string>
#include <vector>

namespace vericode {

enum class Severity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

struct Diagnostic {
    std::string rule_id;       // e.g., "CWE-401"
    std::string rule_name;     // e.g., "Non-RAII Raw Heap Allocation / Potential Memory Leak"
    Severity severity;
    size_t line_start;
    size_t col_start;
    size_t line_end;
    size_t col_end;
    std::string node_type;     // AST node identifier, e.g., "call_expression"
    std::string message;       // Detailed human-readable defect report
    std::string context_snippet; // Extracted source line
    std::string repair_hint;   // Concrete formal guidance for LLM self-repair prompt

    std::string to_json() const;
};

struct VerificationResult {
    bool passed;
    std::string file_path;
    size_t total_lines_analyzed;
    double analysis_time_microseconds;
    std::vector<Diagnostic> diagnostics;

    std::string to_json() const;
};

} // namespace vericode
