#include "vericode/rules/cwe_401_memory_leak.hpp"
#include <sstream>
#include <regex>

namespace vericode {

std::vector<Diagnostic> CWE401MemoryLeakRule::analyze(const std::shared_ptr<ASTNode>& root,
                                                    const std::shared_ptr<ControlFlowGraph>& cfg,
                                                    const std::string& raw_source) {
    std::vector<Diagnostic> diagnostics;
    std::istringstream stream(raw_source);
    std::string line;
    size_t line_num = 0;

    // Check if modern RAII headers/types are used
    bool has_raii_wrapper = (raw_source.find("std::unique_ptr") != std::string::npos) ||
                            (raw_source.find("std::make_unique") != std::string::npos) ||
                            (raw_source.find("std::shared_ptr") != std::string::npos);

    if (has_raii_wrapper) {
        return diagnostics;
    }

    struct AllocationInfo {
        size_t line;
        std::string snippet;
        size_t block_id;
    };

    std::vector<AllocationInfo> raw_alloc_sites;
    std::vector<size_t> dealloc_block_ids;
    size_t alloc_count = 0;
    size_t dealloc_count = 0;

    while (std::getline(stream, line)) {
        line_num++;
        auto comment_pos = line.find("//");
        std::string clean_line = (comment_pos != std::string::npos) ? line.substr(0, comment_pos) : line;

        // Detect raw malloc/calloc
        if (clean_line.find("malloc(") != std::string::npos || clean_line.find("calloc(") != std::string::npos) {
            alloc_count++;
            const auto* blk = cfg ? cfg->get_block_for_line(line_num) : nullptr;
            size_t b_id = blk ? blk->id : 1;
            raw_alloc_sites.push_back({line_num, line, b_id});
        }

        // Detect raw new (excluding placement new or smart pointer make_unique)
        if (clean_line.find("new ") != std::string::npos && clean_line.find("make_unique") == std::string::npos) {
            alloc_count++;
            const auto* blk = cfg ? cfg->get_block_for_line(line_num) : nullptr;
            size_t b_id = blk ? blk->id : 1;
            raw_alloc_sites.push_back({line_num, line, b_id});
        }

        // Detect deallocations
        if (clean_line.find("free(") != std::string::npos || clean_line.find("delete ") != std::string::npos || clean_line.find("delete[]") != std::string::npos) {
            dealloc_count++;
            const auto* blk = cfg ? cfg->get_block_for_line(line_num) : nullptr;
            if (blk) {
                dealloc_block_ids.push_back(blk->id);
            }
        }
    }

    // Invariant check: deallocation must post-dominate allocation along all execution paths
    for (const auto& alloc : raw_alloc_sites) {
        bool post_dominated = false;
        if (cfg && !dealloc_block_ids.empty()) {
            for (size_t dealloc_b_id : dealloc_block_ids) {
                if (cfg->post_dominates(dealloc_b_id, alloc.block_id)) {
                    post_dominated = true;
                    break;
                }
            }
        }

        if (!post_dominated || alloc_count > dealloc_count) {
            Diagnostic diag;
            diag.rule_id = rule_id();
            diag.rule_name = rule_name();
            diag.severity = Severity::ERROR;
            diag.line_start = alloc.line;
            diag.col_start = 1;
            diag.line_end = alloc.line;
            diag.col_end = alloc.snippet.length();
            diag.node_type = "raw_heap_allocation";
            diag.context_snippet = alloc.snippet;
            diag.message = "Raw heap memory allocation detected without guaranteed deterministic deallocation on all exit paths.";
            diag.repair_hint = "Refactor raw heap pointer to modern C++ RAII: replace raw pointer and 'new/malloc' with 'std::unique_ptr<T>' and 'std::make_unique<T>()' to ensure exception-safe automatic deallocation.";
            diagnostics.push_back(diag);
        }
    }

    return diagnostics;
}

} // namespace vericode
