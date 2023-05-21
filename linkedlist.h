#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct node {
    int data;
    struct node *next;
} node;

struct linkedlist {
    node *head;
    int size;
} typedef linkedlist;

linkedlist *newLinkedList()
{
    linkedlist *ll = (linkedlist *) malloc(sizeof(linkedlist));
    ll->head = NULL;
    ll->size = 0;
    return ll;
}

void insertSorted(linkedlist *ll, int data)
{
    node *newNode = (node *) malloc(sizeof(node));
    newNode->data = data;
    newNode->next = NULL;
    if(!ll->size){
        ll->head=newNode;
        ll->size++;
        return;
    }
    node *prev = NULL;
    node *curr = ll->head;
    while (curr && data > curr->data){
        prev = curr;
        curr = curr->next;
    }
    if(!prev){
        newNode->next = ll->head;
        ll->head = newNode;
    }
    else {
        prev->next = newNode;
        newNode->next = curr;
    }
    ll->size++;
}

int deleteNode(linkedlist *ll, int data)
{
    if(!ll->size){
        return -1;
    }
    node *curr = ll->head;
    node *prev = NULL;
    while (curr && data != curr->data) {
        prev = curr;
        curr = curr->next;
    }
    if (curr) {
        if (curr == ll->head) {
            ll->head = ll->head->next;
        }
        else {
            prev->next = curr->next;
        }
        free(curr);
        ll->size--;
        if(!ll->size){
            ll->head = NULL;
        }
        return 0;
    }
    else {
        return -1;
    }
}

int nodeExists(linkedlist *ll, int data)
{
    if (ll->size == 0) {
        return 0;
    }
    node *curr = ll->head;
    while (curr && data != curr->data) {
        curr = curr->next;
    }
    if (curr) {
        return 1; 
    }
    return 0;
}

int isEmpty(linkedlist *ll)
{
    return ll->size==0;
}