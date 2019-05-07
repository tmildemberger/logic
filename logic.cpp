#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <utility>

std::vector<std::string>
split(const std::string& str, const std::string& delim);

// NEEDS IMPROVEMENT: documentation
/**
 *  @brief  Separates a given string into a list
 *  @param str    String to be separated
 *  @param delim  Delimitier string
 *  @return A list containing substrings of @a str
*/
std::vector<std::string>
split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> ret;
    std::size_t st { 0 }, ed { 0 };
    for (ed =  str.find(delim, st);
         ed != std::string::npos;
         ed =  str.find(delim, st)) {
        if (auto sub { str.substr(st, ed - st) }; sub.size() != 0) {
            ret.push_back(sub);
        }
        st = ed + delim.size();
    }
    // maybe do that better
    if (auto sub { str.substr(st, str.size() - st) }; sub.size() != 0) {
        ret.push_back(sub);
    }
    return ret;
}
/** TESTS
 *  split("12.34.5", ".") == { "12", "34", "5" }
 *  split("2,.22.2,2", "2") == { ",.", ".", "," }
 *  split("aaaaa", "a") == {}
 *  split("ok", " ") == { "ok" }
*/

template <typename T>
bool operator==(const std::vector<T> a, const std::vector<T> b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i { 0 }; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

int main() {
    auto exp { [] (bool cond) {
        if (cond) std::cout << "test passed\n";
        else std::cout << "test not passed\n";
    } };
    for (const auto& x : split("2,.22.2,2", "2")) {
        std::cout << x << '\n';
    }
    std::cout << '\n';
    exp(split("12.34.5", ".") == std::vector<std::string>{ "12", "34", "5" });
    exp(split("2,.22.2,2", "2") == std::vector<std::string>{ ",.", ".", "," });
    exp(split("aaaaa", "a") == std::vector<std::string>{});
    exp(split("ok", " ") == std::vector<std::string>{ "ok" });
    return 0;
}

// Old part:

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
    FormulaNode() { }
    void addChild(std::shared_ptr<FormulaNode> new_child) {
        children.push_back(new_child);
        // new_child->setParent();
    }
    void setText(const std::string& txt) {
        this->text = txt;
    }
    const std::vector<std::shared_ptr<FormulaNode>>& getChildren() {
        return this->children;
    }
    void print() {
        std::cout << text;
    }
private:
    // void setParent(std::shared_ptr<FormulaNode> parent) {
    //     this->parent = parent;
    // }
    std::string text;
    // std::shared_ptr<FormulaNode> parent { nullptr };
    std::vector<std::shared_ptr<FormulaNode>> children;
};

class Parser {
public:
    Parser(std::vector<Token>&& tokens) :
        tokens_{ tokens }, current{ tokens_.cbegin() } { }
    virtual void parse() = 0;
    virtual void print() = 0;
    virtual ~Parser() {}
protected:
    void debug(std::string message) {
        std::cout << '\'' << message << "' in:\n";
        std::vector<std::string> toks_rep;
        for (const auto& tok : tokens_) {
            switch (tok.getType()) {
                case TokenType::conjunction:
                    std::cout << "&";
                    toks_rep.push_back("&");
                    break;
                case TokenType::implication:
                    std::cout << "->";
                    toks_rep.push_back("->");
                    break;
                case TokenType::disjunction:
                    std::cout << "|";
                    toks_rep.push_back("|");
                    break;
                case TokenType::negation:
                    std::cout << "!";
                    toks_rep.push_back("!");
                    break;
                case TokenType::start_formula:
                    std::cout << "(";
                    toks_rep.push_back("(");
                    break;
                case TokenType::end_formula:
                    std::cout << ")";
                    toks_rep.push_back("");
                    break;
                case TokenType::atom:
                    std::cout << tok.getValue();
                    toks_rep.push_back(tok.getValue());
                    break;
                default:
                    break;
            }
        }
        std::cout << '\n';
        auto reps { toks_rep.cbegin() };
        for (auto i { tokens_.cbegin() }; i != current; ++i, ++reps) {
            for (std::size_t s  { 0 }; s < reps->size(); ++s) {
                std::cout << ' ';
            }
        }
        std::cout << "^\n";
    }
    std::vector<Token> tokens_;
    std::vector<Token>::const_iterator current;
    std::vector<std::string> errors;
    std::shared_ptr<FormulaNode> root;
    bool valid { true };
};

class TopDownParser : public Parser {
public:
    TopDownParser(std::vector<Token>&& tokens) : Parser(std::move(tokens)) { }
    void parse() override {
        root = std::make_shared<FormulaNode>();
        root->addChild(formula());
        if (errors.size() > 0) {
            valid = false;
            for (const auto& err : errors) {
                std::cout << err;
            }
        }
    }
    void print() override {
        if (valid) {
            print(this->root, "");
        } else {
            std::cout << "there were errors\n";
        }
    }
private:
    void print(std::shared_ptr<FormulaNode> node, const std::string& prefix) {
        std::cout << prefix;
        node->print();
        std::cout << "\n";
        // if (node->getChildren().size()) std::cout << '(';
        for (const auto& child : node->getChildren()) {
            // std::cout << ' ';
            print(child, prefix + "  ");
        }
        // if (node->getChildren().size()) std::cout << ')';
    }
    std::shared_ptr<FormulaNode> formula() {
        return disjunction();
    }
    std::shared_ptr<FormulaNode> disjunction() {
        std::shared_ptr<FormulaNode> node { implication() };
        if ((*current).getType() == TokenType::disjunction) {
            ++current;
            auto disj { std::make_shared<FormulaNode>() };
            disj->setText("|");
            disj->addChild(node);
            disj->addChild(implication());
            return disj;
        }
        return node;
    }
    std::shared_ptr<FormulaNode> implication() {
        std::shared_ptr<FormulaNode> node { conjunction() };
        if ((*current).getType() == TokenType::implication) {
            ++current;
            auto impl { std::make_shared<FormulaNode>() };
            impl->setText("->");
            impl->addChild(node);
            impl->addChild(conjunction());
            return impl;
        }
        return node;
    }
    std::shared_ptr<FormulaNode> conjunction() {
        std::shared_ptr<FormulaNode> node { unary() };
        if ((*current).getType() == TokenType::conjunction) {
            ++current;
            auto conj { std::make_shared<FormulaNode>() };
            conj->setText("&");
            conj->addChild(node);
            conj->addChild(unary());
            return conj;
        }
        return node;
    }
    std::shared_ptr<FormulaNode> unary() {
        if ((*current).getType() == TokenType::negation) {
            ++current;
            auto neg { std::make_shared<FormulaNode>() };
            neg->setText("!");
            neg->addChild(unary());
            return neg;
        }
        return primary();
    }
    std::shared_ptr<FormulaNode> primary() {
        if ((*current).getType() == TokenType::atom) {
            auto atom { std::make_shared<FormulaNode>() };
            atom->setText((*current).getValue());
            ++current;
            return atom;
        } else if ((*current).getType() == TokenType::start_formula) {
            ++current;
            auto form { formula() };
            if ((*current).getType() == TokenType::end_formula) {
                ++current;
                return form;
            } else {
                errors.push_back("syntax error: expected ')'\n");
            }
        } else {
            errors.push_back("syntax error: expected primary\n");
        }
    }
};

class ParenthesisParser : public Parser {
public:
    ParenthesisParser(std::vector<Token>&& tokens) : Parser(std::move(tokens)) { }
    void parse() override {
        root = std::make_shared<FormulaNode>();
        root->addChild(formula());
        if (current != tokens_.cend()) {
            valid = false;
            errors.push_back("error in 'parse':\ntoo much input\n");
        }
        if (errors.size() > 0) {
            valid = false;
            for (const auto& err : errors) {
                std::cout << err;
            }
        }
    }
    void print() override {
        if (valid) {
            print(this->root, "");
        } else {
            std::cout << "there were errors\n";
        }
    }
private:
    // std::stringstream errors;
    void print(std::shared_ptr<FormulaNode> node, const std::string& prefix) {
        std::cout << prefix;
        node->print();
        std::cout << "\n";
        // if (node->getChildren().size()) std::cout << '(';
        for (const auto& child : node->getChildren()) {
            // std::cout << ' ';
            print(child, prefix + "  ");
        }
        // if (node->getChildren().size()) std::cout << ')';
    }
    std::shared_ptr<FormulaNode> formula() {
        if (current->getType() == TokenType::atom) {
            auto atom { std::make_shared<FormulaNode>() };
            atom->setText(current->getValue());
            ++current;
            return atom;
        } else {
            auto paren { parenthesized() };
            if (paren) return paren;
            auto neg { negation() };
            if (neg) return neg;
            valid = false;
            errors.push_back("error in 'formula'\n");
            return nullptr;
        }
    }
    std::shared_ptr<FormulaNode> parenthesized() {
        if (current->getType() == TokenType::start_formula) {
            ++current;
            auto form { binary() };
            if (!form || current->getType() != TokenType::end_formula) {
                valid = false;
                errors.push_back("error in 'parenthesized'\n");
                return nullptr;
            }
            ++current;
            return form;
        } else {
            return nullptr;
        }
    }
    std::shared_ptr<FormulaNode> binary() {
        auto first { formula() };
        if (!first) {
            valid = false;
            errors.push_back("error in 'binary' first\n");
            return nullptr;
        }
        if (current->getType() == TokenType::conjunction ||
            current->getType() == TokenType::disjunction ||
            current->getType() == TokenType::implication) {
            auto oper { std::make_shared<FormulaNode>() };
            switch (current->getType()) {
                case TokenType::conjunction:
                    oper->setText("&");
                    break;
                case TokenType::disjunction:
                    oper->setText("|");
                    break;
                case TokenType::implication:
                    oper->setText("->");
                    break;
                default:
                    break;
            }
            ++current;
            oper->addChild(first);
            auto second { formula() };
            if (!second) {
                valid = false;
                errors.push_back("error in 'binary' second\n");
                return nullptr;
            }
            oper->addChild(second);
            return oper;
        } else {
            valid = false;
            errors.push_back("error in 'binary' operator\n");
            return nullptr;
        }
    }
    std::shared_ptr<FormulaNode> negation() {
        if (current->getType() == TokenType::negation) {
            ++current;
            auto neg { std::make_shared<FormulaNode>() };
            neg->setText("!");
            auto childForm { formula() };
            if (childForm) {
                neg->addChild(childForm);
                return neg;
            } else {
                valid = false;
                errors.push_back("error in 'negation'\n");
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }
    // std::shared_ptr<FormulaNode> disjunction() {
    //     std::shared_ptr<FormulaNode> node { implication() };
    //     if ((*current).getType() == TokenType::disjunction) {
    //         ++current;
    //         auto disj { std::make_shared<FormulaNode>() };
    //         disj->setText("|");
    //         disj->addChild(node);
    //         disj->addChild(implication());
    //         return disj;
    //     }
    //     return node;
    // }
    // std::shared_ptr<FormulaNode> implication() {
    //     std::shared_ptr<FormulaNode> node { conjunction() };
    //     if ((*current).getType() == TokenType::implication) {
    //         ++current;
    //         auto impl { std::make_shared<FormulaNode>() };
    //         impl->setText("->");
    //         impl->addChild(node);
    //         impl->addChild(conjunction());
    //         return impl;
    //     }
    //     return node;
    // }
    // std::shared_ptr<FormulaNode> conjunction() {
    //     std::shared_ptr<FormulaNode> node { unary() };
    //     if ((*current).getType() == TokenType::conjunction) {
    //         ++current;
    //         auto conj { std::make_shared<FormulaNode>() };
    //         conj->setText("&");
    //         conj->addChild(node);
    //         conj->addChild(unary());
    //         return conj;
    //     }
    //     return node;
    // }
    // std::shared_ptr<FormulaNode> unary() {
    //     if ((*current).getType() == TokenType::negation) {
    //         ++current;
    //         auto neg { std::make_shared<FormulaNode>() };
    //         neg->setText("!");
    //         neg->addChild(unary());
    //         return neg;
    //     }
    //     return primary();
    // }
    // std::shared_ptr<FormulaNode> primary() {
    //     if ((*current).getType() == TokenType::atom) {
    //         auto atom { std::make_shared<FormulaNode>() };
    //         atom->setText((*current).getValue());
    //         ++current;
    //         return atom;
    //     } else if ((*current).getType() == TokenType::start_formula) {
    //         ++current;
    //         auto form { formula() };
    //         if ((*current).getType() == TokenType::end_formula) {
    //             ++current;
    //             return form;
    //         } else {
    //             errors.push_back("syntax error: expected ')'\n");
    //         }
    //     } else {
    //         errors.push_back("syntax error: expected primary\n");
    //     }
    // }
};
class PrecedenceParser : public Parser {
public:
    PrecedenceParser(std::vector<Token>&& tokens) : Parser(std::move(tokens)) { }
    void parse() override {
        root = std::make_shared<FormulaNode>();
        root->addChild(formula());
        if (current != tokens_.cend()) {
            valid = false;
            errors.push_back("error in 'parse':\ntoo much input\n");
            debug("parse_error");
        }
        if (errors.size() > 0) {
            valid = false;
            for (const auto& err : errors) {
                std::cout << err;
            }
        }
    }
    void print() override {
        if (valid) {
            print(this->root, "");
        } else {
            std::cout << "there were errors\n";
        }
    }
private:
    void print(std::shared_ptr<FormulaNode> node, const std::string& prefix) {
        std::cout << prefix;
        node->print();
        std::cout << "\n";
        // if (node->getChildren().size()) std::cout << '(';
        for (const auto& child : node->getChildren()) {
            // std::cout << ' ';
            print(child, prefix + "  ");
        }
        // if (node->getChildren().size()) std::cout << ')';
    }
    std::shared_ptr<FormulaNode> formula() {
        auto impl { implication() };
        if (impl) return impl;
        valid = false;
        errors.push_back("error in 'formula'\n");
        return nullptr;
    }
    std::shared_ptr<FormulaNode> implication() {
        // debug("implication");
        auto first { disjunction() };
        if (!first) {
            valid = false;
            errors.push_back("error in 'implication first'\n");
            return nullptr;
        }
        while (current != tokens_.cend() && current->getType() == TokenType::implication) {
            ++current;
            auto next { disjunction() };
            if (!next) {
                valid = false;
                errors.push_back("error in 'implication next'\n");
                return nullptr;
            }
            auto impl { std::make_shared<FormulaNode>() };
            impl->setText("->");
            impl->addChild(first);
            impl->addChild(next);
            first = impl;
        }
        return first;
    }
    std::shared_ptr<FormulaNode> disjunction() {
        // debug("disjunction");
        auto first { conjunction() };
        if (!first) {
            valid = false;
            errors.push_back("error in 'disjunction first'\n");
            return nullptr;
        }
        while (current != tokens_.cend() && current->getType() == TokenType::disjunction) {
            ++current;
            auto next { conjunction() };
            if (!next) {
                valid = false;
                errors.push_back("error in 'disjunction next'\n");
                return nullptr;
            }
            auto disj { std::make_shared<FormulaNode>() };
            disj->setText("|");
            disj->addChild(first);
            disj->addChild(next);
            first = disj;
        }
        return first;
    }
    std::shared_ptr<FormulaNode> conjunction() {
        // debug("conjunction");
        auto first { negation() };
        if (!first) {
            valid = false;
            errors.push_back("error in 'conjunction first'\n");
            return nullptr;
        }
        while (current != tokens_.cend() && current->getType() == TokenType::conjunction) {
            ++current;
            auto next { negation() };
            if (!next) {
                valid = false;
                errors.push_back("error in 'conjunction next'\n");
                return nullptr;
            }
            auto conj { std::make_shared<FormulaNode>() };
            conj->setText("&");
            conj->addChild(first);
            conj->addChild(next);
            first = conj;
        }
        return first;
    }
    std::shared_ptr<FormulaNode> negation() {
        // debug("negation");
        if (current->getType() == TokenType::negation) {
            ++current;
            auto neg { std::make_shared<FormulaNode>() };
            // auto form { formula() };
            auto form { negation() };
            if (form) {
                neg->addChild(form);
                neg->setText("!");
                return neg;
            } else {
                valid = false;
                errors.push_back("error in 'negation'\n");
                return nullptr;
            }
        } else {
            auto prim { primary() };
            if (prim) return prim;
            valid = false;
            errors.push_back("error in 'negation prim'\n");
            return nullptr;
        }
    }
    std::shared_ptr<FormulaNode> primary() {
        // debug("primary");
        if (current->getType() == TokenType::atom) {
            auto atom { std::make_shared<FormulaNode>() };
            atom->setText(current->getValue());
            ++current;
            return atom;
        } else if (current->getType() == TokenType::start_formula) {
            ++current;
            auto form { formula() };
            if (!form || current->getType() != TokenType::end_formula) {
                valid = false;
                errors.push_back("error in 'primary paren'\n");
                return nullptr;
            }
            ++current;
            return form;
        } else {
            valid = false;
            errors.push_back("error in 'nonexistent primary'\n");
            return nullptr;
        }
    }
    // std::shared_ptr<FormulaNode> parenthesized() {
    //     if (current->getType() == TokenType::start_formula) {
    //         ++current;
    //         auto form { binary() };
    //         if (!form || current->getType() != TokenType::end_formula) {
    //             valid = false;
    //             errors.push_back("error in 'parenthesized'\n");
    //             return nullptr;
    //         }
    //         ++current;
    //         return form;
    //     } else {
    //         return nullptr;
    //     }
    // }
    // std::shared_ptr<FormulaNode> binary() {
    //     auto first { formula() };
    //     if (!first) {
    //         valid = false;
    //         errors.push_back("error in 'binary' first\n");
    //         return nullptr;
    //     }
    //     if (current->getType() == TokenType::conjunction ||
    //         current->getType() == TokenType::disjunction ||
    //         current->getType() == TokenType::implication) {
    //         auto oper { std::make_shared<FormulaNode>() };
    //         switch (current->getType()) {
    //             case TokenType::conjunction:
    //                 oper->setText("&");
    //                 break;
    //             case TokenType::disjunction:
    //                 oper->setText("|");
    //                 break;
    //             case TokenType::implication:
    //                 oper->setText("->");
    //                 break;
    //         }
    //         ++current;
    //         oper->addChild(first);
    //         auto second { formula() };
    //         if (!second) {
    //             valid = false;
    //             errors.push_back("error in 'binary' second\n");
    //             return nullptr;
    //         }
    //         oper->addChild(second);
    //         return oper;
    //     } else {
    //         valid = false;
    //         errors.push_back("error in 'binary' operator\n");
    //         return nullptr;
    //     }
    // }
    // std::shared_ptr<FormulaNode> negation() {
    //     if (current->getType() == TokenType::negation) {
    //         ++current;
    //         auto neg { std::make_shared<FormulaNode>() };
    //         neg->setText("!");
    //         auto childForm { formula() };
    //         if (childForm) {
    //             neg->addChild(childForm);
    //             return neg;
    //         } else {
    //             valid = false;
    //             errors.push_back("error in 'negation'\n");
    //             return nullptr;
    //         }
    //     } else {
    //         return nullptr;
    //     }
    // }
};
// int main() {
//     std::string formula;
//     std::getline(std::cin, formula);
//     std::vector<Token> tokens;
//     std::stringstream curr_atom;
//     for (std::size_t i { 0 }; i < formula.size(); ++i) {
//         if (char c { formula[i] }; (c >= 'a' && c <= 'z') ||
//                                    (c >= 'A' && c <= 'Z') ||
//                                    (c >= '0' && c <= '9') ||
//                                     c == '_') {
//             curr_atom << c;
//         } else {
//             if (curr_atom.str().size() > 0) {
//                 tokens.emplace_back(TokenType::atom, curr_atom.str());
//                 curr_atom.clear();
//                 curr_atom.str("");
//             }
//             switch (formula[i]) {
//                 case '&':
//                 case '*':
//                     tokens.emplace_back(TokenType::conjunction);
//                     break;
//                 case '|':
//                 case '+':
//                     tokens.emplace_back(TokenType::disjunction);
//                     break;
//                 case '!':
//                 case '~':
//                     tokens.emplace_back(TokenType::negation);
//                     break;
//                 case '(':
//                     tokens.emplace_back(TokenType::start_formula);
//                     break;
//                 case ')':
//                     tokens.emplace_back(TokenType::end_formula);
//                     break;
//                 case '-':
//                     if (formula[i + 1] == '>') {
//                         tokens.emplace_back(TokenType::implication);
//                         ++i;
//                     }
//                     break;
//                 default:
//                     if (char ch { formula[i] }; !(ch == ' ' || ch == '\n' ||
//                                                  ch == '\t' || ch == '\0')) {
//                         std::cout << "That char I didn't understand\n";
//                     }
//                     break;
//             }
//         }
//     }
//     if (curr_atom.str().size() > 0) {
//         tokens.emplace_back(TokenType::atom, curr_atom.str());
//         curr_atom.clear();
//         curr_atom.str("");
//     }
//     for (const auto& tok : tokens) {
//         switch (tok.getType()) {
//             case TokenType::conjunction:
//                 std::cout << "conj(&)";
//                 break;
//             case TokenType::implication:
//                 std::cout << "impl(->)";
//                 break;
//             case TokenType::disjunction:
//                 std::cout << "disj(|)";
//                 break;
//             case TokenType::negation:
//                 std::cout << "neg(!)";
//                 break;
//             case TokenType::start_formula:
//                 std::cout << "st(()";
//                 break;
//             case TokenType::end_formula:
//                 std::cout << "ed())";
//                 break;
//             case TokenType::atom:
//                 std::cout << "atom(" << tok.getValue() << ")";
//                 break;
//             default:
//                 break;
//         }
//     }
//     std::cout << '\n';
//     PrecedenceParser parser(std::move(tokens));
//     parser.parse();
//     parser.print();
//     // std::cout << formula << "\n";
//     return 0;
// }