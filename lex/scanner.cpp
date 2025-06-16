#include <iostream>
#include <cstring>
#include <cstdlib>
#define MAX_TOKEN_LEN 100

typedef struct {
    char token[MAX_TOKEN_LEN];
    int type;
} Token;

struct DFA_State {
    int H[100];
    int count;
    int flag;
    int mark;
};

int numof_Dtran = 6;
char Dtran[100][100] = {
    {'#', 'a', 'b', 'c', '#', '#'},
    {'#', '#', '#', '#', 'b', '#'},
    {'#', '#', '#', '#', '#', 'c'},
    {'#', '#', '#', 'c', '#', '#'},
    {'#', '#', '#', '#', '#', '#'},
    {'#', '#', '#', '#', '#', '#'},
};

DFA_State DFA[100] = {
    { {0, 1, 2, 4, 5, 6, 8, 12}, 8, 0, 0 },
    { {10}, 1, -1, 1 },
    { {11}, 1, -1, 2 },
    { {1, 5, 12, 13}, 4, 1, 3 },
    { {1, 3, 7}, 3, 1, 4 },
    { {1, 3, 9}, 3, 1, 5 },
};

char alpha[] = {'a', 'b', 'c'};
int numof_char = 3;

Token* lexer(const char* input, int* token_count) {
    int max_tokens = strlen(input) + 1;
    Token* tokens = (Token*)malloc(max_tokens * sizeof(Token));
    *token_count = 0;

    int current_state = 0;
    int start_pos = 0;
    int current_pos = 0;
    int last_accept_pos = -1;
    int last_accept_state = -1;

    while (input[current_pos] != '\0') {
        char current_char = input[current_pos];
        int next_state = -1;

        for (int i = 0; i < numof_Dtran; i++) {
            if (Dtran[current_state][i] == current_char) {
                next_state = i;
                break;
            }
        }

        if (next_state != -1) {
            current_state = next_state;
            current_pos++;

            if (DFA[current_state].flag == 1) {
                last_accept_pos = current_pos;
                last_accept_state = current_state;
            }
        } else {
            if (last_accept_pos != -1) {
                int token_len = last_accept_pos - start_pos;
                strncpy(tokens[*token_count].token, input + start_pos, token_len);
                tokens[*token_count].token[token_len] = '\0';
                tokens[*token_count].type = last_accept_state;
                (*token_count)++;

                start_pos = last_accept_pos;
                current_pos = last_accept_pos;
                current_state = 0;
                last_accept_pos = -1;
                last_accept_state = -1;
            } else {
                std::cout << "Error: Unrecognized character '" 
                          << current_char << "' at position " 
                          << current_pos << std::endl;
                current_pos++;
                start_pos = current_pos;
                current_state = 0;
            }
        }
    }

    if (last_accept_pos != -1) {
        int token_len = last_accept_pos - start_pos;
        strncpy(tokens[*token_count].token, input + start_pos, token_len);
        tokens[*token_count].token[token_len] = '\0';
        tokens[*token_count].type = last_accept_state;
        (*token_count)++;
    }

    return tokens;
}

int main() {
    std::string input;
    std::cout << "Enter input string: ";
    std::getline(std::cin, input);

    int token_count;
    Token* tokens = lexer(input.c_str(), &token_count);

    std::cout << "\nLexical Analysis Results:\n";
    for (int i = 0; i < token_count; i++) {
        std::cout << "Token " << i+1 << ": '" << tokens[i].token 
                  << "' (Type: " << tokens[i].type << ")\n";
    }

    free(tokens);
    return 0;
}
