#pragma once

#include "vericode/rules/rule_interface.hpp"

namespace vericode {

// Rule: CWE-362 - Concurrent Execution using Shared Resource with Improper Synchronization
// Invariant: Non-atomic shared state mutated across multi-threaded worker bodies or async lambdas
// must be synchronized with std::mutex, std::lock_guard, or std::atomic.
class CWE362RaceConditionRule : public IVerificationRule {
public:
    std::string rule_id() const override { return "CWE-362"; }
    std::string rule_name() const override { return "Unsynchronized Concurrent Shared State Access"; }
    std::string description() const override {
        return "Detects concurrent execution bodies accessing shared state without proper mutex or atomic synchronization.";
    }

    std::vector<Diagnostic> analyze(const std::shared_ptr<ASTNode>& root,
                                   const std::string& raw_source) override;
};

} // namespace vericode
