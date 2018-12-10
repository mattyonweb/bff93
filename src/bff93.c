#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "state.h"
#include "datastruct.h"
#include "parse.h"

void exec(int debugMode, int threadedMode, int bits);
int * move(char dir, int *ip);

Stack * stacks;

time_t t;

enum direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};
    
int main(int argv, char** argc) {
    if (argv == 1 || argv > 5) {
        printf("bff93 - a befunge93 interpreter\n\n");
        printf("Syntax:\n");
        printf("\tbff93 -vt filename\n");
        printf("\n");
        printf("Usage:\n");
        printf("\t-v  sets the verbosity to 1 (the maximum);\n");
        printf("\t    if -v is enabled, <filename>.bf93's trace is sent to stdout,\n");
        printf("\t    otherwise it runs with outputting any uneeded information.\n");
        printf("\t-t  enables execution of threaded bf93 code;\n");
        printf("\t    use -t when your code has the branching ({) instruction.\n");
        printf("\t-ub <bits> tells the interpreter the size in bits of the cells.\n");
        printf("\t    default is 8; maximum is 32. Cells values are always unsigned.\n");
        printf("\tbff93 with no arguments prints this text.\n");
        exit(-1);
    }

    // Interpretation of command line arguments
    int debugMode = 0, threadedMode = 0, bits=8;
    for (int i=1; i<argv; i++) {
        if (!strcmp(argc[i], "-t"))
            threadedMode = 1;
        if (!strcmp(argc[i], "-v"))
            debugMode = 1;
        if (!strcmp(argc[i], "-ub")) {
            bits = atoi(argc[i+1]);
            bits = bits >= 32 ? 31 : bits;
            i++;
        }
    }
    
    FILE * fileSrc = fopen(argc[argv-1], "rb");
    if (fileSrc == NULL) {
        printf("File not found. Exiting...");
        exit(-1); 
    }
    
    parse(fileSrc);

    srand(time(&t));  // for the RNG
    exec(debugMode, threadedMode, bits);
}

void exec(int debugMode, int threadedMode, int bits) {
    int maxSize = 2 << (bits - 1);
    
    int numThreads = 1;

    // Linked list ordinata con l'ordine di esecuzione dei thread
    Node orderExecution = nodeInit(0);

    // Gli Instruction Pointer per ogni thread
    int ** ips = calloc(numThreads, sizeof(int *));
    ips[0] = memory;

    // Le direzioni di ogni thread
    char * dirs = calloc(numThreads, sizeof(char));
    dirs[0] = RIGHT;

    // Stack per threads
    stacks = calloc(numThreads, sizeof(Stack));
    stacks[0] = stackInit();

    // Array di bool; dicono se il thread n-esimo può essere eseguito in questo turno
    char * canGos = calloc(numThreads, sizeof(char));
    canGos[0] = 1;

    
    if (debugMode)  printf("turn\ttid\tip\tout\tstack\n");
    
    int c1, c2, c3;
    char * string;
    char updateIp = 1;
    int turn = 0;
    
    while (1) {
        
        for (Node threadId = orderExecution; threadId != NULL; threadId = threadId -> next) {         
            int thread = threadId -> val;

            // Non esegui i thread nati in questo turno
            if (!canGos[thread]) continue;
            
            // Non esegui dei thread morti in questo turno
            if (!linkedFind(orderExecution, thread)) continue;
            
            updateIp = 1;
                        
            if (debugMode) printf("%d\t%d\t%c\t", turn, thread, *ips[thread]);
            
            switch (*ips[thread]) {
                case '+':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], (c1+c2) % maxSize);
                    break;
                case '-':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], (c2-c1) % maxSize);
                    break;
                case '*':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], (c1*c2) % maxSize);
                    break;
                case '/':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    if (c1 == 0)
                        stackPush(stacks[thread], 0); //undefined behaviour
                    else
                        stackPush(stacks[thread], (c2 / c1) % maxSize); // necessario il maxSize?
                    break;
                case '%':
                    c1 = stackPop(stacks[thread]), c2 = stackPop(stacks[thread]);
                    stackPush(stacks[thread], (c2 % c1) % maxSize); //ordine giusto?
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
                case '{':
                    if (!threadedMode) continue;
                    
                    numThreads++;
                    ips = realloc(ips, numThreads * sizeof(int *));
                    stacks = realloc(stacks, numThreads * sizeof(Stack));
                    dirs = realloc(dirs, numThreads * sizeof(char));
                    canGos = realloc(canGos, numThreads * sizeof(char));
                    
                    linkedExpand(orderExecution, thread, numThreads-1); //giusto se messo proprio qui?

                    ips[numThreads-1] = move(DOWN, ips[thread]);    //copia 1:1 di ips[thread],
                    
                    stacks[numThreads-1] = malloc(sizeof(Stack));
                    stacks[numThreads-1] = stackCopy(stacks[thread]);
                    dirs[numThreads-1] = RIGHT;
                    canGos[numThreads-1] = 0;

                    ips[thread] = move(UP, ips[thread]);

                    //updateIp = 0;

                    continue;
                    
                case '_':
                    c1 = stackPop(stacks[thread]);
                    (c1 == 0) ? (dirs[thread] = RIGHT) : (dirs[thread] = LEFT);
                    break;
                case '|':
                    c1 = stackPop(stacks[thread]);
                    (c1 == 0) ? (dirs[thread] = DOWN) : (dirs[thread] = UP);
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
                    string = calloc(maxSize + 1, sizeof(char)); // + 1 giusto? (32 bit + bit terminazione)
                    fgets(string, maxSize + 1, stdin);
                    
                    stackPush(stacks[thread], atoi(string)); //Mah mah mah (ALTA PROBABILIYA BUG)
                    free(string);
                    break;
                case '~':
                    c1 = getchar();
                    stackPush(stacks[thread], c1); //Mah mah mah
                    break;

                
                case '@':
                    if (!threadedMode) return;
                    
                    numThreads--;
                    if (numThreads == 0) return;

                    linkedRemove(orderExecution, thread);
                    
                    ips[thread] = NULL;
                    free(stacks[thread]);
                    stacks[thread] = NULL;
                    dirs[thread] = -1;
                    canGos[thread] = -1;

                    continue;

                case '0' ... '9':
                    stackPush(stacks[thread], *ips[thread] - 48);
                    break;
            }

            if (debugMode) stackPrint(stacks[thread]);
            if (updateIp)  ips[thread] = move(dirs[thread], ips[thread]);
        }

        // Mark all threads as executable in the next turn 
        for (int t=0; t<numThreads; t++)
            if (canGos[t] != -1) canGos[t] = 1;
        turn++;
    }
}

int * move (char dir, int *ip) {
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
