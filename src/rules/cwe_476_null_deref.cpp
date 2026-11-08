#include "vericode/rules/cwe_476_null_deref.hpp"
#include <sstream>
#include <regex>

namespace vericode {

std::vector<Diagnostic> CWE476NullDerefRule::analyze(const std::shared_ptr<ASTNode>& /*root*/,
                                                    const std::string& raw_source) {
    std::vector<Diagnostic> diagnostics;
    std::istringstream stream(raw_source);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    std::regex arrow_regex(R"(([a-zA-Z_]\w*)\s*->\s*([a-zA-Z_]\w*))");

    auto strip_comment = [](const std::string& str) -> std::string {
        auto pos = str.find("//");
        return (pos != std::string::npos) ? str.substr(0, pos) : str;
    };

    for (size_t i = 0; i < lines.size(); ++i) {
        size_t line_num = i + 1;
        const auto& cur_line = lines[i];
        std::string clean_cur = strip_comment(cur_line);
        std::smatch match;
        if (std::regex_search(clean_cur, match, arrow_regex)) {
            std::string ptr_name = match[1].str();
            
            if (ptr_name != "this" && ptr_name != "std") {
                // Check antecedent lines for a null guard on ptr_name
                bool has_guard = false;
                for (size_t j = 0; j < i; ++j) {
                    std::string clean_prev = strip_comment(lines[j]);
                    if (clean_prev.find(ptr_name + " != nullptr") != std::string::npos ||
                        clean_prev.find(ptr_name + " != NULL") != std::string::npos ||
                        clean_prev.find("!" + ptr_name) != std::string::npos ||
                        clean_prev.find("if (" + ptr_name + ")") != std::string::npos ||
                        clean_prev.find("if (" + ptr_name + " !=") != std::string::npos) {
                        has_guard = true;
                        break;
                    }
                }

                if (!has_guard) {
                    Diagnostic diag;
                    diag.rule_id = rule_id();
                    diag.rule_name = rule_name();
                    diag.severity = Severity::WARNING;
                    diag.line_start = line_num;
                    diag.col_start = static_cast<size_t>(match.position(0));
                    diag.line_end = line_num;
                    diag.col_end = static_cast<size_t>(match.position(0) + match[0].length());
                    diag.node_type = "member_access_arrow";
                    diag.context_snippet = cur_line;
                    diag.message = "Raw pointer '" + ptr_name + "' dereferenced without prior null-check guard statement.";
                    diag.repair_hint = "Add defensive null-check guard: 'if (!" + ptr_name + ") { return ...; }' or wrap pointer access within an explicit condition before dereferencing.";
                    diagnostics.push_back(diag);
                }
            }
        }
    }

    return diagnostics;
}

} // namespace vericode
