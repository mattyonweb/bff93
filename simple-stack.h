#ifndef STACK_H
#define STACK_H

typedef struct __node {
    int val;
    struct __node * next;
} * Node;

typedef struct __stack {
    Node top;
    int count;
} * Stack;

Node nodeInit(int val);
Stack stackInit();
void stackPush(Stack s, int el);
int stackPop(Stack s);
int stackCount(Stack s);

#endif
