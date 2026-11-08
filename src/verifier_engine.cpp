#include "vericode/verifier_engine.hpp"
#include "vericode/rules/cwe_401_memory_leak.hpp"
#include "vericode/rules/cwe_362_race_condition.hpp"
#include "vericode/rules/cwe_476_null_deref.hpp"
#include <chrono>
#include <fstream>
#include <sstream>

namespace vericode {

VerifierEngine::VerifierEngine() {
    load_default_rules();
}

void VerifierEngine::register_rule(std::unique_ptr<IVerificationRule> rule) {
    if (rule) {
        rules_.push_back(std::move(rule));
    }
}

void VerifierEngine::load_default_rules() {
    rules_.clear();
    register_rule(std::make_unique<CWE401MemoryLeakRule>());
    register_rule(std::make_unique<CWE362RaceConditionRule>());
    register_rule(std::make_unique<CWE476NullDerefRule>());
}

VerificationResult VerifierEngine::verify_source(const std::string& source_code, const std::string& file_identifier) {
    auto start_time = std::chrono::high_resolution_clock::now();

    VerificationResult result;
    result.file_path = file_identifier;
    result.passed = true;

    // Count lines of code
    size_t lines = 0;
    std::istringstream stream(source_code);
    std::string line;
    while (std::getline(stream, line)) {
        lines++;
    }
    result.total_lines_analyzed = lines;

    // Build AST
    auto ast_root = ASTBuilder::parse_source(source_code);

    // Execute registered rules
    for (const auto& rule : rules_) {
        auto rule_diagnostics = rule->analyze(ast_root, source_code);
        if (!rule_diagnostics.empty()) {
            result.passed = false;
            result.diagnostics.insert(result.diagnostics.end(), rule_diagnostics.begin(), rule_diagnostics.end());
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    result.analysis_time_microseconds = std::chrono::duration<double, std::micro>(end_time - start_time).count();

    return result;
}

VerificationResult VerifierEngine::verify_file(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        VerificationResult err_result;
        err_result.file_path = file_path;
        err_result.passed = false;
        err_result.total_lines_analyzed = 0;
        err_result.analysis_time_microseconds = 0;

        Diagnostic err_diag;
        err_diag.rule_id = "IO-ERROR";
        err_diag.rule_name = "File Read Failure";
        err_diag.severity = Severity::CRITICAL;
        err_diag.line_start = 0;
        err_diag.message = "Unable to open target source file for static analysis: " + file_path;
        err_result.diagnostics.push_back(err_diag);
        return err_result;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return verify_source(buffer.str(), file_path);
}

} // namespace vericode
