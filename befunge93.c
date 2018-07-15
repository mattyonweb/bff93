#include <stdio.h>
#include <stdlib.h>

#define LOOK(x,y) 80*y+x

void parse(FILE * f);
void print();

char memory[80*25] = {0};

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
    print();
}

void parse(FILE * fileSrc) {    
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x<80; x++) {
            
            char c = fgetc(fileSrc);

            if (c == '\n') {
                for (; x<80; x++)
                    memory[LOOK(x,y)] = 0;
                break;
            }
            else if (c == EOF) {
                for (; x<80; x++)
                    memory[LOOK(x,y)] = 0;
                for (; y<25; y++) {
                    for (x=0; x<80; x++)
                        memory[LOOK(x,y)] = 0;
                }
                return;
            }
            else {
                memory[LOOK(x,y)] = c;
            }
        }
    }
}

void print() {
    for (int y=0; y<25; y++) {
        for (int x=0; x<80; x++) {
            if (memory[LOOK(x,y)] == 0)
                printf("*");
            else
                printf("%c", memory[LOOK(x,y)]);
        }
        printf("\n");
    }
}
