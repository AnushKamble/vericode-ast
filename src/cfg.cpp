#include "vericode/cfg.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <regex>

namespace vericode {

void ControlFlowGraph::add_block(const BasicBlock& block) {
    blocks_.push_back(block);
}

void ControlFlowGraph::add_edge(size_t from_id, size_t to_id) {
    for (auto& b : blocks_) {
        if (b.id == from_id) {
            if (std::find(b.successors.begin(), b.successors.end(), to_id) == b.successors.end()) {
                b.successors.push_back(to_id);
            }
        }
        if (b.id == to_id) {
            if (std::find(b.predecessors.begin(), b.predecessors.end(), from_id) == b.predecessors.end()) {
                b.predecessors.push_back(from_id);
            }
        }
    }
}

const BasicBlock* ControlFlowGraph::get_block(size_t id) const {
    for (const auto& b : blocks_) {
        if (b.id == id) return &b;
    }
    return nullptr;
}

const BasicBlock* ControlFlowGraph::get_block_for_line(size_t line) const {
    for (const auto& b : blocks_) {
        if (line >= b.start_line && line <= b.end_line) {
            return &b;
        }
    }
    const BasicBlock* best = nullptr;
    for (const auto& b : blocks_) {
        if (b.start_line <= line) {
            if (!best || b.start_line > best->start_line) {
                best = &b;
            }
        }
    }
    if (best) return best;
    return blocks_.empty() ? nullptr : &blocks_.front();
}

void ControlFlowGraph::find_paths_dfs(size_t current_id, 
                                     std::vector<size_t>& current_path, 
                                     std::vector<std::vector<size_t>>& all_paths,
                                     std::unordered_set<size_t>& visited) const {
    current_path.push_back(current_id);
    visited.insert(current_id);

    const auto* blk = get_block(current_id);
    if (!blk || blk->is_exit || blk->successors.empty()) {
        all_paths.push_back(current_path);
    } else {
        for (size_t next_id : blk->successors) {
            if (visited.find(next_id) == visited.end()) {
                find_paths_dfs(next_id, current_path, all_paths, visited);
            } else {
                // Loop back-edge encountered, record closed path
                all_paths.push_back(current_path);
            }
        }
    }

    visited.erase(current_id);
    current_path.pop_back();
}

bool ControlFlowGraph::all_paths_satisfy(std::function<bool(const std::vector<size_t>&, const ControlFlowGraph&)> path_predicate) const {
    if (blocks_.empty()) return true;

    std::vector<std::vector<size_t>> all_paths;
    std::vector<size_t> current_path;
    std::unordered_set<size_t> visited;

    find_paths_dfs(blocks_.front().id, current_path, all_paths, visited);

    for (const auto& path : all_paths) {
        if (!path_predicate(path, *this)) {
            return false;
        }
    }
    return true;
}

bool ControlFlowGraph::dominates(size_t dominator_id, size_t target_id) const {
    if (dominator_id == target_id) return true;
    if (blocks_.empty()) return false;

    // Determine the entry block for target_id by tracing predecessors backwards
    std::unordered_set<size_t> visited_back;
    std::vector<size_t> q = {target_id};
    visited_back.insert(target_id);
    size_t entry_id = blocks_.front().id;

    while (!q.empty()) {
        size_t cur = q.back();
        q.pop_back();
        const auto* blk = get_block(cur);
        if (!blk) continue;
        if (blk->is_entry || blk->predecessors.empty()) {
            entry_id = cur;
            break;
        }
        for (size_t pred : blk->predecessors) {
            if (visited_back.find(pred) == visited_back.end()) {
                visited_back.insert(pred);
                q.push_back(pred);
            }
        }
    }

    std::vector<std::vector<size_t>> all_paths;
    std::vector<size_t> current_path;
    std::unordered_set<size_t> visited;

    find_paths_dfs(entry_id, current_path, all_paths, visited);

    bool reached_target = false;
    for (const auto& path : all_paths) {
        auto target_it = std::find(path.begin(), path.end(), target_id);
        if (target_it != path.end()) {
            reached_target = true;
            auto dom_it = std::find(path.begin(), target_it, dominator_id);
            if (dom_it == target_it) {
                // Target reached on a path that bypassed dominator
                return false;
            }
        }
    }
    return reached_target;
}

bool ControlFlowGraph::post_dominates(size_t post_dominator_id, size_t target_id) const {
    if (post_dominator_id == target_id) return true;
    if (blocks_.empty()) return false;

    const auto* target_blk = get_block(target_id);
    if (!target_blk) return false;

    if (target_blk->is_exit) return false;

    std::vector<std::vector<size_t>> paths_from_target;
    std::vector<size_t> current_path;
    std::unordered_set<size_t> visited;

    find_paths_dfs(target_id, current_path, paths_from_target, visited);

    if (paths_from_target.empty()) return false;

    for (const auto& path : paths_from_target) {
        if (std::find(path.begin(), path.end(), post_dominator_id) == path.end()) {
            return false;
        }
    }
    return true;
}

static std::string mask_comments_and_strings(const std::string& input) {
    std::string result;
    bool in_string = false;
    for (size_t k = 0; k < input.size(); ++k) {
        if (!in_string && input[k] == '/' && k + 1 < input.size() && input[k + 1] == '/') {
            break; // rest of line is comment
        }
        if (input[k] == '"' && (k == 0 || input[k - 1] != '\\')) {
            in_string = !in_string;
            result += '"';
            continue;
        }
        if (!in_string) {
            result += input[k];
        }
    }
    return result;
}

ControlFlowGraph ControlFlowGraph::build_from_source(const std::string& source_code) {
    return build_from_ast(nullptr, source_code);
}

ControlFlowGraph ControlFlowGraph::build_from_ast(const std::shared_ptr<ASTNode>& ast_root, const std::string& source_code) {
    ControlFlowGraph cfg;
    std::istringstream stream(source_code);
    std::vector<std::string> lines;
    std::string l;
    while (std::getline(stream, l)) {
        lines.push_back(l);
    }
    if (lines.empty()) return cfg;

    size_t num_lines = lines.size();

    // Struct representing an extracted if-construct
    struct IfConstruct {
        size_t cond_line{0};
        std::string condition;
        size_t then_start{0};
        size_t then_end{0};
        bool is_single_line_return{false};
        bool then_has_return{false};
        bool has_else{false};
        size_t else_start{0};
        size_t else_end{0};
        bool else_has_return{false};
        size_t join_line{0};
    };

    std::vector<IfConstruct> if_constructs;
    std::vector<size_t> standalone_return_lines;
    std::unordered_set<size_t> func_start_lines;

    // Leverage AST nodes when available
    if (ast_root) {
        auto fn_nodes = ast_root->find_descendants(ASTNodeType::FUNCTION_DECLARATION);
        for (const auto& fn : fn_nodes) {
            func_start_lines.insert(fn->location_start.line);
        }
        auto ret_nodes = ast_root->find_descendants(ASTNodeType::RETURN_STATEMENT);
        for (const auto& rn : ret_nodes) {
            standalone_return_lines.push_back(rn->location_start.line);
        }
    }

    // Detect function boundaries and standalone return statements
    static const std::regex fn_regex(R"((int|void|bool|auto|char|double|float|\w+\*?)\s+([a-zA-Z_]\w*)\s*\([^)]*\)\s*\{?)");
    for (size_t i = 0; i < num_lines; ++i) {
        size_t line_num = i + 1;
        std::string clean = mask_comments_and_strings(lines[i]);
        if (std::regex_search(clean, fn_regex)) {
            func_start_lines.insert(line_num);
        }
        if (clean.find("return") != std::string::npos && clean.find("if (") == std::string::npos && clean.find("if(") == std::string::npos) {
            standalone_return_lines.push_back(line_num);
        }
    }

    // Parse if constructs
    for (size_t i = 0; i < num_lines; ++i) {
        size_t line_num = i + 1;
        std::string clean = mask_comments_and_strings(lines[i]);

        auto if_pos = clean.find("if (");
        if (if_pos == std::string::npos) {
            if_pos = clean.find("if(");
        }
        if (if_pos != std::string::npos) {
            IfConstruct ic;
            ic.cond_line = line_num;

            // Extract condition inside parentheses
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
                    ic.condition = clean.substr(op_paren + 1, (p_cur - 1) - (op_paren + 1));
                }
            }

            // Check if this line is a single-line if-return: "if (...) return;"
            if (clean.find("return") != std::string::npos && clean.find("return") > if_pos) {
                ic.is_single_line_return = true;
                ic.then_has_return = true;
                ic.then_start = line_num;
                ic.then_end = line_num;
                ic.join_line = line_num + 1;
                if_constructs.push_back(ic);
                continue;
            }

            // Find block then-range
            size_t after_if_line = line_num + 1;
            bool found_brace = false;
            size_t brace_line_idx = i;

            for (size_t scan_idx = i; scan_idx < std::min(num_lines, i + 3); ++scan_idx) {
                std::string scan_clean = mask_comments_and_strings(lines[scan_idx]);
                auto b_pos = scan_clean.find('{');
                if (b_pos != std::string::npos) {
                    found_brace = true;
                    brace_line_idx = scan_idx;
                    break;
                }
            }

            if (found_brace) {
                int b_depth = 0;
                size_t end_idx = brace_line_idx;
                for (size_t scan_idx = brace_line_idx; scan_idx < num_lines; ++scan_idx) {
                    std::string scan_clean = mask_comments_and_strings(lines[scan_idx]);
                    for (char ch : scan_clean) {
                        if (ch == '{') b_depth++;
                        else if (ch == '}') b_depth--;
                    }
                    if (b_depth == 0) {
                        end_idx = scan_idx;
                        break;
                    }
                }
                std::string brace_clean = mask_comments_and_strings(lines[brace_line_idx]);
                auto b_pos = brace_clean.find('{');
                std::string after_brace = (b_pos != std::string::npos) ? brace_clean.substr(b_pos + 1) : "";
                bool has_stmts = after_brace.find_first_not_of(" \t\r\n") != std::string::npos;

                ic.then_start = has_stmts ? (brace_line_idx + 1) : (brace_line_idx + 2);
                ic.then_end = (end_idx > brace_line_idx) ? end_idx : (brace_line_idx + 1);
                if (ic.then_start > ic.then_end) ic.then_start = ic.then_end;
                after_if_line = end_idx + 2;
            } else {
                ic.then_start = line_num + 1;
                ic.then_end = line_num + 1;
                after_if_line = line_num + 2;
            }

            // Check if then-body contains return
            for (size_t k = (ic.then_start > 0 ? ic.then_start - 1 : 0); k < std::min(num_lines, ic.then_end); ++k) {
                std::string k_clean = mask_comments_and_strings(lines[k]);
                if (k_clean.find("return") != std::string::npos) {
                    ic.then_has_return = true;
                    break;
                }
            }

            // Check for else branch
            if (after_if_line <= num_lines) {
                size_t check_idx = after_if_line - 1;
                std::string check_else = mask_comments_and_strings(lines[check_idx]);
                if (check_else.find("else") != std::string::npos) {
                    ic.has_else = true;
                    bool else_brace = false;
                    size_t else_brace_idx = check_idx;
                    for (size_t s_idx = check_idx; s_idx < std::min(num_lines, check_idx + 3); ++s_idx) {
                        if (mask_comments_and_strings(lines[s_idx]).find('{') != std::string::npos) {
                            else_brace = true;
                            else_brace_idx = s_idx;
                            break;
                        }
                    }
                    if (else_brace) {
                        int e_depth = 0;
                        size_t e_end_idx = else_brace_idx;
                        for (size_t s_idx = else_brace_idx; s_idx < num_lines; ++s_idx) {
                            std::string s_clean = mask_comments_and_strings(lines[s_idx]);
                            for (char ch : s_clean) {
                                if (ch == '{') e_depth++;
                                else if (ch == '}') e_depth--;
                            }
                            if (e_depth == 0) {
                                e_end_idx = s_idx;
                                break;
                            }
                        }
                        std::string e_brace_clean = mask_comments_and_strings(lines[else_brace_idx]);
                        auto eb_pos = e_brace_clean.find('{');
                        std::string eb_after = (eb_pos != std::string::npos) ? e_brace_clean.substr(eb_pos + 1) : "";
                        bool eb_has_stmts = eb_after.find_first_not_of(" \t\r\n") != std::string::npos;

                        ic.else_start = eb_has_stmts ? (else_brace_idx + 1) : (else_brace_idx + 2);
                        ic.else_end = (e_end_idx > else_brace_idx) ? e_end_idx : (else_brace_idx + 1);
                        if (ic.else_start > ic.else_end) ic.else_start = ic.else_end;
                        after_if_line = e_end_idx + 2;
                    } else {
                        ic.else_start = check_idx + 1;
                        ic.else_end = check_idx + 1;
                        after_if_line = check_idx + 2;
                    }

                    for (size_t k = (ic.else_start > 0 ? ic.else_start - 1 : 0); k < std::min(num_lines, ic.else_end); ++k) {
                        if (mask_comments_and_strings(lines[k]).find("return") != std::string::npos) {
                            ic.else_has_return = true;
                            break;
                        }
                    }
                }
            }

            ic.join_line = after_if_line;
            if (ic.join_line > num_lines) ic.join_line = num_lines;
            if_constructs.push_back(ic);
        }
    }

    // Determine basic block line partitions
    std::unordered_set<size_t> block_starts;
    block_starts.insert(1);
    for (size_t f_l : func_start_lines) block_starts.insert(f_l);
    for (const auto& ic : if_constructs) {
        block_starts.insert(ic.cond_line);
        if (!ic.is_single_line_return) {
            block_starts.insert(ic.then_start);
            if (ic.has_else) block_starts.insert(ic.else_start);
        }
        if (ic.join_line <= num_lines) block_starts.insert(ic.join_line);
    }
    for (size_t ret_l : standalone_return_lines) {
        if (ret_l + 1 <= num_lines) block_starts.insert(ret_l + 1);
    }

    std::vector<size_t> sorted_starts(block_starts.begin(), block_starts.end());
    std::sort(sorted_starts.begin(), sorted_starts.end());

    // Build blocks
    size_t block_id_counter = 1;
    for (size_t idx = 0; idx < sorted_starts.size(); ++idx) {
        size_t s_line = sorted_starts[idx];
        size_t e_line = (idx + 1 < sorted_starts.size()) ? (sorted_starts[idx + 1] - 1) : num_lines;
        if (e_line < s_line) e_line = s_line;

        BasicBlock blk;
        blk.id = block_id_counter++;
        blk.start_line = s_line;
        blk.end_line = e_line;
        blk.is_entry = (s_line == 1 || func_start_lines.find(s_line) != func_start_lines.end());

        for (size_t l_idx = s_line - 1; l_idx < e_line && l_idx < num_lines; ++l_idx) {
            blk.statements.push_back(lines[l_idx]);
            std::string cl = mask_comments_and_strings(lines[l_idx]);
            // Standalone return marks block as exit
            if (cl.find("return") != std::string::npos && cl.find("if (") == std::string::npos && cl.find("if(") == std::string::npos) {
                blk.is_exit = true;
            }
        }
        if (e_line == num_lines) {
            blk.is_exit = true;
        }

        cfg.add_block(blk);
    }

    // Connect edges based on if-constructs and sequential flows
    std::unordered_set<size_t> cond_block_ids;
    for (const auto& ic : if_constructs) {
        const auto* cond_blk = cfg.get_block_for_line(ic.cond_line);
        const auto* join_blk = (ic.join_line <= num_lines) ? cfg.get_block_for_line(ic.join_line) : nullptr;

        if (cond_blk) {
            cond_block_ids.insert(cond_blk->id);

            if (ic.is_single_line_return) {
                // Single line if-return: cond block branches to join block on false
                // and has an early exit on true
                if (join_blk && join_blk->id != cond_blk->id) {
                    cfg.add_edge(cond_blk->id, join_blk->id);
                }
            } else {
                const auto* then_blk = cfg.get_block_for_line(ic.then_start);
                if (then_blk && cond_blk->id != then_blk->id) {
                    cfg.add_edge(cond_blk->id, then_blk->id);

                    if (!ic.then_has_return && join_blk && join_blk->id != then_blk->id) {
                        cfg.add_edge(then_blk->id, join_blk->id);
                    }

                    if (ic.has_else) {
                        const auto* else_blk = cfg.get_block_for_line(ic.else_start);
                        if (else_blk && cond_blk->id != else_blk->id) {
                            cfg.add_edge(cond_blk->id, else_blk->id);
                            if (!ic.else_has_return && join_blk && join_blk->id != else_blk->id) {
                                cfg.add_edge(else_blk->id, join_blk->id);
                            }
                        }
                    } else {
                        // False branch bypass edge to join block
                        if (join_blk && join_blk->id != cond_blk->id) {
                            cfg.add_edge(cond_blk->id, join_blk->id);
                        }
                    }
                }
            }
        }
    }

    // Connect normal sequential edges
    const auto& all_blks = cfg.get_blocks();
    for (size_t b_i = 0; b_i + 1 < all_blks.size(); ++b_i) {
        const auto& cur_b = all_blks[b_i];
        const auto& next_b = all_blks[b_i + 1];

        // Do not connect exit blocks or branch condition blocks (already wired)
        if (!cur_b.is_exit && cond_block_ids.find(cur_b.id) == cond_block_ids.end()) {
            if (cur_b.successors.empty()) {
                cfg.add_edge(cur_b.id, next_b.id);
            }
        }
    }

    return cfg;
}

} // namespace vericode
