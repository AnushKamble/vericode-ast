#pragma once

#include "vericode/rules/rule_interface.hpp"

namespace vericode {

// Rule: CWE-401 - Improper Release of Memory Before Removing Last Reference
// Invariant: Any raw heap allocation (new, malloc) must have a deterministic deallocation
// (delete, free) in all executing paths, or be replaced by RAII types (std::unique_ptr, std::shared_ptr).
class CWE401MemoryLeakRule : public IVerificationRule {
public:
    std::string rule_id() const override { return "CWE-401"; }
    std::string rule_name() const override { return "Non-RAII Raw Heap Allocation / Potential Memory Leak"; }
    std::string description() const override {
        return "Detects unchecked raw pointer heap allocations lacking deterministic RAII lifecycle management.";
    }

    std::vector<Diagnostic> analyze(const std::shared_ptr<ASTNode>& root,
                                   const std::string& raw_source) override;
};

} // namespace vericode
