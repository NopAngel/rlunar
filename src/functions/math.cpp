#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <memory>

enum TokenType {
    TOK_NUMBER,
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,
    TOK_DIV,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_EOF,
    TOK_UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
    std::string source;
    size_t pos;

public:
    Lexer(const std::string& src) : source(src), pos(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < source.length()) {
            if (isdigit(source[pos])) {
                tokens.push_back({TOK_NUMBER, readNumber()});
            } else if (source[pos] == '+') {
                tokens.push_back({TOK_PLUS, "+"});
                pos++;
            } else if (source[pos] == '-') {
                tokens.push_back({TOK_MINUS, "-"});
                pos++;
            } else if (source[pos] == '*') {
                tokens.push_back({TOK_MUL, "*"});
                pos++;
            } else if (source[pos] == '/') {
                tokens.push_back({TOK_DIV, "/"});
                pos++;
            } else if (source[pos] == '(') {
                tokens.push_back({TOK_LPAREN, "("});
                pos++;
            } else if (source[pos] == ')') {
                tokens.push_back({TOK_RPAREN, ")"});
                pos++;
            } else {
                pos++;
            }
        }
        tokens.push_back({TOK_EOF, ""});
        return tokens;
    }

private:
    std::string readNumber() {
        size_t start = pos;
        while (pos < source.length() && isdigit(source[pos])) {
            pos++;
        }
        return source.substr(start, pos - start);
    }
};

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct NumberNode : ASTNode {
    double value;
    NumberNode(double val) : value(val) {}
};

struct BinaryOpNode : ASTNode {
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    char op;
    BinaryOpNode(char oper, std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : op(oper), left(std::move(lhs)), right(std::move(rhs)) {}
};

class Parser {
    std::vector<Token> tokens;
    size_t pos;

public:
    Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}

    std::unique_ptr<ASTNode> parse() {
        return parseExpression();
    }

private:
    std::unique_ptr<ASTNode> parseExpression() {
        auto left = parseTerm();
        while (tokens[pos].type == TOK_PLUS || tokens[pos].type == TOK_MINUS) {
            char op = tokens[pos].value[0];
            pos++;
            auto right = parseTerm();
            left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ASTNode> parseTerm() {
        auto left = parseFactor();
        while (tokens[pos].type == TOK_MUL || tokens[pos].type == TOK_DIV) {
            char op = tokens[pos].value[0];
            pos++;
            auto right = parseFactor();
            left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ASTNode> parseFactor() {
        if (tokens[pos].type == TOK_NUMBER) {
            double value = std::stod(tokens[pos].value);
            pos++;
            return std::make_unique<NumberNode>(value);
        } else if (tokens[pos].type == TOK_LPAREN) {
            pos++;
            auto expr = parseExpression();
            pos++;  // Consume RPAREN
            return expr;
        } else {
            // Error handling
            std::cerr << "Unexpected token: " << tokens[pos].value << std::endl;
            return nullptr;
        }
    }
};

double evaluate(const std::unique_ptr<ASTNode>& node) {
    if (auto numberNode = dynamic_cast<NumberNode*>(node.get())) {
        return numberNode->value;
    } else if (auto binaryOpNode = dynamic_cast<BinaryOpNode*>(node.get())) {
        double leftValue = evaluate(binaryOpNode->left);
        double rightValue = evaluate(binaryOpNode->right);
        switch (binaryOpNode->op) {
            case '+': return leftValue + rightValue;
            case '-': return leftValue - rightValue;
            case '*': return leftValue * rightValue;
            case '/': return leftValue / rightValue;
            default: return 0.0;  // Error handling
        }
    } else {
        return 0.0;  // Error handling
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.rmath>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto ast = parser.parse();

    double result = evaluate(ast);
    std::cout << "CLI: " << result << std::endl;

    return 0;
}
