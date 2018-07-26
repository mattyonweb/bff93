#include <stdio.h>
#include <stdlib.h>
#include "datastruct.h"

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

Stack stackCopy(Stack s) {
    Stack out = stackInit();

    for (Node n=s->top; n != NULL; n = n->next)
        stackPush(out, n->val);

    return out;
}

int stackPop(Stack s) {
    if ((s -> count) <= 0) {
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

void linkedExpand(Node root, int elem, int newElem) {
    for (Node n=root; n != NULL; n = n->next) {
        if (n->val == elem) {
            Node newNode = nodeInit(newElem);
            newNode -> next = n -> next;
            n -> next = newNode;
            return;
        }
    }
    printf("[%s]\tElemento non trovato!\n", __PRETTY_FUNCTION__);
}

void linkedRemove(Node root, int elem) {
    if (root -> val == elem) {      // BUG: non sono mica sicuro che questo modifichi root...
        *root = *(root -> next);
        return;
    }
    
    for (Node n=root; n != NULL; n = n->next) { //BUG: mancano i free()
        if (n->next->val == elem) {
            n->next = n->next->next;
            return;
        }
    }
}

void linkedPrint(Node root) {
    for (Node n=root; n!=NULL; n=n->next) printf("%d,", n->val);
    printf("\n");
}

char linkedFind(Node root, int v) {
    for (Node n=root; n!=NULL; n=n->next) {
        if (n -> val == v) return 1;
    }
    return 0;
}
