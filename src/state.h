#ifndef STATE_H
#define STATE_H

int memory[80*25];

#define CELL(x,y)   memory[80*y+x]
#define FILLROW(firstX,y_)   for(int x_=firstX;x_<80;x_++) CELL(x_,y_)=0;


#endif
