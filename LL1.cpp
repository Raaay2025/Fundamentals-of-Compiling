#include "bits/stdc++.h"

using namespace std;

map<string, map<string, vector<string>>> ll_table = {
    {"E", {{"i", {"T", "E'"}}, {"(", {"T", "E'"}}}},
    {"E'", {{"+", {"+", "T", "E'"}}, {")", {}}, {"#", {}}}},
    {"T", {{"i", {"F", "T'"}}, {"(", {"F", "T'"}}}},
    {"T'", {{"*", {"*", "F", "T'"}}, {"+", {}}, {")", {}}, {"#", {}}}},
    {"F", {{"i", {"i"}}, {"(", {"(", "E", ")"}}}}};

queue<string> parse_input(const string& input) {
    string input_str;
    for (char c : input) {
        if (c != ' ') input_str += c;
    }

    queue<string> q;
    int pos = 0;
    while (pos < input_str.size()) {
        if (input_str[pos] == '(') {
            pos++;
            string token;
            bool tag = false;
            // ��ȡһ��token
            while (pos < input_str.size()) {
                if (input_str[pos] == ')' && tag) break;
                token += input_str[pos];
                if (input_str[pos] == ',') tag = true;  // ��֤�ܶ�ȡ��һ������
                pos++;
            }
            if (pos >= input_str.size()) {
                cerr << "Error: Missing closing parenthesis or comma" << endl;
                return {};
            }
            pos++;

            // ����token
            int comma = token.find(',');
            if (comma != string::npos) {
                string type = token.substr(0, comma);
                string value = token.substr(comma + 1);
                if (type == "id") {        // ��ʶ��ͳһӳ��Ϊi
                    q.push("i");
                }
                else if (value.empty()) {  // ����ڶ�ԪΪ�յ����
                    q.push(type);
                }
                else if (type == "#") {    // ���������⴦��
                    q.push("#");
                }
            }
        }
    }
    return q;
}

vector<string> init_symbol_stack() {
    return {"#", "E"};
}

void print_step(int step, const vector<string>& stack, queue<string> input, const string& production) {
    string stack_str;
    for (const auto& s : stack) stack_str += s + " ";
    if (!stack_str.empty()) stack_str.pop_back();

    string input_str;
    while (!input.empty()) {
        input_str += input.front() + " ";
        input.pop();
    }
    if (!input_str.empty()) input_str.pop_back();

    cout << left << setw(5) << step
         << setw(25) << stack_str
         << setw(25) << input_str
         << production << endl;
}

void predictive_parser(queue<string> input_queue) {
    auto symbol_stack = init_symbol_stack();
    int step = 0;

    cout << left << setw(6) << "����"
         << setw(26) << "����ջ"
         << setw(25) << "���봮"
         << "���ò���ʽ" << endl;
    print_step(0, symbol_stack, input_queue, "��ʼ��");

    while (!input_queue.empty()) {
        step++;
        string top = symbol_stack.back();
        string current_input = input_queue.front();
        string production;

        if (ll_table.count(top)) {
            auto& row = ll_table[top];
            if (row.count(current_input)) {
                auto right = row[current_input];
                production = top + " �� ";
                if (right.empty()) {
                    production += "eps";
                } else {
                    for (const auto& s : right) production += s + " ";
                    production.pop_back();
                }
                symbol_stack.pop_back();
                reverse(right.begin(), right.end());
                for (const auto& s : right) {
                    if (!s.empty()) symbol_stack.push_back(s);
                }
            } else {
                cout << "����: ����" << step << " �޲���ʽ: " << top << " ���� " << current_input << endl;
                return;
            }
        } else {
            if (top == current_input) {
                production = "ƥ�� '" + top + "'";
                symbol_stack.pop_back();
                input_queue.pop();
            } else {
                cout << "����: ����" << step << " ջ��'" << top << "' ������'" << current_input << "'��ƥ��" << endl;
                return;
            }
        }

        print_step(step, symbol_stack, input_queue, production);

        if (symbol_stack.size() == 1 && symbol_stack[0] == "#" && input_queue.front() == "#")
            break;
    }
}

int main() {
    //  string input = "(id,i1) (*, ) (id,i2) (+, ) (id,i3) (#, )";

    string input = "(id,i1) (*, ) (id,i2) (+, ) ((,) (id,i3) (+, ) (id,i4) (),) (#, )";

    auto input_queue = parse_input(input);

    cout << "����������봮: ";
    queue<string> debug = input_queue;
    while (!debug.empty()) {
        cout << debug.front() << " ";
        debug.pop();
    }
    cout << endl << endl;

    predictive_parser(input_queue);
    return 0;
}
