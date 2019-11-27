//Doubly linked list
//All nodes are located in one block struct Node* nodes

#ifndef LIST_H
#define LIST_H

typedef int elem_t;

#define LIST_SIZE 8

#define FREE 0
#define NOT_FREE 1

#define POISON 666666

struct Node {
	elem_t data;
	int next;
	int prev;
	int free; // = NOT_FREE if the node is occupied otherwise FREE
};

struct List_h {
	int head;      //position of the first element in *nodes, TAIL by default
	int tail;      //position of the last element in *nodes, HEAD by default
	               //TAIL and HEAD are defined in "list.c"
	int size;      //number of nodes in list
	int maxsize;   //max nubmer of nodes in allocated block
	int free;      //number of free space in array struct Node* nodes
	int is_sorted; //ListSort sorts elements in *nodes
		       //so that physical address of the 
		       //node is equal to logical 
		       //e.g. FindPosByOrder computation complexity is O(1)
		       //if is_sorted == 1
	struct Node* nodes;
};

struct List_h* ListInit();
void ListDestroy     (struct List_h* list);

void NodeDestroy     (struct List_h* list, int num);
int FindFreePosition (struct List_h* list, struct Node **nodes);
int InsertAfter      (struct List_h* list, int num, elem_t val);
int ListAddFirst     (struct List_h* list, elem_t val);
int ListAddLast      (struct List_h* list, elem_t val);

int FindPosByValue   (struct List_h* list, elem_t val);
int FindPosByOrder   (struct List_h* list, int num);
int GetPosFirstOrder (struct List_h* list);
int GetPosLastOrder  (struct List_h* list);

void ListSort        (struct List_h* list);
int ListOk           (struct List_h* list);
int ListDump         (struct List_h* list);

#endif //LIST_H
