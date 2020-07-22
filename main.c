#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define DEBUG 0
#define debug(fmt, ...) \
            do { if (DEBUG) printf(fmt, ##__VA_ARGS__); } while (0)

char *code;
char *input;
int tok_ptr = 0;
int line_counter = 1;
int line_position = 0;
int input_ptr = 0;
int bracket_counter = 0;

int *brackets;
int *test;
int puffer;

unsigned char *registers;
unsigned char *r;

void push_bracket(int position) {
    *(++brackets) = position;
}

int pop_bracket() {
    return *(brackets--);
}

void delete_char(int i)
{
    int len = strlen(code);

    for (; i < len - 1; i++)
    {
        code[i] = code[i + 1];
    }

    code[i] = '\0';
}

char getToken(int skip)
{
    char c = code[tok_ptr++];
    line_position++;
    if (c == '\n')
    {
        line_counter++;
        line_position = 1;
    }
    return c;
}

int interpret()
{
    brackets = calloc(200, sizeof(int));
    test = brackets;
    registers = calloc(200, sizeof(char));
    r = registers;
    registers += 100; // Go to center of allocated memory

    tok_ptr = 0;
    char cur_tok;
    int skip = 0;
    
    do
    {
        cur_tok = getToken(skip);

        for(int i = 0; i < 5; i++) {
            debug("%d\t", *(test + i));
        }

        debug("\n");
        debug("LAST_BRACKET: %d\n", *brackets);

        if (skip != 0) 
        {
            if(cur_tok == ']')
            {
                skip--;
            }
            else if (cur_tok == '[')
            {
                skip++;
            }
        }
        else
        {
            debug("token: %c, position: %d, skip: %d\n", cur_tok, tok_ptr, skip);
            switch (cur_tok)
            {
            case '+':
                (*registers)++;
                debug("Increased register at position %p to %d\n", registers, (int)*registers);
                break;
            case '-':
                (*registers)--;
                debug("Decreased register at position %p to %d\n", registers, (int)*registers);
                break;
            case '<':
                *registers--;
                debug("Switched to previous register\n");
                break;
            case '>':
                *registers++;
                debug("Switched to next register\n");
                break;
            case '.':
                debug("Printing register at position %p. Value: %d\n", registers, (int)*registers);
                printf("%c", *registers);
                break;
            case ',':
                *registers = input[input_ptr++];
                break;
            case '[':
                bracket_counter++;
                if (*registers != 0)
                {
                    debug("pushing bracket at position %d\n", tok_ptr);
                    push_bracket(tok_ptr);
                }
                else
                {
                    skip = 1;
                }
                break;
            case ']':
                bracket_counter--;
                puffer = pop_bracket();
                if (*registers != 0)
                {
                    debug("returning to last bracket at position %d\n", puffer);
                    tok_ptr = puffer;
                    tok_ptr--;
                }
                break;
            case '\0':
                if (bracket_counter != 0)
                {
                    debug("ERROR: Unclosed bracket\n");
                    return 1;
                }
                printf("\n");
                break;
            default:
                /* printf("Got different char in line %d at position %d: %c\n", line_counter, line_position, cur_tok); */
                break;
            }
        }
    } while (cur_tok != '\0');

    free(r);
    return 0;
}

void showHelp()
{
    printf("Usage: compiler \"[brainfuck_code]\" \"[input]\"\n");
}

int main(int argc, char *argv[])
{
    if (argc == 1 || strcmp(argv[1], "--help") == 0)
    {
        showHelp();
        return 0;
    }

    struct stat buffer;
    if (stat(argv[1], &buffer) == 0)
    {
        FILE *file = fopen(argv[1], "r");
        fseek(file, 0, SEEK_END);
        long fsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        code = malloc(fsize + 1);
        fread(code, 1, fsize, file);
        fclose(file);
    }
    else
    {
        code = argv[1];
    }

    input = argv[2];

    debug("%s\n", code);  

    return interpret();
}