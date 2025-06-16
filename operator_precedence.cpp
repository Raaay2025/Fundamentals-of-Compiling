#include "bits/stdc++.h"

using namespace std;

// ������������ȹ�ϵ������
// �洢��������ȼ���ϵ: <, =, >
typedef map<char, map<char, char> > PrecedenceTable;

class OperatorPrecedenceParser {
private:
    string input; // ���봮
    string st; // ����ջ
    PrecedenceTable table; // ��������ȱ�
    int step = 1; // ��ǰ����

    // ��ʼ����������ȱ�
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

    // ����ַ��Ƿ�Ϊ�ս��
    bool isTerminal(char c) {
        return c == '+' || c == '*' || c == '^' || c == 'i' || c == '(' || c == ')' || c == '#';
    }

    // ��ȡ�����ض����λ��
    int getReductionPos(int j) {
        // ��j��ʼ������������ض������ʼλ��
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

    // ��Լ�Ӵ�Ϊ���ս��
    char reduce(string substring) {
        // �����ķ������Լ
        // ��Ȼ�ڹ�Լ�����в����ַ��ս��������Լ��ʹ����ȷ�ķ��ս������

        // E��E + T | T
        if (substring.length() == 3 && substring[1] == '+') {
            return 'E';
        }

        // T��T * F | F
        else if (substring.length() == 3 && substring[1] == '*') {
            return 'T';
        }

        // F��P �� F | P
        else if (substring.length() == 3 && substring[1] == '^') {
            return 'F';
        }

        // P��( E ) | i
        else if (substring.length() == 3 && substring[0] == '(' && substring[2] == ')') {
            return 'P';
        } else if (substring == "i") {
            return 'P';
        }

        // �������ս�����������Ϊ����
        else if (substring.length() == 1 && !isTerminal(substring[0])) {
            // �����ķ������������
            if (substring[0] == 'P') return 'F';
            else if (substring[0] == 'F') return 'T';
            else if (substring[0] == 'T') return 'E';
            else return substring[0]; // ����ԭ��
        }

        // �޷�ʶ������������X��ʾ����
        return 'X';
    }

    // ��ӡ��ǰ����
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
        st = "#"; // ��ʼ��ջ
    }

    // ִ�з�������
    void parse() {
        cout << "���� ջ                ���봮             ������ȹ�ϵ     ����" << endl;

        string inputRemain = input;
        int pos = 0;
        char nextAction = 'S'; // S:�ƽ�(Shift), R:��Լ(Reduce)

        while (true) {
            // ��ȡ��һ�������ַ�
            char a = input[pos];

            if (nextAction == 'S') {
                // �ƽ�����
                // ��ȡջ���ս����λ��
                int j;
                if (isTerminal(st.back())) {
                    j = st.length() - 1;
                } else {
                    j = st.length() - 2;
                }

                // �Ƚ�ջ���ս���뵱ǰ������ŵ����ȼ�
                char relation = table[st[j]][a];

                if (relation == '<' || relation == '=') {
                    // ��ջ������С�ڻ�����������ʱ�������������ջ
                    string action;
                    if (a == 'i') {
                        action = "i��ջ";
                    } else {
                        action = string(1, a) + "��ջ";
                    }

                    printStep(inputRemain, action, relation);

                    st.push_back(a);
                    pos++; // �ƶ�����һ���������
                    inputRemain = input.substr(pos);

                    // ����Ƿ���Ҫ����һ�����й�Լ
                    if (pos < input.length()) {
                        char nextChar = input[pos];
                        int k;
                        if (isTerminal(st.back())) {
                            k = st.length() - 1;
                        } else {
                            k = st.length() - 2;
                        }

                        if (table[st[k]][nextChar] == '>') {
                            nextAction = 'R'; // ��һ�����й�Լ
                        }
                    }
                } else if (relation == '>') {
                    nextAction = 'R'; // ��Ҫ���й�Լ
                } else {
                    // ������
                    cout << "Error: �﷨����" << endl;
                    return;
                }
            } else if (nextAction == 'R') {
                // ��Լ����
                int j;
                if (isTerminal(st.back())) {
                    j = st.length() - 1;
                } else {
                    j = st.length() - 2;
                }

                // ���������ض���
                int reductionPos = getReductionPos(j);

                // ��ȡҪ��Լ���Ӵ�
                string substring = st.substr(reductionPos + 1);

                // ��ȡջ���ս������һ�������ַ��Ĺ�ϵ
                char relation = ' ';
                if (pos < input.length()) {
                    relation = table[st[j]][input[pos]];
                }

                // ȷ����Լ�Ķ�������
                string action;
                if (substring == "i") {
                    action = "i��Լ";
                } else if (substring.length() == 1) {
                    if (substring[0] == 'P') action = "P��Լ";
                    else if (substring[0] == 'F') action = "F��Լ";
                    else if (substring[0] == 'T') action = "T��Լ";
                    else action = substring + "��Լ";
                } else if (substring.length() == 3 && substring[1] == '+') {
                    action = substring + "��Լ";
                } else if (substring.length() == 3 && substring[1] == '*') {
                    action = substring + "��Լ";
                } else if (substring.length() == 3 && substring[1] == '^') {
                    action = substring + "��Լ";
                } else if (substring.length() == 3 && substring[0] == '(' && substring[2] == ')') {
                    action = substring + "��Լ";
                } else {
                    action = substring + "��Լ";
                }

                // ��Լǰ�ȴ�ӡ
                printStep(inputRemain, action, relation);

                // ִ�й�Լ����
                char N = reduce(substring);
                st = st.substr(0, reductionPos + 1) + N;

                // ��Լ������һ������
                if (pos < input.length()) {
                    char nextChar = input[pos];
                    int k;
                    if (isTerminal(st.back())) {
                        k = st.length() - 1;
                    } else {
                        k = st.length() - 2;
                    }

                    if (table[st[k]][nextChar] == '<' || table[st[k]][nextChar] == '=') {
                        nextAction = 'S'; // ��һ�������ƽ�
                    } else {
                        nextAction = 'R'; // ������Լ
                    }
                } else if (st == "#E" || st == "#T" || st == "#F" || st == "#P") {
                    // �ѵ��������������Ƿ����
                    printStep("#", "����");
                    break;
                }
            }

            // ����Ƿ������� (���ǵ����ܲ���Ҫ��Լ��ֱ����ɵ����)
            if ((st == "#E" || st == "#T" || st == "#F" || st == "#P") && inputRemain == "#" && nextAction == 'S') {
                printStep("#", "����");
                break;
            }
        }
    }
};

// Ԥ���������ַ����������﷨�ṹ
string preprocessInput(const string &input) {
    string result;
    for (size_t i = 0; i < input.length(); i++) {
        if (input[i] == 'i') {
            result.push_back('i');
            // ���������ŵ�����
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
    // Ԥ�������룬����ʶ��i1, i2, i3�ȼ�Ϊi���������﷨�ṹ
    string processedInput = preprocessInput(input);

    cout << "ԭʼ����: " << input << endl;
    cout << "����������: " << processedInput << endl << endl;

    OperatorPrecedenceParser parser(processedInput);
    parser.parse();

    return 0;
}
