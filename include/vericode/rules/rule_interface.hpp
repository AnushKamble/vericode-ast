#pragma once

#include "vericode/ast_visitor.hpp"
#include "vericode/cfg.hpp"
#include "vericode/diagnostic.hpp"
#include <vector>
#include <memory>

namespace vericode {

class IVerificationRule {
public:
    virtual ~IVerificationRule() = default;

    virtual std::string rule_id() const = 0;
    virtual std::string rule_name() const = 0;
    virtual std::string description() const = 0;

    // Executes the static rule analysis on the provided AST root and CFG
    virtual std::vector<Diagnostic> analyze(const std::shared_ptr<ASTNode>& root,
                                           const std::shared_ptr<ControlFlowGraph>& cfg,
                                           const std::string& raw_source) = 0;

    virtual std::vector<Diagnostic> analyze(const std::shared_ptr<ASTNode>& root,
                                           const std::string& raw_source) {
        return analyze(root, nullptr, raw_source);
    }
};

} // namespace vericode
