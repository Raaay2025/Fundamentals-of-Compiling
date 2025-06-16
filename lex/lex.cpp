#include "bits/stdc++.h"
#include "fstream"

using namespace std;

#define MAX_STATES 100
#define MAX_TRANSITIONS 100
#define MAX_TOKEN_LEN 100

// NFA状态结构体
typedef struct {
    int id;
} State;

// NFA转移结构体
typedef struct {
    int start;
    int end;
    char symbol; // '~' 表示空转移
} Transition;

// NFA结构体
typedef struct {
    State states[MAX_STATES];
    Transition transitions[MAX_TRANSITIONS];
    int state_count;
    int transition_count;
    int start_state;
    int accept_state;
} NFA;

// DFA相关数据结构
int numof_Dtran = 0;
char Dtran[100][100];
int useof_DFA[100];
int numof_char = 0;
int useof_char[256];
char alpha[100];

struct DFA_State {
    int H[100];
    int count;
    int flag;
    int mark;
};

DFA_State DFA[100];

// 词法分析器数据结构
typedef struct {
    char token[MAX_TOKEN_LEN];
    int type;
} Token;

// 创建新状态
State create_state(int id) {
    State s;
    s.id = id;
    return s;
}

// 创建新转移
Transition create_transition(int start, int end, char symbol) {
    Transition t;
    t.start = start;
    t.end = end;
    t.symbol = symbol;
    return t;
}

// 初始化NFA
void init_nfa(NFA *nfa) {
    nfa->state_count = 0;
    nfa->transition_count = 0;
    nfa->start_state = -1;
    nfa->accept_state = -1;
}

// 添加状态到NFA
int add_state(NFA *nfa, State state) {
    if (nfa->state_count < MAX_STATES) {
        nfa->states[nfa->state_count] = state;
        return nfa->state_count++;
    }
    return -1;
}

// 添加转移到NFA
void add_transition(NFA *nfa, Transition transition) {
    if (nfa->transition_count < MAX_TRANSITIONS) {
        nfa->transitions[nfa->transition_count++] = transition;

        if (transition.symbol != '~' && !useof_char[transition.symbol]) {
            alpha[numof_char++] = transition.symbol;
            useof_char[transition.symbol] = 1;
        }
    }
}

// 检查括号是否匹配
bool is_balanced(const char *reg, int start, int end) {
    int balance = 0;
    for (int i = start; i <= end; i++) {
        if (reg[i] == '(') balance++;
        else if (reg[i] == ')') balance--;

        if (balance < 0) return false;
    }
    return balance == 0;
}

// 查找最低优先级的操作符位置
int find_lowest_op(const char *reg, int start, int end) {
    int pos = -1;
    int min_priority = 3;
    int balance = 0;

    for (int i = start; i <= end; i++) {
        if (reg[i] == '(') balance++;
        else if (reg[i] == ')') balance--;

        if (balance == 0) {
            int priority = 0;
            if (reg[i] == '|') {
                priority = 1;
            } else if (reg[i] == '.') {
                priority = 2;
            } else if (reg[i] == '*') {
                priority = 3;
            }

            if (priority > 0 && priority <= min_priority) {
                min_priority = priority;
                pos = i;
            }
        }
    }

    return pos;
}

// 递归构建NFA
void reg2nfa(NFA *nfa, const char *reg, int start, int end, int begin_state, int end_state) {
    while (start <= end && reg[start] == '(' && reg[end] == ')' && is_balanced(reg, start + 1, end - 1)) {
        start++;
        end--;
    }

    if (start == end && ((reg[start] >= 'a' && reg[start] <= 'z') ||
                         (reg[start] >= 'A' && reg[start] <= 'Z') ||
                         (reg[start] >= '0' && reg[start] <= '9'))) {
        add_transition(nfa, create_transition(begin_state, end_state, reg[start]));
        return;
    }

    int op_pos = find_lowest_op(reg, start, end);
    if (op_pos == -1) {
        printf("Error: Invalid regular expression\n");
        exit(1);
    }

    char op = reg[op_pos];

    if (op == '|') {
        int begin_b = add_state(nfa, create_state(nfa->state_count));
        int end_b = add_state(nfa, create_state(nfa->state_count));
        int begin_c = add_state(nfa, create_state(nfa->state_count));
        int end_c = add_state(nfa, create_state(nfa->state_count));

        add_transition(nfa, create_transition(begin_state, begin_b, '~'));
        add_transition(nfa, create_transition(begin_state, begin_c, '~'));
        add_transition(nfa, create_transition(end_b, end_state, '~'));
        add_transition(nfa, create_transition(end_c, end_state, '~'));

        reg2nfa(nfa, reg, start, op_pos - 1, begin_b, end_b);
        reg2nfa(nfa, reg, op_pos + 1, end, begin_c, end_c);
    } else if (op == '.') {
        int middle_state = add_state(nfa, create_state(nfa->state_count));

        reg2nfa(nfa, reg, start, op_pos - 1, begin_state, middle_state);
        reg2nfa(nfa, reg, op_pos + 1, end, middle_state, end_state);
    } else if (op == '*') {
        int begin_b = add_state(nfa, create_state(nfa->state_count));
        int end_b = add_state(nfa, create_state(nfa->state_count));

        add_transition(nfa, create_transition(begin_state, end_state, '~'));
        add_transition(nfa, create_transition(begin_state, begin_b, '~'));
        add_transition(nfa, create_transition(end_b, begin_b, '~'));
        add_transition(nfa, create_transition(end_b, end_state, '~'));

        reg2nfa(nfa, reg, start, op_pos - 1, begin_b, end_b);
    } else {
        printf("Error: Unknown operator '%c'\n", op);
        exit(1);
    }
}

// 从正则表达式构建NFA
NFA regex_to_nfa(const char *regex) {
    NFA nfa;
    init_nfa(&nfa);

    nfa.start_state = add_state(&nfa, create_state(0));
    nfa.accept_state = add_state(&nfa, create_state(1));

    reg2nfa(&nfa, regex, 0, strlen(regex) - 1, nfa.start_state, nfa.accept_state);

    return nfa;
}

// 打印NFA
void print_nfa(NFA *nfa) {
    printf("NFA States: ");
    for (int i = 0; i < nfa->state_count; i++) {
        printf("%d ", nfa->states[i].id);
    }
    printf("\nStart State: %d\nAccept State: %d\n", nfa->start_state, nfa->accept_state);

    printf("Transitions:\n");
    for (int i = 0; i < nfa->transition_count; i++) {
        printf("%d --%c--> %d\n",
               nfa->transitions[i].start,
               nfa->transitions[i].symbol,
               nfa->transitions[i].end);
    }

    printf("Alphabet: ");
    for (int i = 0; i < numof_char; i++) {
        printf("%c ", alpha[i]);
    }
    printf("\n");
}

// 状态t通过不定个~字符到达的状态
void arriveByone(int t, int result[], int &num, NFA *nfa) {
    num = 0;
    stack<int> S;
    S.push(t);

    while (!S.empty()) {
        int j = S.top();
        S.pop();

        for (int m = 0; m < nfa->transition_count; m++) {
            if (nfa->transitions[m].start == j && nfa->transitions[m].symbol == '~') {
                bool found = false;
                for (int k = 0; k < num; k++) {
                    if (result[k] == nfa->transitions[m].end) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    result[num++] = nfa->transitions[m].end;
                    S.push(nfa->transitions[m].end);
                }
            }
        }
    }
}

// 判断状态i是否在状态集T中
bool check(int i, DFA_State T) {
    for (int j = 0; j < T.count; j++) {
        if (T.H[j] == i)
            return true;
    }
    return false;
}

// 计算状态的ε闭包
DFA_State closure(DFA_State T, NFA *nfa) {
    stack<int> STACK;
    DFA_State temp;
    temp.count = T.count;
    temp.mark = T.mark;
    temp.flag = -1;

    for (int i = 0; i < T.count; i++) {
        STACK.push(T.H[i]);
        temp.H[i] = T.H[i];
    }

    while (!STACK.empty()) {
        int t = STACK.top();
        STACK.pop();

        int search_result[100];
        int num;
        arriveByone(t, search_result, num, nfa);

        for (int j = 0; j < num; j++) {
            if (!check(search_result[j], temp)) {
                temp.H[temp.count++] = search_result[j];
                STACK.push(search_result[j]);
            }
        }
    }

    for (int k = 0; k < temp.count; k++) {
        if (nfa->accept_state == temp.H[k]) {
            temp.flag = 1;
            break;
        }
        if (nfa->start_state == temp.H[k]) {
            temp.flag = 0;
        }
    }

    sort(temp.H, temp.H + temp.count);
    for (int i = 0; i < numof_Dtran; i++) {
        if (temp.count != DFA[i].count)
            continue;

        sort(DFA[i].H, DFA[i].H + DFA[i].count);
        bool same = true;
        for (int j = 0; j < DFA[i].count; j++) {
            if (DFA[i].H[j] != temp.H[j]) {
                same = false;
                break;
            }
        }
        if (same) {
            temp.mark = DFA[i].mark;
            break;
        }
    }

    return temp;
}

// 状态集T在输入符号s下的转移
DFA_State move(DFA_State T, char s, NFA *nfa) {
    DFA_State temp;
    temp.count = 0;
    temp.mark = T.mark;

    for (int i = 0; i < T.count; i++) {
        for (int j = 0; j < nfa->transition_count; j++) {
            if (nfa->transitions[j].start == T.H[i] && nfa->transitions[j].symbol == s) {
                bool found = false;
                for (int k = 0; k < temp.count; k++) {
                    if (temp.H[k] == nfa->transitions[j].end) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    temp.H[temp.count++] = nfa->transitions[j].end;
                }
            }
        }
    }

    return temp;
}

// 检查DFA中是否存在未被标记的状态
int check_inDFA() {
    for (int i = 0; i < numof_Dtran; i++) {
        if (!useof_DFA[i])
            return i;
    }
    return -1;
}

// 检查状态T是否已经在DFA状态集中
bool check_whetherin_DFA(DFA_State T) {
    sort(T.H, T.H + T.count);
    for (int i = 0; i < numof_Dtran; i++) {
        if (T.count != DFA[i].count)
            continue;

        sort(DFA[i].H, DFA[i].H + DFA[i].count);
        bool same = true;
        for (int j = 0; j < DFA[i].count; j++) {
            if (DFA[i].H[j] != T.H[j]) {
                same = false;
                break;
            }
        }
        if (same)
            return true;
    }
    return false;
}

// 子集构造法将NFA转换为DFA
void nfa_to_dfa(NFA *nfa) {
    for (int m = 0; m < 100; m++) {
        for (int n = 0; n < 100; n++) {
            Dtran[m][n] = '#';
        }
    }

    for (int m = 0; m < 100; m++)
        DFA[m].flag = -1;

    DFA_State S0;
    S0.flag = 0;
    S0.count = 1;
    S0.H[0] = nfa->start_state;

    DFA_State T = closure(S0, nfa);
    T.mark = 0;
    T.flag = 0;
    DFA[numof_Dtran++] = T;

    memset(useof_DFA, 0, sizeof(useof_DFA));

    int j = check_inDFA();
    while (j != -1) {
        useof_DFA[j] = 1;

        for (int k = 0; k < numof_char; k++) {
            DFA_State g = move(DFA[j], alpha[k], nfa);
            DFA_State U = closure(g, nfa);

            if (U.count > 0) {
                if (!check_whetherin_DFA(U)) {
                    U.mark = numof_Dtran;
                    DFA[numof_Dtran] = U;
                    numof_Dtran++;
                }

                int actual_mark = U.mark;
                for (int i = 0; i < numof_Dtran; i++) {
                    bool same = true;
                    if (DFA[i].count != U.count) continue;

                    for (int m = 0; m < DFA[i].count; m++) {
                        if (DFA[i].H[m] != U.H[m]) {
                            same = false;
                        }
                    }

                    if (same) {
                        actual_mark = DFA[i].mark;
                        break;
                    }
                }

                Dtran[DFA[j].mark][actual_mark] = alpha[k];
            }
        }

        j = check_inDFA();
    }
}

// 打印DFA
void print_dfa() {
    printf("\nDFA States:\n");
    for (int i = 0; i < numof_Dtran; i++) {
        printf("%d: {", DFA[i].mark);
        for (int j = 0; j < DFA[i].count; j++) {
            printf("%d", DFA[i].H[j]);
            if (j < DFA[i].count - 1) printf(", ");
        }
        printf("}");

        if (DFA[i].flag == 1)
            printf(" (Accepting)");
        else if (DFA[i].flag == 0)
            printf(" (Starting)");

        printf("\n");
    }

    printf("\nDFA Transition Table:\n");
    for (int i = 0; i < numof_Dtran; i++) {
        for (int j = 0; j < numof_Dtran; j++) {
            if (Dtran[i][j] != '#') {
                printf("%d -> %d on '%c'\n", i, j, Dtran[i][j]);
            }
        }
    }
}

// 词法分析器生成
void generate_scanner_cpp() {
    ofstream out("../lex/scanner.cpp");

    out << "#include <iostream>\n";
    out << "#include <cstring>\n";
    out << "#include <cstdlib>\n";

    out << "#define MAX_TOKEN_LEN 100\n\n";

    out << "typedef struct {\n";
    out << "    char token[MAX_TOKEN_LEN];\n";
    out << "    int type;\n";
    out << "} Token;\n\n";

    out << "struct DFA_State {\n";
    out << "    int H[100];\n";
    out << "    int count;\n";
    out << "    int flag;\n";
    out << "    int mark;\n";
    out << "};\n\n";

    out << "int numof_Dtran = " << numof_Dtran << ";\n";
    out << "char Dtran[100][100] = {\n";
    for (int i = 0; i < numof_Dtran; ++i) {
        out << "    {";
        for (int j = 0; j < numof_Dtran; ++j) {
            if (j != 0) out << ", ";
            out << "'" << Dtran[i][j] << "'";
        }
        out << "},\n";
    }
    out << "};\n\n";

    out << "DFA_State DFA[100] = {\n";
    for (int i = 0; i < numof_Dtran; ++i) {
        out << "    { {";
        for (int j = 0; j < DFA[i].count; ++j) {
            if (j != 0) out << ", ";
            out << DFA[i].H[j];
        }
        out << "}, " << DFA[i].count << ", " << DFA[i].flag << ", " << DFA[i].mark << " },\n";
    }
    out << "};\n\n";

    out << "char alpha[] = {";
    for (int i = 0; i < numof_char; ++i) {
        if (i != 0) out << ", ";
        out << "'" << alpha[i] << "'";
    }
    out << "};\n";
    out << "int numof_char = " << numof_char << ";\n\n";

    out << "Token* lexer(const char* input, int* token_count) {\n";
    out << "    int max_tokens = strlen(input) + 1;\n";
    out << "    Token* tokens = (Token*)malloc(max_tokens * sizeof(Token));\n";
    out << "    *token_count = 0;\n\n";
    out << "    int current_state = 0;\n";
    out << "    int start_pos = 0;\n";
    out << "    int current_pos = 0;\n";
    out << "    int last_accept_pos = -1;\n";
    out << "    int last_accept_state = -1;\n\n";
    out << "    while (input[current_pos] != '\\0') {\n";
    out << "        char current_char = input[current_pos];\n";
    out << "        int next_state = -1;\n\n";
    out << "        for (int i = 0; i < numof_Dtran; i++) {\n";
    out << "            if (Dtran[current_state][i] == current_char) {\n";
    out << "                next_state = i;\n";
    out << "                break;\n";
    out << "            }\n";
    out << "        }\n\n";
    out << "        if (next_state != -1) {\n";
    out << "            current_state = next_state;\n";
    out << "            current_pos++;\n\n";
    out << "            if (DFA[current_state].flag == 1) {\n";
    out << "                last_accept_pos = current_pos;\n";
    out << "                last_accept_state = current_state;\n";
    out << "            }\n";
    out << "        } else {\n";
    out << "            if (last_accept_pos != -1) {\n";
    out << "                int token_len = last_accept_pos - start_pos;\n";
    out << "                strncpy(tokens[*token_count].token, input + start_pos, token_len);\n";
    out << "                tokens[*token_count].token[token_len] = '\\0';\n";
    out << "                tokens[*token_count].type = last_accept_state;\n";
    out << "                (*token_count)++;\n\n";
    out << "                start_pos = last_accept_pos;\n";
    out << "                current_pos = last_accept_pos;\n";
    out << "                current_state = 0;\n";
    out << "                last_accept_pos = -1;\n";
    out << "                last_accept_state = -1;\n";
    out << "            } else {\n";
    out << "                std::cout << \"Error: Unrecognized character '\" \n";
    out << "                          << current_char << \"' at position \" \n";
    out << "                          << current_pos << std::endl;\n";
    out << "                current_pos++;\n";
    out << "                start_pos = current_pos;\n";
    out << "                current_state = 0;\n";
    out << "            }\n";
    out << "        }\n";
    out << "    }\n\n";
    out << "    if (last_accept_pos != -1) {\n";
    out << "        int token_len = last_accept_pos - start_pos;\n";
    out << "        strncpy(tokens[*token_count].token, input + start_pos, token_len);\n";
    out << "        tokens[*token_count].token[token_len] = '\\0';\n";
    out << "        tokens[*token_count].type = last_accept_state;\n";
    out << "        (*token_count)++;\n";
    out << "    }\n\n";
    out << "    return tokens;\n";
    out << "}\n\n";

    // 生成main函数
    out << "int main() {\n";
    out << "    std::string input;\n";
    out << "    std::cout << \"Enter input string: \";\n";
    out << "    std::getline(std::cin, input);\n\n";
    out << "    int token_count;\n";
    out << "    Token* tokens = lexer(input.c_str(), &token_count);\n\n";
    out << "    std::cout << \"\\nLexical Analysis Results:\\n\";\n";
    out << "    for (int i = 0; i < token_count; i++) {\n";
    out << "        std::cout << \"Token \" << i+1 << \": '\" << tokens[i].token \n";
    out << "                  << \"' (Type: \" << tokens[i].type << \")\\n\";\n";
    out << "    }\n\n";
    out << "    free(tokens);\n";
    out << "    return 0;\n";
    out << "}\n";

    out.close();
    std::cout << "Scanner generated to scanner.cpp\n";
}

int main() {
    // 从文件读取正则表达式
    ifstream fin("../lex/regex.txt");
    if (!fin) {
        cerr << "Error: Could not open regex.txt" << endl;
        return 1;
    }

    string regex;
    getline(fin, regex);
    fin.close();
    cout << "Converting regular expression: " << regex << '\n';

    // 初始化全局变量
    memset(useof_char, 0, sizeof(useof_char));
    memset(useof_DFA, 0, sizeof(useof_DFA));
    numof_char = 0;
    numof_Dtran = 0;

    // 构建NFA并转换为DFA
    NFA nfa = regex_to_nfa(regex.c_str());
    print_nfa(&nfa);
    nfa_to_dfa(&nfa);
    print_dfa();

    // 生成扫描器代码
    generate_scanner_cpp();

    return 0;
}

