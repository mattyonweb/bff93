#ifndef DATASTRUCT_H
#define DATASTRUCT_H

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
Stack stackCopy(Stack s);
int stackCount(Stack s);
void stackPrint(Stack s);

void linkedExpand(Node root, int elem, int newElem);
void linkedPrint(Node root);
void linkedRemove(Node root, int elem);
#endif
