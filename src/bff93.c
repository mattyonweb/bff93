#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "state.h"
#include "datastruct.h"
#include "parse.h"

typedef struct {
    int id;
    int * ip;
    int canGo;
    char dir;
    Stack stack;
    int mail;
    int state;
} Thread;

Thread * createThread(int pid, int* ip, int canGo, char dir, Stack stack, int mail, int state) {
    Thread * thread = malloc(sizeof(Thread));
    thread -> id = pid;
    thread -> ip = ip; //??????
    thread -> canGo = canGo;
    thread -> dir = dir;
    thread -> stack = stack; //???????????
    thread -> mail = mail;
    thread -> state = state;
    
    return thread;
}

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

    Thread ** threads = calloc(numThreads, sizeof(Thread *));
    threads[0] = createThread(0, memory, 1, RIGHT,  stackInit(), -1, RUNNING);
    
    // Linked list ordinata con l'ordine di esecuzione dei thread
    Node orderExecution = nodeInit(0);

    // Gli Instruction Pointer per ogni thread
    //int ** ips = calloc(numThreads, sizeof(int *));
    //ips[0] = memory;

    // Le direzioni di ogni thread
    //char * dirs = calloc(numThreads, sizeof(char));
    //dirs[0] = RIGHT;

    // Stack per threads
    //stacks = calloc(numThreads, sizeof(Stack));
    //stacks[0] = stackInit();

    //int * mails = calloc(numThreads, sizeof(int));
    //int * states = calloc(numThreads, sizeof(int));
    //mails[0] = 0;
    //states[0] = RUNNING;
    
    // Array di bool; dicono se il thread n-esimo può essere eseguito in questo turno
    //char * canGos = calloc(numThreads, sizeof(char));
    //canGos[0] = 1;

    
    if (debugMode)  printf("turn\ttid\tip\tdir\tout\tstack\n");
    
    int c1, c2, c3;
    char * string;
    char updateIp = 1;
    int turn = 0;
    Thread * thread = NULL;
    
    while (1) {
        
        for (Node threadId = orderExecution; threadId != NULL; threadId = threadId -> next) {         
            //int thread = threadId -> val;
            thread = threads[threadId -> val];

            // Non esegui i thread nati in questo turno
            if (!(thread -> canGo)) continue;
            
            // Non esegui dei thread morti in questo turno
            if (!linkedFind(orderExecution, thread -> id)) continue; /////MMMMMH BUG?!?!
            
            updateIp = 1;
                        
            if (debugMode) printf("%d\t%d\t%c\t%d\t", turn, thread->id, *(thread -> ip), thread->dir);
            
            switch (*(thread -> ip)) {
                case '+':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, (c1+c2) % maxSize);
                    break;
                case '-':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, (c2-c1) % maxSize);
                    break;
                case '*':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, (c1*c2) % maxSize);
                    break;
                case '/':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    if (c1 == 0)
                        stackPush(thread -> stack, 0); //undefined behaviour
                    else
                        stackPush(thread -> stack, (c2 / c1) % maxSize); // necessario il maxSize?
                    break;
                case '%':
                    c1 = stackPop(thread -> stack), c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, (c2 % c1) % maxSize); //ordine giusto?
                    break;

                case '!':
                    c1 = stackPop(thread -> stack);
                    (c1 == 0) ? stackPush(thread -> stack, 1) : stackPush(thread -> stack, 0);
                    break;
                case '`':
                    c1 = stackPop(thread -> stack), c2 = stackPop(thread -> stack);
                    (c2 > c1) ? stackPush(thread -> stack, 1) : stackPush(thread -> stack, 0);
                    break;
                    
                case '>':
                    thread -> dir = RIGHT;
                    break;
                case '<':
                    thread -> dir = LEFT;
                    break;
                case '^':
                    thread -> dir = UP;
                    break;
                case 'v':
                    thread -> dir = DOWN;
                    break;
                case '?':
                    thread -> dir = rand() % 4;   // Giusto
                    break;
                case '{':
                    if (!threadedMode) continue;
                    
                    numThreads++;
                    
                    linkedExpand(orderExecution, thread -> id, numThreads-1); //giusto se messo proprio qui?

                    threads = realloc(threads, numThreads * sizeof(Thread *));
                    threads[numThreads-1] = createThread(numThreads-1, move(DOWN, thread -> ip), 
                                                         0, RIGHT, stackCopy(thread -> stack), -1, RUNNING);

                    thread -> ip = move(UP, thread -> ip);

                    //updateIp = 0;

                    continue;
                    
                //case 'w':
                    //c1 = stackPop(stacks[thread]);
                    
                case '_':
                    c1 = stackPop(thread -> stack);
                    (c1 == 0) ? (thread -> dir = RIGHT) : (thread -> dir = LEFT);
                    break;
                case '|':
                    c1 = stackPop(thread -> stack);
                    (c1 == 0) ? (thread -> dir = DOWN) : (thread -> dir = UP);
                    break;

                case '"':
                    thread -> ip = move(thread -> dir, thread -> ip);
                    while (*(thread -> ip) != '"') {
                        stackPush(thread -> stack, *(thread -> ip));
                        thread -> ip = move(thread -> dir, thread -> ip);
                    }
                    break;

                case ':':
                    c1 = stackPop(thread -> stack);
                    stackPush(thread -> stack, c1);
                    stackPush(thread -> stack, c1);
                    break;
                case '\\':
                    c1 = stackPop(thread -> stack), c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, c1);
                    stackPush(thread -> stack, c2);
                    break;
                case '$':
                    stackPop(thread -> stack);
                    break;
                case '.':
                    printf("%d ", stackPop(thread -> stack));
                    break;
                case ',':
                    printf("%c", stackPop(thread -> stack));
                    break;

                case '#':
                    thread -> ip = move(thread -> dir, thread -> ip);
                    break;

                case 'g':
                    c1 = stackPop(thread -> stack), c2 = stackPop(thread -> stack);
                    if (c1 >= 25 || c1 < 0 || c2 >= 80 || c2 < 0)
                        stackPush(thread -> stack, 0);
                    else
                        stackPush(thread -> stack, CELL(c2, c1));
                    break;
                case 'p':
                    c1 = stackPop(thread -> stack), c2 = stackPop(thread -> stack), c3 = stackPop(thread -> stack);
                    CELL(c2, c1) = c3;
                    break;

                case '&':
                    string = calloc(maxSize + 1, sizeof(char)); // + 1 giusto? (32 bit + bit terminazione)
                    fgets(string, maxSize + 1, stdin);
                    
                    stackPush(thread -> stack, atoi(string)); //Mah mah mah (ALTA PROBABILIYA BUG)
                    free(string);
                    break;
                case '~':
                    c1 = getchar();
                    stackPush(thread -> stack, c1); //Mah mah mah
                    break;

                
                case '@':
                    if (!threadedMode) return;
                    
                    numThreads--;
                    if (numThreads == 0) return;

                    linkedRemove(orderExecution, threadId -> val);
                    
                    //free(threads[threadId -> val]);
                    thread -> ip = NULL;
                    free(thread -> stack);
                    thread -> stack = NULL;
                    thread -> dir = -1;
                    thread -> canGo = -1;
                    thread -> mail = -1;
                    thread -> state = KILLED;

                    continue;

                case '0' ... '9':
                    stackPush(thread -> stack, *(thread -> ip) - 48);
                    break;
            }

            if (debugMode) stackPrint(thread -> stack);
            if (updateIp)  thread -> ip = move(thread -> dir, thread -> ip);
        }

        // Mark all threads as executable in the next turn 
        for (int t=0; t<numThreads; t++) {
            if (threads[t] -> canGo != -1) 
                threads[t] -> canGo = 1;
        }
        turn++;
        
        if (debugMode)
            getchar();
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
