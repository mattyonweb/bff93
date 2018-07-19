#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "state.h"
#include "simple-stack.h"
#include "parse.h"

void exec(int debugMode);
unsigned char * move(char dir, unsigned char *ip);

Stack * stacks;

time_t t;

enum direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};
    
int main(int argv, char** argc) {
    /* argc[0]  ->  befunge93
     * argc[1] if argn==3  ->  debug mode
     * argc[1] if argn==2  ->  filename
     * argc[2] if argn==3  ->  filename     */
    
    if (argv == 1 || argv > 3) {
        printf("bff93 - a befunge93 interpreter\n\n");
        printf("Syntax:\n");
        printf("\tbff93 <opt: debug> path/to/file\n");
        printf("\n");
        printf("Usage:\n");
        printf("\t- without the debug flag, bff93 runs the .bf93 file as it is.\n");
        printf("\t- if debug != 0, <filename>.bf93's trace is sent to stdout.\n");
        printf("\t- bff93 with no arguments prints this text.\n");
        exit(-1);
    }

    int filenameIdx = (argv == 2) ? 1 : 2;
    int debugMode   = (argv == 3) ? *argc[1] - 48 : 0;
    
    FILE * fileSrc = fopen(argc[filenameIdx], "rb");
    if (fileSrc == NULL) {
        printf("File not found. Exiting...");
        exit(-1); 
    }
    
    parse(fileSrc);

    srand(time(&t));  // for the RNG
    exec(debugMode);
}

void exec(int debugMode) {
    int numThreads = 1;
        
    unsigned char ** ips = calloc(numThreads, sizeof(unsigned char *));
    ips[0] = memory;
    
    char * dirs = calloc(numThreads, sizeof(char));
    dirs[0] = RIGHT;
    
    stacks = calloc(numThreads, sizeof(Stack));
    stacks[0] = stackInit();


    if (debugMode)  printf("ip\tout\tstack\n");
    
    unsigned char c1, c2, c3;
    char * string;
    while (1) {
        for (int thread=0; thread < numThreads; thread++) {
            //Stack stack = stacks[thread];
            //unsigned char * ip = ips[thread];
            char dir = dirs[thread];
            
            if (debugMode) printf("%c\t", *ips[thread]);
            
            switch (*ips[thread]) {
                case '+':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], c1+c2);
                    break;
                case '-':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], c2-c1);
                    break;
                case '*':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], c1*c2);
                    break;
                case '/':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    if (c1 == 0)
                        stackPush(stacks[thread], 0); //undefined behaviour
                    else
                        stackPush(stacks[thread], c2 / c1);
                    break;
                case '%':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], c2 % c1); //ordine giusto?
                    break;

                case '!':
                    c1 = stackPop(stacks[thread]);
                    (c1 == 0) ? stackPush(stacks[thread], 1) : stackPush(stacks[thread], 0);
                    break;
                case '`':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    (c2 > c1) ? stackPush(stacks[thread], 1) : stackPush(stacks[thread], 0);
                    break;
                    
                case '>':
                    dirs[thread] = RIGHT;
                    break;
                case '<':
                    dirs[thread] = LEFT;
                    break;
                case '^':
                    dirs[thread] = UP;
                    break;
                case 'v':
                    dirs[thread] = DOWN;
                    break;
                case '?':
                    dirs[thread] = rand() % 4;   // Giusto
                    break;
                    
                case '_':
                    c1 = stackPop(stacks[thread]);
                    (c1 == 0) ? (dirs[thread] = RIGHT) : (dirs[thread] = LEFT); // Testato, funziona
                    break;
                case '|':
                    c1 = stackPop(stacks[thread]);
                    (c1 == 0) ? (dirs[thread] = DOWN) : (dirs[thread] = UP); // Testato, funziona
                    break;

                case '"':
                    ips[thread] = move(dirs[thread], ips[thread]);
                    while (*ips[thread] != '"') {
                        stackPush(stacks[thread], *ips[thread]);
                        ips[thread] = move(dirs[thread], ips[thread]);
                    }
                    break;

                case ':':
                    c1 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], c1);
                    stackPush(stacks[thread], c1);
                    break;
                case '\\':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], c1);
                    stackPush(stacks[thread], c2);
                    break;
                case '$':
                    stackPop(stacks[thread]);
                    break;
                case '.':
                    printf("%d ", stackPop(stacks[thread]));
                    break;
                case ',':
                    printf("%c", stackPop(stacks[thread]));
                    break;

                case '#':
                    ips[thread] = move(dirs[thread], ips[thread]);
                    break;

                case 'g':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    if (c1 >= 25 || c1 < 0 || c2 >= 80 || c2 < 0)
                        stackPush(stacks[thread], 0);
                    else
                        stackPush(stacks[thread], CELL(c2, c1));
                    break;
                case 'p':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]), c3 = stackPop(stacks[thread]);
                    CELL(c2, c1) = c3;
                    break;

                case '&':
                    string = calloc(4, sizeof(char));
                    fgets(string, 4, stdin);
                    
                    stackPush(stacks[thread], atoi(string)); //Mah mah mah (ALTA PROBABILIYA BUG)
                    free(string);
                    break;
                case '~':
                    c1 = getchar();
                    stackPush(stacks[thread], c1); //Mah mah mah
                    break;

                
                case '@':
                    return;

                case '0' ... '9':
                    stackPush(stacks[thread], *ips[thread] - 48);
                    break;
            }

            if (debugMode) stackPrint(stacks[thread]);
            ips[thread] = move(dirs[thread], ips[thread]);
        }

    }
}

unsigned char * move (char dir, unsigned char *ip) {
    int pos = ip - memory;
    switch(dir) {
        case UP:
            pos -= 80;
            if (pos < 0)
                pos += 80 * 25 - 1; // Booooh... mah...
            return memory + pos;
        case DOWN:
            pos += 80;
            if (pos > 80 * 25 - 1)
                pos -= 80 * 25 + 1;
            return memory + pos;
        case RIGHT:
            pos += 1;
            if (pos % 80 == 0)
                pos -= 80;
            return memory + pos;
        case LEFT:
            pos -= 1;
            if (pos % 80 == 79)
                pos += 80;
            return memory + pos;
        default:
            printf("ERROR");
            exit(-1);
    }
}
