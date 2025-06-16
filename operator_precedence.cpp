#include "bits/stdc++.h"

using namespace std;

// 定义运算符优先关系表类型
// 存储运算符优先级关系: <, =, >
typedef map<char, map<char, char> > PrecedenceTable;

class OperatorPrecedenceParser {
private:
    string input; // 输入串
    string st; // 分析栈
    PrecedenceTable table; // 运算符优先表
    int step = 1; // 当前步骤

    // 初始化运算符优先表
    void initPrecedenceTable() {
        table['+']['+'] = '>';
        table['+']['*'] = '<';
        table['+']['^'] = '<';
        table['+']['i'] = '<';
        table['+']['('] = '<';
        table['+'][')'] = '>';
        table['+']['#'] = '>';

        table['*']['+'] = '>';
        table['*']['*'] = '>';
        table['*']['^'] = '<';
        table['*']['i'] = '<';
        table['*']['('] = '<';
        table['*'][')'] = '>';
        table['*']['#'] = '>';

        table['^']['+'] = '>';
        table['^']['*'] = '>';
        table['^']['^'] = '<';
        table['^']['i'] = '<';
        table['^']['('] = '<';
        table['^'][')'] = '>';
        table['^']['#'] = '>';

        table['i']['+'] = '>';
        table['i']['*'] = '>';
        table['i']['^'] = '>';
        table['i'][')'] = '>';
        table['i']['#'] = '>';

        table['(']['+'] = '<';
        table['(']['*'] = '<';
        table['(']['^'] = '<';
        table['(']['i'] = '<';
        table['(']['('] = '<';
        table['('][')'] = '=';

        table[')']['+'] = '>';
        table[')']['*'] = '>';
        table[')']['^'] = '>';
        table[')'][')'] = '>';
        table[')']['#'] = '>';

        table['#']['+'] = '<';
        table['#']['*'] = '<';
        table['#']['^'] = '<';
        table['#']['i'] = '<';
        table['#']['('] = '<';
        table['#']['#'] = '=';
    }

    // 检查字符是否为终结符
    bool isTerminal(char c) {
        return c == '+' || c == '*' || c == '^' || c == 'i' || c == '(' || c == ')' || c == '#';
    }

    // 获取最左素短语的位置
    int getReductionPos(int j) {
        // 从j开始向左查找最左素短语的起始位置
        char topSymbol = st[j];
        if (isTerminal(st[j - 1])) {
            j = j - 1;
        } else {
            j = j - 2;
        }
        while (table[st[j]][topSymbol] != '<') {
            topSymbol = st[j];
            if (isTerminal(st[j - 1])) {
                j = j - 1;
            } else {
                j = j - 2;
            }
        }
        return j;
    }

    // 归约子串为非终结符
    char reduce(string substring) {
        // 根据文法规则归约
        // 虽然在归约过程中不区分非终结符，但归约后使用正确的非终结符符号

        // E→E + T | T
        if (substring.length() == 3 && substring[1] == '+') {
            return 'E';
        }

        // T→T * F | F
        else if (substring.length() == 3 && substring[1] == '*') {
            return 'T';
        }

        // F→P ↑ F | P
        else if (substring.length() == 3 && substring[1] == '^') {
            return 'F';
        }

        // P→( E ) | i
        else if (substring.length() == 3 && substring[0] == '(' && substring[2] == ')') {
            return 'P';
        } else if (substring == "i") {
            return 'P';
        }

        // 单个非终结符的情况：视为自身
        else if (substring.length() == 1 && !isTerminal(substring[0])) {
            // 根据文法规则进行提升
            if (substring[0] == 'P') return 'F';
            else if (substring[0] == 'F') return 'T';
            else if (substring[0] == 'T') return 'E';
            else return substring[0]; // 保持原样
        }

        // 无法识别的情况，返回X表示错误
        return 'X';
    }

    // 打印当前步骤
    void printStep(const string &inputRemain, const string &action, char relation = ' ') {
        cout << setw(3) << step << ")  " << setw(15) << left << st << setw(20) << left << inputRemain;

        if (relation != ' ') {
            string relationStr;
            relationStr += st.back();
            relationStr += relation;
            relationStr += inputRemain[0];
            cout << setw(15) << left << relationStr;
        } else {
            cout << setw(15) << " ";
        }

        cout << action << endl;
        step++;
    }

public:
    OperatorPrecedenceParser(const string &input) : input(input) {
        initPrecedenceTable();
        st = "#"; // 初始化栈
    }

    // 执行分析过程
    void parse() {
        cout << "步骤 栈                输入串             算符优先关系     动作" << endl;

        string inputRemain = input;
        int pos = 0;
        char nextAction = 'S'; // S:移进(Shift), R:归约(Reduce)

        while (true) {
            // 获取下一个输入字符
            char a = input[pos];

            if (nextAction == 'S') {
                // 移进操作
                // 获取栈顶终结符的位置
                int j;
                if (isTerminal(st.back())) {
                    j = st.length() - 1;
                } else {
                    j = st.length() - 2;
                }

                // 比较栈顶终结符与当前输入符号的优先级
                char relation = table[st[j]][a];

                if (relation == '<' || relation == '=') {
                    // 当栈顶符号小于或等于输入符号时，将输入符号入栈
                    string action;
                    if (a == 'i') {
                        action = "i入栈";
                    } else {
                        action = string(1, a) + "入栈";
                    }

                    printStep(inputRemain, action, relation);

                    st.push_back(a);
                    pos++; // 移动到下一个输入符号
                    inputRemain = input.substr(pos);

                    // 检查是否需要在下一步进行归约
                    if (pos < input.length()) {
                        char nextChar = input[pos];
                        int k;
                        if (isTerminal(st.back())) {
                            k = st.length() - 1;
                        } else {
                            k = st.length() - 2;
                        }

                        if (table[st[k]][nextChar] == '>') {
                            nextAction = 'R'; // 下一步进行归约
                        }
                    }
                } else if (relation == '>') {
                    nextAction = 'R'; // 需要进行归约
                } else {
                    // 错误处理
                    cout << "Error: 语法错误！" << endl;
                    return;
                }
            } else if (nextAction == 'R') {
                // 归约操作
                int j;
                if (isTerminal(st.back())) {
                    j = st.length() - 1;
                } else {
                    j = st.length() - 2;
                }

                // 查找最左素短语
                int reductionPos = getReductionPos(j);

                // 获取要归约的子串
                string substring = st.substr(reductionPos + 1);

                // 获取栈顶终结符与下一个输入字符的关系
                char relation = ' ';
                if (pos < input.length()) {
                    relation = table[st[j]][input[pos]];
                }

                // 确定归约的动作描述
                string action;
                if (substring == "i") {
                    action = "i规约";
                } else if (substring.length() == 1) {
                    if (substring[0] == 'P') action = "P规约";
                    else if (substring[0] == 'F') action = "F规约";
                    else if (substring[0] == 'T') action = "T规约";
                    else action = substring + "规约";
                } else if (substring.length() == 3 && substring[1] == '+') {
                    action = substring + "规约";
                } else if (substring.length() == 3 && substring[1] == '*') {
                    action = substring + "规约";
                } else if (substring.length() == 3 && substring[1] == '^') {
                    action = substring + "规约";
                } else if (substring.length() == 3 && substring[0] == '(' && substring[2] == ')') {
                    action = substring + "规约";
                } else {
                    action = substring + "规约";
                }

                // 归约前先打印
                printStep(inputRemain, action, relation);

                // 执行归约操作
                char N = reduce(substring);
                st = st.substr(0, reductionPos + 1) + N;

                // 归约后检查下一个动作
                if (pos < input.length()) {
                    char nextChar = input[pos];
                    int k;
                    if (isTerminal(st.back())) {
                        k = st.length() - 1;
                    } else {
                        k = st.length() - 2;
                    }

                    if (table[st[k]][nextChar] == '<' || table[st[k]][nextChar] == '=') {
                        nextAction = 'S'; // 下一步进行移进
                    } else {
                        nextAction = 'R'; // 继续归约
                    }
                } else if (st == "#E" || st == "#T" || st == "#F" || st == "#P") {
                    // 已到输入结束，检查是否接受
                    printStep("#", "接受");
                    break;
                }
            }

            // 检查是否分析完成 (考虑到可能不需要规约就直接完成的情况)
            if ((st == "#E" || st == "#T" || st == "#F" || st == "#P") && inputRemain == "#" && nextAction == 'S') {
                printStep("#", "接受");
                break;
            }
        }
    }
};

// 预处理输入字符串，保留语法结构
string preprocessInput(const string &input) {
    string result;
    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] == 'i') {
            result.push_back('i');
            // 跳过紧跟着的数字
            while (i + 1 < input.length() && isdigit(input[i + 1])) {
                i++;
            }
        } else if (!isspace(input[i])) {
            result.push_back(input[i]);
        }
    }
    return result;
}

int main() {
    // string input = "i1 *( i2 + i3) # ";

    string input = "i1 + i2 * i3 + i4 # ";
    // 预处理输入，将标识符i1, i2, i3等简化为i，但保留语法结构
    string processedInput = preprocessInput(input);

    cout << "原始输入: " << input << endl;
    cout << "处理后的输入: " << processedInput << endl << endl;

    OperatorPrecedenceParser parser(processedInput);
    parser.parse();

    return 0;
}
