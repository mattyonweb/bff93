#include <stdio.h>
#include <stdlib.h>
#include "state.h"
#include "parse.h"

//#define FILLROW(firstX,y_)   for(int x_=firstX;x_<80;x_++) CELL(x_,y_)=0;

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
