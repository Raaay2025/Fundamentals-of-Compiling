#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// 种别码定义
#define KEYWORD_BASE 1
#define OPERATOR_BASE 33
#define INT_CONST 100
#define FLOAT_CONST 101
#define IDENTIFIER 200
#define STRING_CONST 102
#define CHAR_CONST 103

// 状态定义
#define STATE_INITIAL 0
#define STATE_IDENTIFIER 1
#define STATE_INTEGER 2
#define STATE_FLOAT 3
#define STATE_OPERATOR 4
#define STATE_STRING 5
#define STATE_CHAR 6
#define STATE_ESCAPE_STRING 7
#define STATE_ESCAPE_CHAR 8
#define STATE_SINGLE_COMMENT 9
#define STATE_MULTI_COMMENT 10

char* keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "return", "register", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"
};

char* operators[] = {
    ",", "(", ")", "[", "]", "->", ".", "!", "++", "--",
    "&", "~", "*", "/", "%", "+", "-", "<<", ">>", "<",
    "<=", ">", ">=", "==", "!=", "&&", "||", "=", "+=",
    "-=", "*=", "/=", ";", "{", "}", "'", "\""
};

int is_keyword(char* token) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(token, keywords[i]) == 0) return KEYWORD_BASE + i;
    }
    return -1;
}

int get_operator_code(char* op) {
    for (int i = 0; i < 37; i++) {
        if (strcmp(op, operators[i]) == 0) return OPERATOR_BASE + i;
    }
    return -1;
}

void write_token(FILE* out, int code, char* value) {
    fprintf(out, "(%d, %s)\n", code, value);
}

int main() {
    FILE *src = fopen("../dfa/dfa_input.txt", "r");
    FILE *out = fopen("../dfa/dfa_output.txt", "w");
    if (!src || !out) {
        perror("File open error");
        exit(1);
    }

    char c, token[256];
    int pos = 0, state = STATE_INITIAL;

    while ((c = fgetc(src)) != EOF) {
        switch (state) {
            case STATE_INITIAL:
                if (c == '#') {
                    // 跳过预处理指令直到行尾
                    while ((c = fgetc(src)) != '\n' && c != EOF);
                } else if (c == '/') {
                    char next = fgetc(src);
                    if (next == '/') {
                        state = STATE_SINGLE_COMMENT;
                    } else if (next == '*') {
                        state = STATE_MULTI_COMMENT;
                    } else {
                        fseek(src, -1, SEEK_CUR);
                        token[pos++] = c;
                        state = STATE_OPERATOR;
                    }
                } else if (isalpha(c) || c == '_') {
                    token[pos++] = c;
                    state = STATE_IDENTIFIER;
                } else if (isdigit(c)) {
                    token[pos++] = c;
                    state = STATE_INTEGER;
                } else if (strchr("+-<>=!&|*/%", c)) {
                    token[pos++] = c;
                    state = STATE_OPERATOR;
                } else if (c == '.') {
                    char next = fgetc(src);
                    if (isdigit(next)) {
                        token[pos++] = c;
                        token[pos++] = next;
                        state = STATE_FLOAT;
                    } else {
                        fseek(src, -1, SEEK_CUR);
                        token[pos++] = c;
                        write_token(out, get_operator_code("."), token);
                        pos = 0;
                    }
                } else if (c == '"') {
                    token[pos++] = c;
                    state = STATE_STRING;
                } else if (c == '\'') {
                    token[pos++] = c;
                    state = STATE_CHAR;
                } else if (strchr(",()[]{};", c)) {
                    token[pos++] = c;
                    token[pos] = '\0';
                    write_token(out, get_operator_code(token), token);
                    pos = 0;
                } else if (!isspace(c)) {
                    printf("Invalid character: %c\n", c);
                }
                break;

            case STATE_IDENTIFIER:
                if (isalnum(c) || c == '_') {
                    token[pos++] = c;
                } else {
                    fseek(src, -1, SEEK_CUR);
                    token[pos] = '\0';
                    int code = is_keyword(token);
                    write_token(out, code != -1 ? code : IDENTIFIER, token);
                    pos = 0;
                    state = STATE_INITIAL;
                }
                break;

            case STATE_INTEGER:
                if (isdigit(c)) {
                    token[pos++] = c;
                } else if (c == '.') {
                    token[pos++] = c;
                    state = STATE_FLOAT;
                } else {
                    fseek(src, -1, SEEK_CUR);
                    token[pos] = '\0';
                    write_token(out, INT_CONST, token);
                    pos = 0;
                    state = STATE_INITIAL;
                }
                break;

            case STATE_FLOAT:
                if (isdigit(c)) {
                    token[pos++] = c;
                } else {
                    fseek(src, -1, SEEK_CUR);
                    token[pos] = '\0';
                    write_token(out, FLOAT_CONST, token);
                    pos = 0;
                    state = STATE_INITIAL;
                }
                break;

            case STATE_OPERATOR:
                token[pos++] = c;
                token[pos] = '\0';
                if (get_operator_code(token) != -1) {
                    write_token(out, get_operator_code(token), token);
                } else {
                    token[--pos] = '\0';
                    fseek(src, -1, SEEK_CUR);
                    write_token(out, get_operator_code(token), token);
                }
                pos = 0;
                state = STATE_INITIAL;
                break;

            case STATE_STRING:
                if (c == '\\') {
                    state = STATE_ESCAPE_STRING;
                } else if (c == '"') {
                    token[pos++] = c;
                    token[pos] = '\0';
                    write_token(out, STRING_CONST, token);
                    pos = 0;
                    state = STATE_INITIAL;
                } else {
                    token[pos++] = c;
                }
                break;

            case STATE_CHAR:
                if (c == '\\') {
                    state = STATE_ESCAPE_CHAR;
                } else if (c == '\'') {
                    token[pos++] = c;
                    token[pos] = '\0';
                    write_token(out, CHAR_CONST, token);
                    pos = 0;
                    state = STATE_INITIAL;
                } else {
                    token[pos++] = c;
                }
                break;

            case STATE_ESCAPE_STRING:
                token[pos++] = c;
                state = STATE_STRING;
                break;

            case STATE_ESCAPE_CHAR:
                token[pos++] = c;
                state = STATE_CHAR;
                break;

            case STATE_SINGLE_COMMENT:
                if (c == '\n') {
                    state = STATE_INITIAL;
                }
                break;

            case STATE_MULTI_COMMENT:
                if (c == '*') {
                    char next = fgetc(src);
                    if (next == '/') {
                        state = STATE_INITIAL;
                    } else {
                        fseek(src, -1, SEEK_CUR);
                    }
                }
                break;
        }
    }

    // 处理文件末尾未完成的token
    if (pos > 0) {
        token[pos] = '\0';
        switch (state) {
            case STATE_IDENTIFIER: {
                int code = is_keyword(token);
                write_token(out, code != -1 ? code : IDENTIFIER, token);
                break;
            }
            case STATE_INTEGER:
                write_token(out, INT_CONST, token);
                break;
            case STATE_FLOAT:
                write_token(out, FLOAT_CONST, token);
                break;
            case STATE_STRING:
                printf("Error: Unclosed string literal\n");
                break;
            case STATE_CHAR:
                printf("Error: Unclosed char literal\n");
                break;
        }
    }

    fclose(src);
    fclose(out);
    return 0;
}
