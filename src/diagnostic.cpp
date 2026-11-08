#include "vericode/diagnostic.hpp"
#include <sstream>
#include <iomanip>

namespace vericode {

static std::string escape_json(const std::string& input) {
    std::ostringstream ss;
    for (char c : input) {
        switch (c) {
            case '"':  ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:
                if ('\x00' <= c && c <= '\x1f') {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    ss << c;
                }
        }
    }
    return ss.str();
}

static std::string severity_to_string(Severity s) {
    switch (s) {
        case Severity::INFO:     return "INFO";
        case Severity::WARNING:  return "WARNING";
        case Severity::ERROR:    return "ERROR";
        case Severity::CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";
}

std::string Diagnostic::to_json() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "    \"rule_id\": \"" << escape_json(rule_id) << "\",\n";
    ss << "    \"rule_name\": \"" << escape_json(rule_name) << "\",\n";
    ss << "    \"severity\": \"" << severity_to_string(severity) << "\",\n";
    ss << "    \"line_start\": " << line_start << ",\n";
    ss << "    \"col_start\": " << col_start << ",\n";
    ss << "    \"line_end\": " << line_end << ",\n";
    ss << "    \"col_end\": " << col_end << ",\n";
    ss << "    \"node_type\": \"" << escape_json(node_type) << "\",\n";
    ss << "    \"message\": \"" << escape_json(message) << "\",\n";
    ss << "    \"context_snippet\": \"" << escape_json(context_snippet) << "\",\n";
    ss << "    \"repair_hint\": \"" << escape_json(repair_hint) << "\"\n";
    ss << "  }";
    return ss.str();
}

std::string VerificationResult::to_json() const {
    std::ostringstream ss;
    ss << "{\n";
    ss << "  \"passed\": " << (passed ? "true" : "false") << ",\n";
    ss << "  \"file_path\": \"" << escape_json(file_path) << "\",\n";
    ss << "  \"total_lines_analyzed\": " << total_lines_analyzed << ",\n";
    ss << "  \"analysis_time_us\": " << analysis_time_microseconds << ",\n";
    ss << "  \"diagnostic_count\": " << diagnostics.size() << ",\n";
    ss << "  \"diagnostics\": [\n";
    for (size_t i = 0; i < diagnostics.size(); ++i) {
        ss << "    " << diagnostics[i].to_json();
        if (i + 1 < diagnostics.size()) {
            ss << ",";
        }
        ss << "\n";
    }
    ss << "  ]\n";
    ss << "}\n";
    return ss.str();
}

} // namespace vericode
