#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_set>

namespace vericode {

struct BasicBlock {
    size_t id{0};
    size_t start_line{0};
    size_t end_line{0};
    std::vector<std::string> statements;
    std::vector<size_t> successors;
    std::vector<size_t> predecessors;
    bool is_entry{false};
    bool is_exit{false};
};

class ControlFlowGraph {
public:
    ControlFlowGraph() = default;

    void add_block(const BasicBlock& block);
    void add_edge(size_t from_id, size_t to_id);
    
    const BasicBlock* get_block(size_t id) const;
    const std::vector<BasicBlock>& get_blocks() const { return blocks_; }

    // Depth-First Path Analysis: verifies whether all paths from entry to exit
    // satisfy a given invariant predicate before reaching an exit block.
    bool all_paths_satisfy(std::function<bool(const std::vector<size_t>& path, const ControlFlowGraph& cfg)> path_predicate) const;

    // Dominance analysis: checks if block 'dominator_id' dominates block 'target_id'
    bool dominates(size_t dominator_id, size_t target_id) const;

    // Factory method to construct CFG from tokenized source lines
    static ControlFlowGraph build_from_source(const std::string& source_code);

private:
    std::vector<BasicBlock> blocks_;
    void find_paths_dfs(size_t current_id, 
                        std::vector<size_t>& current_path, 
                        std::vector<std::vector<size_t>>& all_paths,
                        std::unordered_set<size_t>& visited) const;
};

} // namespace vericode
