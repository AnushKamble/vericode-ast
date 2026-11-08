#pragma once

#include "vericode/rules/rule_interface.hpp"

namespace vericode {

// Rule: CWE-476 - NULL Pointer Dereference
// Invariant: Raw pointer parameters or function returns that can be null must have
// an explicit branch guard check before pointer member dereference (->) or value dereference (*).
class CWE476NullDerefRule : public IVerificationRule {
public:
    std::string rule_id() const override { return "CWE-476"; }
    std::string rule_name() const override { return "Unchecked Pointer Dereference"; }
    std::string description() const override {
        return "Detects raw pointer dereferences lacking prior null-check guard conditionals.";
    }

    std::vector<Diagnostic> analyze(const std::shared_ptr<ASTNode>& root,
                                   const std::string& raw_source) override;
};

} // namespace vericode
