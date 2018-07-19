#include <stdio.h>
#include <stdlib.h>
#include "simple-stack.h"

Node nodeInit(int val) {
    Node node = malloc(sizeof(struct __node));
    node -> val = val;
    node -> next = NULL;

    return node;
}

Stack stackInit() {
    Stack stack = malloc(sizeof(struct __stack));
    stack -> top = NULL;
    stack -> count = 0;

    return stack;
}

void stackPush(Stack s, int val) {
    Node node = nodeInit(val);
    node -> next = s -> top;
    s -> top = node;
    (s -> count)++;
}

int stackPop(Stack s) {
    if ((s -> count) == 0) {
        return 0;   // http://www.nsl.com/papers/befunge93/befunge93.htm
    }
    (s -> count)--;
    Node node = s -> top;
    s -> top = node -> next;
    int result = node -> val;
    free(node);
    return result;
}

int stackCount(Stack s) {
    return s -> count;
}

void stackPrint(Stack s) {
    Node n = s -> top;
    printf("\t[");
    while (n != NULL) {
        printf("%d,", n->val);
        n = n-> next;
    }
    printf("]\n");
}
