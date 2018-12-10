#ifndef STATE_H
#define STATE_H

int memory[80*25];

#define CELL(x,y)   memory[80*y+x]
#define FILLROW(firstX,y_)   for(int x_=firstX;x_<80;x_++) CELL(x_,y_)=0;

#define READY 0   // Ready to start at the next turn
#define RUNNING 1
#define WAITING 2
#define KILLED -1

#define WRAP(x__, maxSize__) (x__ < 0) ? maxSize + (x__ % maxSize__) : x__ % maxSize__

#endif
