#pragma once

#include "vericode/rules/rule_interface.hpp"
#include "vericode/diagnostic.hpp"
#include <vector>
#include <memory>
#include <string>

namespace vericode {

class VerifierEngine {
public:
    VerifierEngine();
    ~VerifierEngine() = default;

    // Rule management
    void register_rule(std::unique_ptr<IVerificationRule> rule);
    void load_default_rules();

    // Verification methods
    VerificationResult verify_source(const std::string& source_code, const std::string& file_identifier = "inline_source");
    VerificationResult verify_file(const std::string& file_path);

private:
    std::vector<std::unique_ptr<IVerificationRule>> rules_;
};

} // namespace vericode
