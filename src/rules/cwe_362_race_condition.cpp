#include "vericode/rules/cwe_362_race_condition.hpp"
#include <sstream>
#include <regex>

namespace vericode {

std::vector<Diagnostic> CWE362RaceConditionRule::analyze(const std::shared_ptr<ASTNode>& /*root*/,
                                                        const std::string& raw_source) {
    std::vector<Diagnostic> diagnostics;

    // Check if multithreading is used
    bool has_threads = (raw_source.find("std::thread") != std::string::npos) ||
                       (raw_source.find("std::async") != std::string::npos) ||
                       (raw_source.find("pthread_create") != std::string::npos);

    if (!has_threads) {
        return diagnostics;
    }

    // Check if proper synchronization primitives are present
    bool has_sync = (raw_source.find("std::mutex") != std::string::npos) ||
                    (raw_source.find("std::lock_guard") != std::string::npos) ||
                    (raw_source.find("std::unique_lock") != std::string::npos) ||
                    (raw_source.find("std::atomic") != std::string::npos);

    if (!has_sync) {
        std::istringstream stream(raw_source);
        std::string line;
        size_t line_num = 0;

        while (std::getline(stream, line)) {
            line_num++;
            auto pos = line.find("//");
            std::string clean = (pos != std::string::npos) ? line.substr(0, pos) : line;
            if (clean.find("std::thread") != std::string::npos || clean.find("pthread_create") != std::string::npos) {
                Diagnostic diag;
                diag.rule_id = rule_id();
                diag.rule_name = rule_name();
                diag.severity = Severity::CRITICAL;
                diag.line_start = line_num;
                diag.col_start = 1;
                diag.line_end = line_num;
                diag.col_end = line.length();
                diag.node_type = "concurrent_execution_dispatch";
                diag.context_snippet = line;
                diag.message = "Multi-threaded worker dispatched without detected synchronization primitives (std::mutex or std::atomic). Shared mutations cause undefined behavior and data races.";
                diag.repair_hint = "Protect shared mutable state using 'std::mutex' and 'std::lock_guard<std::mutex> lock(mtx);' or convert shared variables to 'std::atomic<T>' to guarantee thread safety.";
                diagnostics.push_back(diag);
            }
        }
    }

    return diagnostics;
}

} // namespace vericode
