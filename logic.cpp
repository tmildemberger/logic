#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

enum class TokenType {
    implication,
    disjunction,
    conjunction,
    negation,
    start_formula,
    end_formula,
    atom,
};

class Token {
public:
    Token(TokenType type) : Token(type, "") { }
    Token(TokenType type, std::string value) : type_(type), value_(value) { }
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
private:
    TokenType type_;
    std::string value_;
};

class FormulaNode {
public:
    FormulaNode();
    void addChild(std::shared_ptr<FormulaNode> new_child) {
        children.push_back(new_child);
    }
private:
    std::shared_ptr<FormulaNode> parent { nullptr };
    std::vector<std::shared_ptr<FormulaNode>> children;
};

class Parser {
public:
    Parser(std::vector<Token>&& tokens) : tokens_(tokens) { }
    FormulaNode formula() {
        return disjunction();
    }
    FormulaNode disjunction() {

    }
    FormulaNode implication() {

    }
    FormulaNode conjunction() {

    }
    FormulaNode unary() {

    }
    FormulaNode primary() {

    }
    void parse() {
        root.addChild(std::make_shared<FormulaNode>(formula()));
    }
private:
    std::vector<Token> tokens_;
    FormulaNode root;
};

int main() {
    std::string formula;
    std::getline(std::cin, formula);
    std::vector<Token> tokens;
    std::stringstream curr_atom;
    for (std::size_t i { 0 }; i < formula.size(); ++i) {
        if (char c { formula[i] }; (c >= 'a' && c <= 'z') ||
                                   (c >= 'A' && c <= 'Z') ||
                                   (c >= '0' && c <= '9') ||
                                    c == '_') {
            curr_atom << c;
        } else {
            if (curr_atom.str().size() > 0) {
                tokens.emplace_back(TokenType::atom, curr_atom.str());
                curr_atom.clear();
                curr_atom.str("");
            }
            switch (formula[i]) {
                case '&':
                case '*':
                    tokens.emplace_back(TokenType::conjunction);
                    break;
                case '|':
                case '+':
                    tokens.emplace_back(TokenType::disjunction);
                    break;
                case '!':
                case '~':
                    tokens.emplace_back(TokenType::negation);
                    break;
                case '(':
                    tokens.emplace_back(TokenType::start_formula);
                    break;
                case ')':
                    tokens.emplace_back(TokenType::end_formula);
                    break;
                case '-':
                    if (formula[i + 1] == '>') {
                        tokens.emplace_back(TokenType::implication);
                    }
                    break;
                default:
                    if (char c { formula[i] }; !(c == ' ' || c == '\n' ||
                                                c == '\t' || c == '\0')) {
                        std::cout << "That char I didn't understand\n";
                    }
                    break;
            }
        }
    }
    for (const auto& tok : tokens) {
        switch (tok.getType()) {
            case TokenType::conjunction:
                std::cout << '&';
                break;
            case TokenType::disjunction:
                std::cout << '|';
                break;
            case TokenType::negation:
                std::cout << '!';
                break;
            case TokenType::start_formula:
                std::cout << '(';
                break;
            case TokenType::end_formula:
                std::cout << ')';
                break;
            case TokenType::atom:
                std::cout << tok.getValue();
                break;
            default:
                break;
        }
    }
    Parser parser(std::move(tokens));
    // std::cout << formula << "\n";
    return 0;
}