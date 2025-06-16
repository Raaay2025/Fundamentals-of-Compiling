#include "bits/stdc++.h"

using namespace std;

// Token���Ͷ���
enum class TokenType {
    ID, // ��ʶ��
    ADD, // +
    MUL, // *
    LPAREN, // (
    RPAREN, // )
    END, // #
    UNKNOWN // δ֪����
};

// Token�ṹ
struct Token {
    TokenType type;
    string value;
};

// �ʷ�������
vector<Token> tokenize(const string &input) {
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
            bool tag = false;
            while (pos < input.size()) {
                if (input[pos] == ')' && tag) break;
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
            string value = (comma != string::npos) ? content.substr(comma + 1) : "";

            Token token;
            if (typeStr == "i") token.type = TokenType::ID;
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
        } else {
            cerr << "Unexpected character: " << input[pos] << endl;
            return {};
        }
    }
    return tokens;
}

// SLR������
unordered_map<int, unordered_map<string, string> > action_table;
unordered_map<int, unordered_map<string, int> > goto_table;

// ��ʼ��SLR������
void initAnalysisTables() {
    // ��ʼ��ACTION��
    action_table[0]["i"] = "s5";
    action_table[0]["("] = "s4";

    action_table[1]["+"] = "s6";
    action_table[1]["#"] = "acc";

    action_table[2]["+"] = "rE->T";
    action_table[2]["*"] = "s7";
    action_table[2][")"] = "rE->T";
    action_table[2]["#"] = "rE->T";

    action_table[3]["+"] = "rT->F";
    action_table[3]["*"] = "rT->F";
    action_table[3][")"] = "rT->F";
    action_table[3]["#"] = "rT->F";

    action_table[4]["i"] = "s5";
    action_table[4]["("] = "s4";

    action_table[5]["+"] = "rF->i";
    action_table[5]["*"] = "rF->i";
    action_table[5][")"] = "rF->i";
    action_table[5]["#"] = "rF->i";

    action_table[6]["i"] = "s5";
    action_table[6]["("] = "s4";

    action_table[7]["i"] = "s5";
    action_table[7]["("] = "s4";

    action_table[8]["+"] = "s6";
    action_table[8][")"] = "s11";

    action_table[9]["+"] = "rE->E+T";
    action_table[9]["*"] = "s7";
    action_table[9][")"] = "rE->E+T";
    action_table[9]["#"] = "rE->E+T";

    action_table[10]["+"] = "rT->T*F";
    action_table[10]["*"] = "rT->T*F";
    action_table[10][")"] = "rT->T*F";
    action_table[10]["#"] = "rT->T*F";

    action_table[11]["+"] = "rF->(E)";
    action_table[11]["*"] = "rF->(E)";
    action_table[11][")"] = "rF->(E)";
    action_table[11]["#"] = "rF->(E)";

    // ��ʼ��GOTO��
    goto_table[0]["E"] = 1;
    goto_table[0]["T"] = 2;
    goto_table[0]["F"] = 3;

    goto_table[4]["E"] = 8;
    goto_table[4]["T"] = 2;
    goto_table[4]["F"] = 3;

    goto_table[6]["T"] = 9;
    goto_table[6]["F"] = 3;

    goto_table[7]["F"] = 10;
}

// ��Լ����ӳ��
struct Production {
    char lhs;
    int rhs_len;
};

unordered_map<string, Production> reduce_map = {
    {"rE->E+T", {'E', 3}},
    {"rE->T", {'E', 1}},
    {"rT->T*F", {'T', 3}},
    {"rT->F", {'T', 1}},
    {"rF->(E)", {'F', 3}},
    {"rF->i", {'F', 1}},
};

// Token���͵�����ת��
string tokenTypeToSymbol(TokenType type) {
    switch (type) {
        case TokenType::ID: return "i";
        case TokenType::ADD: return "+";
        case TokenType::MUL: return "*";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::END: return "#";
        default: return "";
    }
}

bool SLR(vector<Token> tokens) {
    vector<int> state_stack = {0};
    vector<string> symbol_stack = {"#"};
    int ptr = 0;
    int step = 1;

    // �����ͷ
    cout << left
            << setw(5) << "����"
            << setw(12) << "״̬ջ"
            << setw(14) << "����ջ"
            << setw(21) << "���봮"
            << "��������"
            << endl;

    while (true) {
        // ���쵱ǰ״̬��ʾ
        string state_str;
        for (int s: state_stack) state_str += to_string(s);

        string symbol_str;
        for (string s: symbol_stack) symbol_str += s;

        // ����ʣ������
        string input_str;
        for (int i = ptr; i < tokens.size(); ++i) {
            input_str += tokenTypeToSymbol(tokens[i].type);
        }

        // ��ȡ��ǰ״̬���������
        int current_state = state_stack.back();
        string current_symbol;
        string action_desc;

        if (ptr >= tokens.size()) {
            cerr << "Error: Unexpected end of input" << endl;
            break;
        }
        current_symbol = tokenTypeToSymbol(tokens[ptr].type);

        // ����ACTION��
        string action;
        if (action_table[current_state].find(current_symbol) != action_table[current_state].end()) {
            action = action_table[current_state][current_symbol];
        } else {
            action_desc = "ERROR: No action for [" + to_string(current_state) + ", " + current_symbol + "]";
            cout << left
                    << setw(4) << step
                    << setw(12) << state_str
                    << setw(12) << symbol_str
                    << setw(20) << input_str
                    << action_desc
                    << endl;
            break;
        }

        // ���춯������
        action_desc = "ACTION[" + to_string(current_state) + ", " + current_symbol + "] = " + action;

        if (action[0] == 's') {
            // �ƽ�
            int new_state = stoi(action.substr(1));
            symbol_stack.push_back(current_symbol);
            state_stack.push_back(new_state);
            ptr++;
        } else if (action[0] == 'r') {
            // ��Լ
            auto it = reduce_map.find(action);
            if (it == reduce_map.end()) {
                action_desc += " (Undefined reduction)";
                cout << left
                        << setw(4) << step
                        << setw(12) << state_str
                        << setw(12) << symbol_str
                        << setw(20) << input_str
                        << action_desc
                        << endl;
                break;
            }
            Production prod = it->second;

            // �������ź�״̬
            for (int i = 0; i < prod.rhs_len; ++i) {
                if (symbol_stack.empty() || state_stack.empty()) {
                    action_desc += " (Stack underflow)";
                    cout << left
                            << setw(4) << step
                            << setw(12) << state_str
                            << setw(12) << symbol_str
                            << setw(20) << input_str
                            << action_desc
                            << endl;
                    return false;
                }
                symbol_stack.pop_back();
                state_stack.pop_back();
            }

            // GOTO
            string new_symbol(1, prod.lhs);
            int goto_state = goto_table[state_stack.back()][new_symbol];
            action_desc += ", GOTO[" + to_string(state_stack.back()) + ", " + new_symbol + "] = " + to_string(
                goto_state);

            symbol_stack.push_back(new_symbol);
            state_stack.push_back(goto_state);
        } else if (action == "acc") {
            // ����
            action_desc = "Accept";
            cout << left
                    << setw(4) << step
                    << setw(12) << state_str
                    << setw(12) << symbol_str
                    << setw(20) << input_str
                    << action_desc
                    << endl;
            break;
        }

        // �����ǰ״̬
        cout << left
                << setw(4) << step++
                << setw(12) << state_str
                << setw(12) << symbol_str
                << setw(20) << input_str
                << action_desc
                << endl;

        if (action == "acc") break;
    }
    return true;
}

int main() {
     string input = "(i,a)(*,)(i,b)(+,)(i,c)(#,)";
     // string input = "(i,a)(*,)((,)(i,b)(+,)(i,c)(),)(+,)(i,d)(#,)";

    vector<Token> tokens = tokenize(input);
    if (tokens.empty()) {
        cerr << "Tokenization failed" << endl;
        return 1;
    }

    initAnalysisTables();

    bool sucess = SLR(tokens);
    if (!sucess) {
        cerr << "Parsing failed" << endl;
        return 1;
    }
    return 0;
}
