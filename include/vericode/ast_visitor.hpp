#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace vericode {

enum class ASTNodeType {
    TRANSLATION_UNIT,
    FUNCTION_DECLARATION,
    VARIABLE_DECLARATION,
    CALL_EXPRESSION,
    BINARY_EXPRESSION,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    RETURN_STATEMENT,
    MEMBER_ACCESS,
    IDENTIFIER,
    LITERAL,
    BLOCK,
    UNKNOWN
};

struct SourceLocation {
    size_t line;
    size_t column;
};

struct ASTNode {
    ASTNodeType type;
    std::string value;
    SourceLocation location_start;
    SourceLocation location_end;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::weak_ptr<ASTNode> parent;

    // Helper utilities
    bool is_type(ASTNodeType t) const { return type == t; }
    void add_child(std::shared_ptr<ASTNode> child);
};

// Lightweight AST parser & builder interface
class ASTBuilder {
public:
    static std::shared_ptr<ASTNode> parse_source(const std::string& source_code);
    static std::string node_type_to_string(ASTNodeType type);
};

} // namespace vericode
