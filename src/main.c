#include<stdio.h>
#include<stdlib.h>

char *token;
char *input;
int tok_ptr = 0;
int input_ptr = 0;
int bracket_counter = 0;

char *registers;

char getToken() {
    char c = token[tok_ptr++];
    if (c == '\0') {
        return 'E';
    }
    return c;
}

int scanner() {
    char cur_tok;
    
    while(cur_tok != 'E') {
        cur_tok = getToken();
        
        switch (cur_tok) {
            case '+':
            case '-':
            case '<':
            case '>':
            case '.':
            case ',':
                break;
            case '[':
                bracket_counter++;
                break;
            case ']':
                if (bracket_counter == 0) {
                    printf("No bracket to close at position %d\n", tok_ptr);
                    return 1;
                }
                bracket_counter--;
                break;
            case 'E':
                if (bracket_counter != 0) {
                    printf("Unclosed bracket\n");
                    return 1;
                }
                break;
            default:
                printf("Invalid character %c in position %d.\n", cur_tok, tok_ptr);
                return 1;
        }
    }
    return 0;
}

void showHelp() {
    printf("Usage: compiler \"[brainfuck_code]\" \"[input]\"\n");
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        showHelp();
        return 0;
    }
    token = argv[1];
    input = argv[2];

    if (scanner() != 0) {
        return 1;
    }

    registers = calloc(100, sizeof(char));

    tok_ptr = 0;
    char cur_tok;
    int last_bracket_pos;
    int skip = 0;

    while(cur_tok != 'E') {
        cur_tok = getToken();

        if (skip == 1 && cur_tok == ']') {
            skip = 0;
        }

        if (skip == 0) {
            switch (cur_tok) {
                case '+':
                    (*registers)++;
                    break;
                case '-':
                    (*registers)--;
                    break;
                case '<':
                    *registers--;
                    break;
                case '>':
                    *registers++;
                    break;
                case '.':
                    printf("%c", *registers);
                    break;
                case ',':
                    *registers = input[input_ptr++];
                    break;
                case '[':
                    if (*registers != 0) {
                        last_bracket_pos = tok_ptr-1;
                    } else {
                        skip = 1;
                    }
                    break;
                case ']':
                    if (*registers != 0) {
                        tok_ptr = last_bracket_pos;
                    }
                    break;
                case 'E':
                    printf("\n");
            }
        }
    }

    free(registers);

    return 0;
}