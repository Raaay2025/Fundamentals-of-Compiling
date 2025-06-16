#include "bits/stdc++.h"

using namespace std;

// Token类型定义
enum class TokenType {
    ID, NUM,
    ADD, SUB,
    LPAREN, RPAREN,
    END, UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

// 语法树节点
struct TreeNode {
    string label;
    TreeNode *left;
    TreeNode *right;

    TreeNode(string lbl) : label(lbl), left(nullptr), right(nullptr) {
    }
};

// 词法分析器
vector<Token> tokenize(const string &input) {
    vector<Token> tokens;
    int pos = 0;

    while (pos < input.size()) {
        if (input[pos] == '(') {
            pos++;
            string content;
            bool tag = false;
            while (pos < input.size() && !(input[pos] == ')' && tag)) {
                if (input[pos] == ',') tag = true;
                content += input[pos++];
            }
            if (pos >= input.size()) {
                cerr << "词法错误：括号不匹配" << endl;
                return {};
            }
            pos++;

            size_t comma = content.find(',');
            string typeStr = content.substr(0, comma);
            string value = (comma != string::npos) ? content.substr(comma + 1) : "";

            Token token;
            if (typeStr == "i") token.type = TokenType::ID;
            else if (typeStr == "+") token.type = TokenType::ADD;
            else if (typeStr == "-") token.type = TokenType::SUB;
            else if (typeStr == "num") token.type = TokenType::NUM;
            else if (typeStr == "(") token.type = TokenType::LPAREN;
            else if (typeStr == ")") token.type = TokenType::RPAREN;
            else if (typeStr == "#") token.type = TokenType::END;
            else {
                cerr << "未知符号类型: " << typeStr << endl;
                return {};
            }
            token.value = value;
            tokens.push_back(token);
        } else pos++;
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}

// 递归下降分析器
class Parser {
    vector<Token> tokens;
    int current = 0;
    bool hasError = false;

    Token currentToken() {
        return (current < tokens.size()) ? tokens[current] : Token{TokenType::END, ""};
    }

    void advance() {
        if (current < tokens.size()) current++;
    }

public:
    Parser(vector<Token> t) : tokens(t) {
    }

    TreeNode *parse() {
        TreeNode *root = E();
        if (hasError || currentToken().type != TokenType::END) {
            cerr << "语法错误！" << endl;
            return nullptr;
        }
        return root;
    }

private:
    TreeNode *E() {
        TreeNode *t = T();
        if (!t) {
            hasError = true;
            return nullptr;
        }
        return R(t);
    }

    TreeNode *R(TreeNode *i) {
        if (currentToken().type == TokenType::ADD || currentToken().type == TokenType::SUB) {
            string op = currentToken().type == TokenType::ADD ? "+" : "-";
            advance();

            TreeNode *t = T();
            if (!t) {
                hasError = true;
                return nullptr;
            }

            TreeNode *node = new TreeNode(op);
            node->left = i;
            node->right = t;

            return R(node);
        }
        return i;
    }

    TreeNode *T() {
        TreeNode *node = nullptr;
        Token ct = currentToken();
        if (ct.type == TokenType::ID) {
            node = new TreeNode("id:" + ct.value);
            advance();
        } else if (ct.type == TokenType::NUM) {
            node = new TreeNode("num:" + ct.value);
            advance();
        } else {
            hasError = true;
        }
        return node;
    }
};

// 表达式求值函数
int evaluate(TreeNode *node) {
    if (!node) return 0;

    // 处理叶子节点
    if (!node->left && !node->right) {
        if (node->label.find("num:") == 0) {
            return stoi(node->label.substr(4));
        }
        return 0;
    }

    // 递归求值
    int left = evaluate(node->left);
    int right = evaluate(node->right);

    if (node->label == "+") return left + right;
    if (node->label == "-") return left - right;
    return 0;
}

// 语法树打印功能
struct SubtreeDisplay {
    vector<string> lines;
    int width, root_pos;
};

SubtreeDisplay get_subtree(TreeNode *node) {
    if (!node) return {{}, 0, 0};

    if (!node->left && !node->right) {
        return {{node->label}, (int) node->label.size(), (int) node->label.size() / 2};
    }

    SubtreeDisplay left = get_subtree(node->left);
    SubtreeDisplay right = get_subtree(node->right);

    string root_line = string(left.width, ' ') + node->label + string(right.width, ' ');
    vector<string> lines = {root_line};

    if (node->left || node->right) {
        string conn_line;
        if (node->left) {
            conn_line += string(left.root_pos, ' ') + "/" + string(left.width - left.root_pos - 1, ' ');
        }
        if (node->right) {
            int rpos = left.width + node->label.size() + right.root_pos;
            if (rpos > conn_line.size()) conn_line += string(rpos - conn_line.size(), ' ');
            conn_line += "\\";
        }
        lines.push_back(conn_line);
    }

    int max_child = max(left.lines.size(), right.lines.size());
    for (int i = 0; i < max_child; ++i) {
        string l = (i < left.lines.size()) ? left.lines[i] : string(left.width, ' ');
        string r = (i < right.lines.size()) ? right.lines[i] : string(right.width, ' ');
        lines.push_back(l + string(node->label.size() + 2, ' ') + r);
    }

    return {lines, (int) root_line.size(), left.width + (int) node->label.size() / 2};
}

void print_tree(TreeNode *root) {
    SubtreeDisplay disp = get_subtree(root);
    cout << "\n抽象语法树：" << endl;
    for (const auto &line: disp.lines) {
        cout << line << endl;
    }
}

int main() {
    string input = "(num,9)(-,)(num,5)(+,)(num,2)(#,)";

    // string input = "(num,5)(+,)(num,8)(-,)(num,15)(-,)(num,9)(#,)";


    vector<Token> tokens = tokenize(input);
    if (tokens.empty()) {
        cerr << "词法分析失败！" << endl;
        return 1;
    }

    Parser parser(tokens);
    TreeNode *ast = parser.parse();

    if (ast) {
        int result = evaluate(ast);
        cout << "\n表达式计算结果: " << result << endl;
        print_tree(ast);
    } else {
        cout << "分析失败！" << endl;
    }

    return 0;
}
