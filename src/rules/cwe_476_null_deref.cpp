#include "vericode/rules/cwe_476_null_deref.hpp"
#include <sstream>
#include <regex>

namespace vericode {

static std::string strip_comment(const std::string& str) {
    auto pos = str.find("//");
    return (pos != std::string::npos) ? str.substr(0, pos) : str;
}

static bool is_non_null_check(const std::string& cond, const std::string& ptr) {
    if (cond.find(ptr + " != nullptr") != std::string::npos ||
        cond.find(ptr + " != NULL") != std::string::npos ||
        cond.find(ptr + " != 0") != std::string::npos ||
        cond.find("nullptr != " + ptr) != std::string::npos ||
        cond.find("NULL != " + ptr) != std::string::npos ||
        cond.find("0 != " + ptr) != std::string::npos) {
        return true;
    }
    std::regex simple_ptr(R"(^\s*)" + ptr + R"(\s*$)");
    if (std::regex_match(cond, simple_ptr)) {
        return true;
    }
    return false;
}

static bool is_null_check(const std::string& cond, const std::string& ptr) {
    if (cond.find(ptr + " == nullptr") != std::string::npos ||
        cond.find(ptr + " == NULL") != std::string::npos ||
        cond.find(ptr + " == 0") != std::string::npos ||
        cond.find("nullptr == " + ptr) != std::string::npos ||
        cond.find("NULL == " + ptr) != std::string::npos ||
        cond.find("0 == " + ptr) != std::string::npos ||
        cond.find("!" + ptr) != std::string::npos) {
        return true;
    }
    return false;
}

std::vector<Diagnostic> CWE476NullDerefRule::analyze(const std::shared_ptr<ASTNode>& root,
                                                    const std::shared_ptr<ControlFlowGraph>& cfg,
                                                    const std::string& raw_source) {
    std::vector<Diagnostic> diagnostics;

    // Check if modern RAII or smart pointers are used
    if (raw_source.find("std::unique_ptr") != std::string::npos ||
        raw_source.find("std::make_unique") != std::string::npos ||
        raw_source.find("std::shared_ptr") != std::string::npos) {
        return diagnostics;
    }

    std::istringstream stream(raw_source);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    std::regex arrow_regex(R"(([a-zA-Z_]\w*)\s*->\s*([a-zA-Z_]\w*))");

    // Extract all if-guards with conditions and block IDs
    struct GuardInfo {
        size_t cond_line;
        std::string condition;
        size_t cond_block_id;
        size_t then_block_id;
        bool then_exits;
        bool is_single_line_return;
    };
    std::vector<GuardInfo> guards;

    for (size_t i = 0; i < lines.size(); ++i) {
        size_t line_num = i + 1;
        std::string clean = strip_comment(lines[i]);

        auto if_pos = clean.find("if (");
        if (if_pos == std::string::npos) if_pos = clean.find("if(");

        if (if_pos != std::string::npos && cfg) {
            auto op_paren = clean.find('(', if_pos);
            if (op_paren != std::string::npos) {
                int p_depth = 1;
                size_t p_cur = op_paren + 1;
                while (p_cur < clean.length() && p_depth > 0) {
                    if (clean[p_cur] == '(') p_depth++;
                    else if (clean[p_cur] == ')') p_depth--;
                    p_cur++;
                }
                if (p_depth == 0) {
                    std::string cond = clean.substr(op_paren + 1, (p_cur - 1) - (op_paren + 1));
                    const auto* cond_blk = cfg->get_block_for_line(line_num);
                    if (cond_blk) {
                        bool single_ret = (clean.find("return") != std::string::npos && clean.find("return") > if_pos);
                        if (single_ret) {
                            guards.push_back({line_num, cond, cond_blk->id, cond_blk->id, true, true});
                        } else if (!cond_blk->successors.empty()) {
                            size_t then_b_id = cond_blk->successors[0];
                            const auto* then_blk = cfg->get_block(then_b_id);
                            bool exits = then_blk ? then_blk->is_exit : false;
                            guards.push_back({line_num, cond, cond_blk->id, then_b_id, exits, false});
                        }
                    }
                }
            }
        }
    }

    // Check each pointer dereference site
    for (size_t i = 0; i < lines.size(); ++i) {
        size_t line_num = i + 1;
        const auto& cur_line = lines[i];
        std::string clean_cur = strip_comment(cur_line);

        auto arrow_begin = std::sregex_iterator(clean_cur.begin(), clean_cur.end(), arrow_regex);
        auto arrow_end = std::sregex_iterator();

        for (std::sregex_iterator iter = arrow_begin; iter != arrow_end; ++iter) {
            std::smatch match = *iter;
            std::string ptr_name = match[1].str();

            if (ptr_name != "this" && ptr_name != "std") {
                const auto* deref_blk = cfg ? cfg->get_block_for_line(line_num) : nullptr;
                size_t deref_b_id = deref_blk ? deref_blk->id : 1;

                bool has_guard = false;

                if (cfg) {
                    for (const auto& g : guards) {
                        // Pattern 1: Non-null condition (e.g. if (ptr != nullptr))
                        // The dereference is safe if dominated by the then-branch block
                        if (is_non_null_check(g.condition, ptr_name) && !g.is_single_line_return) {
                            if (cfg->dominates(g.then_block_id, deref_b_id)) {
                                has_guard = true;
                                break;
                            }
                        }

                        // Pattern 2: Null check with early exit (e.g. if (!ptr) return;)
                        // The dereference is safe if dominated by cond block and not inside the exit branch
                        if (is_null_check(g.condition, ptr_name) && g.then_exits) {
                            if (g.is_single_line_return) {
                                if (cfg->dominates(g.cond_block_id, deref_b_id)) {
                                    has_guard = true;
                                    break;
                                }
                            } else {
                                if (cfg->dominates(g.cond_block_id, deref_b_id) && deref_b_id != g.then_block_id) {
                                    has_guard = true;
                                    break;
                                }
                            }
                        }
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
