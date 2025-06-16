#include "bits/stdc++.h"

using namespace std;

// Token����
enum class TokenType {
    ID,     // ��ʶ��
    ADD,    // +
    MUL,    // *
    LPAREN, // (
    RPAREN, // )
    END,    // #
    UNKNOWN // δ֪����
};

struct Token {
    TokenType type;
    string value;
};

// �﷨���ڵ�
struct TreeNode {
    string label;
    TreeNode* left;
    TreeNode* right;

    TreeNode(string lbl) : label(lbl), left(nullptr), right(nullptr) {}
};

// �ʷ�������
vector<Token> tokenize(const string& input) {
    vector<Token> tokens;
    int pos = 0;

    while (pos < input.size()) {
        if (isspace(input[pos])) {
            pos++;
            continue;
        }

        if (input[pos] == '(') {
            pos++;
            string content;
            bool tag = false;  // ��֤�ܶ�ȡ��һ������
            while (pos < input.size()) {
                if (input[pos] == ')' && tag) break;  // ��ȡ��һ�����Ų��Ҷ�����������
                content += input[pos];
                if (input[pos] == ',') tag = true;
                pos++;
            }
            if (pos >= input.size()) {
                cerr << "Error: Missing closing parenthesis" << endl;
                return {};
            }
            pos++;

            int comma = content.find(',');
            string typeStr = content.substr(0, comma);
            string value = comma != string::npos ? content.substr(comma+1) : "";

            Token token;
            if (typeStr == "i")       token.type = TokenType::ID;
            else if (typeStr == "+") token.type = TokenType::ADD;
            else if (typeStr == "*") token.type = TokenType::MUL;
            else if (typeStr == "(") token.type = TokenType::LPAREN;
            else if (typeStr == ")") token.type = TokenType::RPAREN;
            else if (typeStr == "#") token.type = TokenType::END;
            else {
                cerr << "Unknown token type: " << typeStr << endl;
                return {};
            }
            token.value = value;
            tokens.push_back(token);
        }
        else {
            cerr << "Unexpected character: " << input[pos] << endl;
            return {};
        }
    }
    return tokens;
}

// �ݹ��½�������
class Parser {
    vector<Token> tokens;
    int current = 0;
    bool hasError = false;
    int indent = 0;

    void printIndent() {
        for (int i = 0; i < indent; ++i) cout << "  ";
    }

    Token currentToken() {
        if (current < tokens.size()) return tokens[current];
        return {TokenType::END, ""};
    }

    void advance() {
        if (current < tokens.size()) current++;
    }

public:
    Parser(vector<Token> t) : tokens(t) {}

    TreeNode* parse() {
        cout << "��ʼ�﷨����..." << endl;
        TreeNode* root = E();
        if (hasError || current != tokens.size() - 1 ||
            currentToken().type != TokenType::END) {
            cerr << "�﷨�����쳣��ֹ" << endl;
            return nullptr;
        }
        cout << "�﷨�������" << endl;
        return root;
    }

private:
    TreeNode* E() {
        printIndent();
        cout << "���� E" << endl;
        indent++;

        TreeNode* t_node = T();
        TreeNode* result = E_prime(t_node);

        indent--;
        printIndent();
        cout << "�뿪 E" << endl;
        return result;
    }

    TreeNode* E_prime(TreeNode* current) {
        printIndent();
        cout << "���� E'" << endl;
        indent++;

        if (currentToken().type == TokenType::ADD) {
            printIndent();
            cout << "ƥ�������: +" << endl;
            advance();

            TreeNode* t_node = T();
            TreeNode* add_node = new TreeNode("+");
            add_node->left = current;
            add_node->right = t_node;

            TreeNode* result = E_prime(add_node);

            indent--;
            printIndent();
            cout << "�뿪 E'" << endl;
            return result;
        }

        indent--;
        printIndent();
        cout << "�뿪 E'��epsƥ�䣩" << endl;
        return current;
    }

    TreeNode* T() {
        printIndent();
        cout << "���� T" << endl;
        indent++;

        TreeNode* f_node = F();
        TreeNode* result = T_prime(f_node);

        indent--;
        printIndent();
        cout << "�뿪 T" << endl;
        return result;
    }

    TreeNode* T_prime(TreeNode* current) {
        printIndent();
        cout << "���� T'" << endl;
        indent++;

        if (currentToken().type == TokenType::MUL) {
            printIndent();
            cout << "ƥ�������: *" << endl;
            advance();

            TreeNode* f_node = F();
            TreeNode* mul_node = new TreeNode("*");
            mul_node->left = current;
            mul_node->right = f_node;

            TreeNode* result = T_prime(mul_node);

            indent--;
            printIndent();
            cout << "�뿪 T'" << endl;
            return result;
        }

        indent--;
        printIndent();
        cout << "�뿪 T'��epsƥ�䣩" << endl;
        return current;
    }

    TreeNode* F() {
        printIndent();
        cout << "���� F" << endl;
        indent++;

        TreeNode* node = nullptr;
        if (currentToken().type == TokenType::ID) {
            printIndent();
            cout << "ƥ���ʶ��: " << currentToken().value << endl;
            node = new TreeNode(currentToken().value);
            advance();
        }
        else if (currentToken().type == TokenType::LPAREN) {
            printIndent();
            cout << "ƥ�������� (" << endl;
            advance();

            node = E();
            if (currentToken().type != TokenType::RPAREN) {
                printIndent();
                cerr << "����ȱ�������� )" << endl;
                hasError = true;
                indent--;
                return nullptr;
            }
            printIndent();
            cout << "ƥ�������� )" << endl;
            advance();
        }
        else {
            printIndent();
            cerr << "�����ڴ���ʶ����������" << endl;
            hasError = true;
        }

        indent--;
        printIndent();
        cout << "�뿪 F" << endl;
        return node;
    }
};

// ��ӡ�﷨��
struct SubtreeDisplay {
    vector<string> lines;
    int width;
    int root_pos;
};

SubtreeDisplay get_subtree_display(TreeNode* node) {
    SubtreeDisplay result;
    if (!node) return result;

    if (!node->left && !node->right) {
        result.lines.push_back(node->label);
        result.width = node->label.length();
        result.root_pos = node->label.length() / 2;
        return result;
    }

    SubtreeDisplay left = get_subtree_display(node->left);
    SubtreeDisplay right = get_subtree_display(node->right);

    string root_line = string(left.width, ' ') + node->label + string(right.width, ' ');
    vector<string> merged_lines;

    merged_lines.push_back(root_line);

    if (node->left || node->right) {
        string connect_line;
        if (node->left) {
            connect_line += string(left.root_pos, ' ') + "/";
            if (left.width > left.root_pos + 1)
                connect_line += string(left.width - left.root_pos - 1, ' ');
        }

        if (node->right) {
            int right_pos = left.width + node->label.length() + right.root_pos;
            if (right_pos > connect_line.length())
                connect_line += string(right_pos - connect_line.length(), ' ');
            connect_line += "\\";
        }
        merged_lines.push_back(connect_line);
    }

    int child_lines = max(left.lines.size(), right.lines.size());
    for (int i = 0; i < child_lines; ++i) {
        string left_part = (i < left.lines.size()) ? left.lines[i] : string(left.width, ' ');
        string right_part = (i < right.lines.size()) ? right.lines[i] : string(right.width, ' ');
        merged_lines.push_back(left_part + "  " + right_part);
    }

    result.lines = merged_lines;
    result.width = left.width + node->label.length() + right.width;
    result.root_pos = left.width + node->label.length() / 2;
    return result;
}

void print_tree(TreeNode* root) {
    cout << "\n�﷨���ṹ��" << endl;
    SubtreeDisplay display = get_subtree_display(root);
    for (const auto& line : display.lines) {
        cout << line << endl;
    }
}

int main() {
    string input = "(i,i1) (+, ) (i,i2) (*, ) ((, ) (i,i3) (+, ) (i,i4) (), ) (#, )";

    auto tokens = tokenize(input);
    if (tokens.empty()) {
        cerr << "�ʷ�����ʧ��" << endl;
        return 1;
    }

    Parser parser(tokens);
    if (TreeNode* syntaxTree = parser.parse()) {
        print_tree(syntaxTree);
    } else {
        cout << "�﷨����ʧ��" << endl;
    }

    return 0;
}
