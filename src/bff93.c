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
    char dir;
    Stack stack;
    int mail;
    int waitFor;
    int state;
} Thread;

Thread * createThread(int pid, int* ip, char dir, Stack stack, int mail, int state) {
    Thread * thread = malloc(sizeof(Thread));
    thread -> id = pid;
    thread -> ip = ip;
    thread -> dir = dir;
    thread -> stack = stack;
    thread -> mail = mail;
    thread -> waitFor = -1;
    thread -> state = state;
    
    return thread;
}

void exec(int debugMode, int threadedMode, int bits);
int * move(char dir, int *ip);
char rotate(char dir, char angle);

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
        printf("\tbff93 {-v} {-t} {-b} filename\n");
        printf("\n");
        printf("Usage:\n");
        printf("\t-v  sets the verbosity to 1 (the maximum);\n");
        printf("\t    if -v is enabled, <filename>.bf93's trace is sent to stdout,\n");
        printf("\t    otherwise it runs with outputting any uneeded information.\n");
        printf("\t-t  enables execution of threaded bf93 code;\n");
        printf("\t    use -t when your code has the branching ({) instruction.\n");
        printf("\t-b  <bits> tells the interpreter the size in bits of the cells.\n");
        printf("\t    default is 8; maximum is 32. Cells values are always signed.\n");
        printf("\n");
        printf("\tbff93 with no arguments prints this text.\n\n");
        exit(-1);
    }

    // Interpretation of command line arguments
    int debugMode = 0, threadedMode = 0, bits=8;
    for (int i=1; i<argv; i++) {
        if (!strcmp(argc[i], "-t"))
            threadedMode = 1;
        if (!strcmp(argc[i], "-v"))
            debugMode = 1;
        if (!strcmp(argc[i], "-b")) {
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
    // Maximum value possible in a cell
    int maxSize = 2 << (bits - 1);

    // Array of threads struct and initialization of the root thread
    int numThreads = 1;
    
    Thread ** threads = calloc(numThreads, sizeof(Thread *));
    threads[0] = createThread(0, memory, RIGHT,  stackInit(), -1, RUNNING);
    
    // Linked list ordinata con l'ordine di esecuzione dei thread
    Node orderExecution = nodeInit(0);

    
    if (debugMode)  printf("turn\ttid\tip\tdir\tmail\tw4\tout\tstack\n");
    
    int c1, c2, c3;
    char * string;
    char updateIp = 1;
    int turn = 0;
    Thread * thread = NULL;
    
    while (1) {
        
        for (Node threadId = orderExecution; threadId != NULL; threadId = threadId -> next) {
            // Il thread che viene eseguito ora      
            thread = threads[threadId -> val];
            
            // Non esegui i thread nati in questo turno
            if (thread -> state == READY) continue;
            
            // Non esegui dei thread morti in questo turno
            if (!linkedFind(orderExecution, thread -> id)) continue;
            
            updateIp = 1;
                        
            if (debugMode) printf("%d\t%d\t%c\t%d\t%d\t%d\t", turn, thread->id, *(thread -> ip), thread->dir, thread->mail, thread->waitFor);
            
            // If the thread is WAITING, check if it can be unlocked
            if (threadedMode && thread -> state == WAITING) {
                if (thread -> mail == thread -> waitFor) {
                    thread -> state = READY;
                    thread -> ip = move(thread -> dir, thread -> ip);
                }
                if (debugMode) printf("\n");
                continue;
            }
            switch (*(thread -> ip)) {
                case '+':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, c1+c2 % maxSize);
                    break;
                case '-':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, c2-c1 % maxSize);
                    break;
                case '*':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, c1*c2 % maxSize);
                    break;
                case '/':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    if (c1 == 0)
                        stackPush(thread -> stack, 0); //undefined behaviour
                    else
                        stackPush(thread -> stack, c2/c1 % maxSize);
                    break;
                case '%':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    stackPush(thread -> stack, c2%c1);
                    break;

                case '!':
                    c1 = stackPop(thread -> stack);
                    (c1 == 0) ? stackPush(thread -> stack, 1) : stackPush(thread -> stack, 0);
                    break;
                case '`':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
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
                    thread -> dir = rand() % 4;
                    break;
                case '{':
                    if (!threadedMode) continue;
                    
                    numThreads++;
                    
                    linkedExpand(orderExecution, thread -> id, numThreads-1); 
                    
                    
                    threads = realloc(threads, numThreads * sizeof(Thread *));
                    threads[numThreads-1] = createThread(numThreads-1, move(rotate(thread->dir,1), thread -> ip), 
                                                         RIGHT, stackCopy(thread -> stack), -1, READY);

                    thread -> ip = move(rotate(thread->dir,-1), thread -> ip);

                    continue;
                    
                case 'w':
                    if (!threadedMode) continue;
                    
                    c1 = stackPop(thread -> stack);
                    thread -> waitFor = c1;
                    thread -> state = WAITING;
                    if (debugMode) printf("\n");
                    continue;
                case 's':
                    if (!threadedMode) continue;
                    
                    c1 = stackPop(thread -> stack);
                    for (int t=0; t<numThreads; t++)
                        threads[t] -> mail = c1;
                    break;

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
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    if (c1 >= 25 || c1 < 0 || c2 >= 80 || c2 < 0)
                        stackPush(thread -> stack, 0);
                    else
                        stackPush(thread -> stack, CELL(c2, c1));
                    break;
                case 'p':
                    c1 = stackPop(thread -> stack);
                    c2 = stackPop(thread -> stack);
                    c3 = stackPop(thread -> stack);
                    CELL(c2, c1) = c3; //WRAP(c3,maxSize);
                    break;

                case '&':
                    string = calloc(maxSize + 1, sizeof(char)); // + 1 giusto? (32 bit + bit terminazione)
                    fgets(string, maxSize + 1, stdin);
                    
                    stackPush(thread -> stack, WRAP(atoi(string),maxSize)); //Mah mah mah (ALTA PROBABILIYA BUG)
                    free(string);
                    break;
                case '~':
                    c1 = getchar();
                    stackPush(thread -> stack, c1);
                    break;

                
                case '@':
                    if (!threadedMode) return;
                    
                    numThreads--;
                    if (numThreads == 0) return;

                    linkedRemove(orderExecution, threadId -> val);
                    
                    thread -> ip = NULL;
                    free(thread -> stack);
                    thread -> stack = NULL;
                    thread -> dir = -1;
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
            if (threads[t] -> state == READY) 
                threads[t] -> state = RUNNING;
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

char rotate(char dir, char angle) {
    switch(dir) {
        case(RIGHT): return angle == -1 ? UP : DOWN;
        case(LEFT) : return angle == -1 ? DOWN : UP;
        case(UP)   : return angle == -1 ? LEFT : RIGHT;  
        case(DOWN)   : return angle == -1 ? RIGHT : LEFT;  
    }
}
