#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simple-stack.h"

#define CELL(x,y)   memory[80*y+x]
#define FILLROW(firstX,y_)   for(int x_=firstX;x_<80;x_++) CELL(x_,y_)=0;

void exec();
void parse(FILE * f);
void print();
unsigned char * move(char dir, unsigned char *ip);

unsigned char memory[80*25] = {0};
Stack stack;

time_t t;

enum direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};
    
int main(int argv, char** argc) {
    if (argv != 2) {
        printf("befunge93\n");
        exit(-1);
    }
    
    FILE * fileSrc = fopen(argc[1], "rb");
    if (fileSrc == NULL) {
        printf("File not found. Exiting...");
        exit(-1); 
    }

    
    parse(fileSrc);

    srand(time(&t));  // for the RNG
    exec();
}

void exec() {
    unsigned char * ip  = memory;
    char dir            = RIGHT;
    stack               = stackInit();
    
    unsigned char c1, c2, c3;
    while (1) {
        switch (*ip) {
            case '+':
                c1 = stackPop(stack), c2 = stackPop(stack);
                stackPush(stack, c1+c2);
                break;
            case '-':
                c1 = stackPop(stack), c2 = stackPop(stack);
                stackPush(stack, c2-c1);
                break;
            case '*':
                c1 = stackPop(stack), c2 = stackPop(stack);
                stackPush(stack, c1*c2);
                break;
            case '/':
                c1 = stackPop(stack), c2 = stackPop(stack);
                if (c1 == 0)
                    stackPush(stack, 0); //undefined behaviour
                else
                    stackPush(stack, c2 / c1);
                break;
            case '%':
                c1 = stackPop(stack), c2 = stackPop(stack);
                stackPush(stack, c2 % c1); //ordine giusto?
                break;

            case '!':
                c1 = stackPop(stack);
                (c1 == 0) ? stackPush(stack, 1) : stackPush(stack, 0);
                break;
            case '`':
                c1 = stackPop(stack), c2 = stackPop(stack);
                (c2 > c1) ? stackPush(stack, 1) : stackPush(stack, 0);
                break;
                
            case '>':
                dir = RIGHT;
                break;
            case '<':
                dir = LEFT;
                break;
            case '^':
                dir = UP;
                break;
            case 'v':
                dir = DOWN;
                break;
            case '?':
                dir = rand() % 4;   // Giusto
                break;
                
            case '_':
                c1 = stackPop(stack);
                (c1 == 0) ? (dir = RIGHT) : (dir = LEFT); // Testato, funziona
                break;
            case '|':
                c1 = stackPop(stack);
                (c1 == 0) ? (dir = DOWN) : (dir = UP); // Testato, funziona
                break;

            case '"':
                ip = move(dir, ip);
                while (*ip != '"') {
                    stackPush(stack, *ip);
                    ip++;
                }
                break;

            case ':':
                c1 = stackPop(stack);
                stackPush(stack, c1);
                stackPush(stack, c1);
                break;
            case '\\':
                c1 = stackPop(stack), c2 = stackPop(stack);
                stackPush(stack, c1);
                stackPush(stack, c2);
                break;
            case '$':
                stackPop(stack);
                break;
            case '.':
                printf("%d", stackPop(stack));
                break;
            case ',':
                printf("%c", stackPop(stack));
                break;

            case '#':
                ip = move(dir, ip);
                break;

            case 'g':
                c1 = stackPop(stack), c2 = stackPop(stack);
                if (c1 >= 25 || c1 < 0 || c2 >= 80 || c2 < 0)
                    stackPush(stack, 0);
                else
                    stackPush(stack, CELL(c1, c2));
                break;
            case 'p':
                c1 = stackPop(stack), c2 = stackPop(stack), c3 = stackPop(stack);
                CELL(c2, c1) = c3;
                break;

            case '&':
                c1 = getchar();
                stackPush(stack, c1 - 48); //Mah mah mah (ALTA PROBABILIYA BUG)
                break;
            case '~':
                c1 = getchar();
                stackPush(stack, c1); //Mah mah mah
                break;

            case '@':
                return;

            case '0' ... '9':
                stackPush(stack, *ip - 48);
                break;
        }

        ip = move(dir, ip);
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

    
void parse(FILE * fileSrc) {
    /* From file to a 80x25 matrix. */
    int isLineEnded = 0;
    char c;
    
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x<80; x++) {
            
            c = fgetc(fileSrc);

            /* If the current line ends before reaching 80 chars, fill the
             * remaining of the row and go to the next one */
            if (c == '\n') {
                FILLROW(x,y);
                isLineEnded = 1;
                break;
            }

            /* If we encounter the EOF, fill the remaining of the current
             * row and then fill all the remaining rows. Then, exit. */
            else if (c == EOF) {
                FILLROW(x, y);
                y++;
                for (; y<25; y++)
                    FILLROW(0, y)
                return;
            }

            /* If everything is cool, just copy the char into the matrix. */
            else
                CELL(x,y) = c;
            
        }

        /* If we found an '\n' before the end of the row, we don't need to
         * loop until we find a '\n' (aka. the next block of code), so
         * just don't do it. */
        if (isLineEnded) {
            isLineEnded = 0;
            continue;
        }

        /* If the line is longer than 80 then extract and ignore chars until
         * you find a '\n' or a EOF. */
        do {
            c = fgetc(fileSrc);
            if (c == EOF) {
                y++;
                for (; y<25; y++)
                    FILLROW(0, y);
                return;
            }
        } while (c != '\n');
        
    }
}

void print() {
    for (int y=0; y<25; y++) {
        for (int x=0; x<80; x++) {
            if (CELL(x,y) == 0)
                printf("*");
            else
                printf("%c", CELL(x,y));
        }
        printf("\n");
    }
}
