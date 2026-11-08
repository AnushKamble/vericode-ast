#include "vericode/ast_visitor.hpp"
#include <sstream>
#include <regex>
#include <iostream>

namespace vericode {

void ASTNode::add_child(std::shared_ptr<ASTNode> child) {
    if (child) {
        children.push_back(child);
    }
}

std::string ASTBuilder::node_type_to_string(ASTNodeType type) {
    switch (type) {
        case ASTNodeType::TRANSLATION_UNIT:     return "translation_unit";
        case ASTNodeType::FUNCTION_DECLARATION: return "function_declaration";
        case ASTNodeType::VARIABLE_DECLARATION: return "variable_declaration";
        case ASTNodeType::CALL_EXPRESSION:     return "call_expression";
        case ASTNodeType::BINARY_EXPRESSION:   return "binary_expression";
        case ASTNodeType::IF_STATEMENT:         return "if_statement";
        case ASTNodeType::WHILE_STATEMENT:      return "while_statement";
        case ASTNodeType::FOR_STATEMENT:        return "for_statement";
        case ASTNodeType::RETURN_STATEMENT:     return "return_statement";
        case ASTNodeType::MEMBER_ACCESS:        return "member_access";
        case ASTNodeType::IDENTIFIER:           return "identifier";
        case ASTNodeType::LITERAL:              return "literal";
        case ASTNodeType::BLOCK:                return "compound_statement";
        case ASTNodeType::UNKNOWN:              return "unknown";
    }
    return "unknown";
}

// Lightweight AST parser scanning line-by-line semantic tokens
std::shared_ptr<ASTNode> ASTBuilder::parse_source(const std::string& source_code) {
    auto root = std::make_shared<ASTNode>();
    root->type = ASTNodeType::TRANSLATION_UNIT;
    root->value = "root";
    root->location_start = {1, 1};

    std::istringstream stream(source_code);
    std::string line;
    size_t line_num = 0;

    std::shared_ptr<ASTNode> current_function = nullptr;

    while (std::getline(stream, line)) {
        line_num++;
        
        // Strip comments
        auto comment_pos = line.find("//");
        std::string clean_line = (comment_pos != std::string::npos) ? line.substr(0, comment_pos) : line;

        // Check for function definitions
        std::regex fn_regex(R"((int|void|bool|auto|char|double|float|std::\w+|\w+\*?)\s+([a-zA-Z_]\w*)\s*\([^)]*\)\s*\{?)");
        std::smatch fn_match;
        if (std::regex_search(clean_line, fn_match, fn_regex)) {
            auto fn_node = std::make_shared<ASTNode>();
            fn_node->type = ASTNodeType::FUNCTION_DECLARATION;
            fn_node->value = fn_match[2].str();
            fn_node->location_start = {line_num, static_cast<size_t>(fn_match.position(2))};
            fn_node->location_end = {line_num, static_cast<size_t>(fn_match.position(2) + fn_match[2].length())};
            root->add_child(fn_node);
            current_function = fn_node;
        }

        auto active_scope = current_function ? current_function : root;

        // Check for function calls & allocations
        std::regex call_regex(R"(([a-zA-Z_]\w*)\s*\([^)]*\))");
        auto calls_begin = std::sregex_iterator(clean_line.begin(), clean_line.end(), call_regex);
        auto calls_end = std::sregex_iterator();
        for (std::sregex_iterator i = calls_begin; i != calls_end; ++i) {
            std::smatch match = *i;
            auto call_node = std::make_shared<ASTNode>();
            call_node->type = ASTNodeType::CALL_EXPRESSION;
            call_node->value = match[1].str();
            call_node->location_start = {line_num, static_cast<size_t>(match.position(1))};
            call_node->location_end = {line_num, static_cast<size_t>(match.position(1) + match[1].length())};
            active_scope->add_child(call_node);
        }

        // Check for 'new' allocations
        std::regex new_regex(R"(\bnew\s+([a-zA-Z_]\w*))");
        std::smatch new_match;
        if (std::regex_search(clean_line, new_match, new_regex)) {
            auto new_node = std::make_shared<ASTNode>();
            new_node->type = ASTNodeType::CALL_EXPRESSION;
            new_node->value = "operator_new";
            new_node->location_start = {line_num, static_cast<size_t>(new_match.position(0))};
            new_node->location_end = {line_num, static_cast<size_t>(new_match.position(0) + new_match[0].length())};
            active_scope->add_child(new_node);
        }

        // Check for member access (pointer dereference ->)
        std::regex arrow_regex(R"(([a-zA-Z_]\w*)\s*->\s*([a-zA-Z_]\w*))");
        auto arrow_begin = std::sregex_iterator(clean_line.begin(), clean_line.end(), arrow_regex);
        auto arrow_end = std::sregex_iterator();
        for (std::sregex_iterator i = arrow_begin; i != arrow_end; ++i) {
            std::smatch match = *i;
            auto deref_node = std::make_shared<ASTNode>();
            deref_node->type = ASTNodeType::MEMBER_ACCESS;
            deref_node->value = match[1].str() + "->" + match[2].str();
            deref_node->location_start = {line_num, static_cast<size_t>(match.position(0))};
            deref_node->location_end = {line_num, static_cast<size_t>(match.position(0) + match[0].length())};
            active_scope->add_child(deref_node);
        }

        // Check for thread invocations / async lambda
        std::regex thread_regex(R"(std::thread\s+([a-zA-Z_]\w*))");
        std::smatch thread_match;
        if (std::regex_search(clean_line, thread_match, thread_regex)) {
            auto th_node = std::make_shared<ASTNode>();
            th_node->type = ASTNodeType::VARIABLE_DECLARATION;
            th_node->value = "std::thread::" + thread_match[1].str();
            th_node->location_start = {line_num, static_cast<size_t>(thread_match.position(0))};
            th_node->location_end = {line_num, static_cast<size_t>(thread_match.position(0) + thread_match[0].length())};
            active_scope->add_child(th_node);
        }
    }

    root->location_end = {line_num, 1};
    return root;
}

} // namespace vericode
