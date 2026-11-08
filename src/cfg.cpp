#include "vericode/cfg.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

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
                // Detected loop, close path at back-edge
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

    std::vector<std::vector<size_t>> all_paths;
    std::vector<size_t> current_path;
    std::unordered_set<size_t> visited;

    find_paths_dfs(blocks_.front().id, current_path, all_paths, visited);

    for (const auto& path : all_paths) {
        auto target_it = std::find(path.begin(), path.end(), target_id);
        if (target_it != path.end()) {
            auto dom_it = std::find(path.begin(), target_it, dominator_id);
            if (dom_it == target_it) {
                // Target reached without passing through dominator
                return false;
            }
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
    ControlFlowGraph cfg;
    std::istringstream stream(source_code);
    std::string line;
    size_t line_num = 0;

    BasicBlock cur_block;
    cur_block.id = 1;
    cur_block.start_line = 1;
    cur_block.is_entry = true;

    size_t block_counter = 1;

    while (std::getline(stream, line)) {
        line_num++;
        cur_block.statements.push_back(line);

        std::string clean = mask_comments_and_strings(line);

        // Control flow branch tokens trigger new basic blocks
        bool is_branch = (clean.find("if (") != std::string::npos) ||
                         (clean.find("else") != std::string::npos) ||
                         (clean.find("while (") != std::string::npos) ||
                         (clean.find("for (") != std::string::npos) ||
                         (clean.find("return") != std::string::npos) ||
                         (clean.find("}") != std::string::npos);

        if (is_branch && !cur_block.statements.empty()) {
            cur_block.end_line = line_num;
            if (clean.find("return") != std::string::npos) {
                cur_block.is_exit = true;
            }
            cfg.add_block(cur_block);

            // Connect sequential edge
            size_t next_id = ++block_counter;
            cfg.add_edge(cur_block.id, next_id);

            cur_block = BasicBlock();
            cur_block.id = next_id;
            cur_block.start_line = line_num + 1;
        }
    }

    if (!cur_block.statements.empty()) {
        cur_block.end_line = line_num;
        cur_block.is_exit = true;
        cfg.add_block(cur_block);
    }

    return cfg;
}

} // namespace vericode
