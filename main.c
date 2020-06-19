#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>

char *token;
char *input;
int tok_ptr = 0;
int line_counter = 1;
int line_position = 1;
int input_ptr = 0;
int bracket_counter = 0;

int bracket_stack[1000];
int *last_bracket;
int puffer;

#define push(last_bracket, n) (*((last_bracket)++) = (n))
#define pop(last_bracket) (*--(last_bracket))

char *registers;
char *r;

void delete_char(int i) {
    int len = strlen(token);

    for (; i < len - 1 ; i++)
    {
       token[i] = token[i+1];
    }

    token[i] = '\0';
}

char getToken() {
    char c = token[tok_ptr++];
    line_position++;
    if (c == '\0') {
        return 'E';
    }
    if (c == '\n') {
        line_counter++;
        line_position = 1;
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
                    printf("No bracket to close in line %d at position %d\n", line_counter, tok_ptr);
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
                delete_char(tok_ptr-1);
                tok_ptr -= 1;
                break;
        }
    }
    return 0;
}

int interpret() {
    last_bracket = bracket_stack;

    registers = calloc(200, sizeof(char));
    r = registers;
    registers += 100;   // Go to center of allocated memory

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
                        push(last_bracket, tok_ptr-1);
                    } else {
                        skip = 1;
                    }
                    break;
                case ']':
                    puffer = pop(last_bracket);
                    if (*registers != 0) {
                        tok_ptr = puffer;
                    }
                    break;
                case 'E':
                    printf("\n");
                    break;
                default:
                    printf("Got different char in line %d at position %d: %c\n", line_counter, line_position, cur_tok);
                    break;
            }
        }
    }

    free(r);
    return 0;
}

void showHelp() {
    printf("Usage: compiler \"[brainfuck_code]\" \"[input]\"\n");
}

int main(int argc, char* argv[]) {
    if (argc == 1 || strcmp(argv[1], "--help") == 0) {
        showHelp();
        return 0;
    }

    struct stat buffer;
    if (stat(argv[1], &buffer) == 0 ) {
        FILE *file = fopen(argv[1], "r");
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        token = malloc(fsize + 1);
        fread(token, 1, fsize, file);
        fclose(file);
    } else {
        token = argv[1];
    }

    input = argv[2];

    if (scanner() != 0) {
        return 1;
    }

    if (interpret() != 0) {
        return 1;
    }

    return 0;
}