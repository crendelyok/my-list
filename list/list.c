#include "list.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#ifndef _LIST_C
#define _LIST_C

#define SECURE_LIST

#define TAIL -1
#define HEAD -2

#ifdef SECURE_LIST
#define SECURE if(1)
#else
#define SECURE if(0)
#endif //SECURE_LIST

//Initializes List_h list
//and returns pointer for List_h* 
struct List_h* ListInit() {
	struct List_h* list = calloc(1, sizeof(struct List_h));
	assert(list != NULL);

	list -> size = 0;

	list -> nodes = calloc(LIST_SIZE, sizeof(struct Node));
	assert(list -> nodes != NULL);

	list -> head = TAIL;
	list -> tail = HEAD;
	list -> maxsize = LIST_SIZE;
	list -> free = 0;
	list -> is_sorted = 1;
	return list;
}

//Frees List_h* list and list -> nodes
void ListDestroy(struct List_h* list) {
	assert(list != NULL);
	if (list -> nodes != NULL) {
		free(list -> nodes);
	}
	free(list);
}

//Unlinks the node with position num in list -> nodes
//from the List_h *list
void NodeDestroy(struct List_h* list, int num) {
	assert(list != NULL);
	assert(list -> nodes != NULL);

	if (num >= list -> maxsize) 
		return;
	if ((list -> nodes)[num].free == FREE) 
		return;
	if (((list -> nodes)[num].next == TAIL) && 
	     (list -> nodes)[num].prev == HEAD) {
		list -> head = TAIL;
		list -> tail = HEAD;		
		(list -> nodes)[num].data = POISON;
		list -> size --;
		(list -> nodes)[num].free = FREE;
		list -> is_sorted = 0;
		return;
	}
	if ((list -> nodes)[num].next == TAIL) {
		list -> tail = (list -> nodes)[num].prev;
		(list -> nodes)[(list -> nodes)[num].prev].next = TAIL;
		(list -> nodes)[num].free = FREE;
		(list -> nodes)[num].data = POISON;
		list -> is_sorted = 0;
		list -> size --;
		return;
	}
	if ((list -> nodes)[num].prev == HEAD) {
		list -> head = (list -> nodes)[num].next;
		(list -> nodes)[(list -> nodes)[num].next].prev = HEAD;
		(list -> nodes)[num].free = FREE;
		(list -> nodes)[num].data = POISON;
		list -> size --;
		list -> is_sorted = 0;
		return;
	}
	(list -> nodes)[(list -> nodes)[num].next].prev = (list -> nodes)[num].prev;
	(list -> nodes)[(list -> nodes)[num].prev].next = (list -> nodes)[num].next;
	(list -> nodes)[num].free = FREE;
	(list -> nodes)[num].data = POISON;
	list -> size --;
	list -> is_sorted = 0;
	return;
}

//Returns position in list -> nodes, with node.free == FREE
//If there is no such position, reallocates list -> nodes
#define new_node ((list -> nodes)[list -> free])
int FindFreePosition(struct List_h* list, struct Node **nodes) {
	assert(nodes != NULL);
	assert(list != NULL);
	assert(ListOk(list));

	if (list -> free < list -> maxsize) { 
		if (new_node.free == FREE) {
			return (list -> free);
		}
		else {
			for (int i = 0; i < list -> maxsize; ++i)
				if (new_node.free == FREE)
					return i;	
		}
	}
	//reallocation list -> nodes
	struct Node* AllocRet = realloc(*nodes, 
			                sizeof(struct Node) * (list -> maxsize) * 2);
	assert(AllocRet != NULL);
	*nodes = AllocRet;
	for (int i = list -> maxsize; i < (list -> maxsize) * 2; ++i) {
		(*nodes)[i].free = FREE;
	}
	list -> maxsize *= 2;
	list -> free = list -> maxsize / 2;	

	return list -> free;	
}
#undef new_node

//creates node in list -> nodes with node.data = val
//adds this node after the node with position = num in the List_h *list
//returns position of the new node in list -> nodes
int InsertAfter(struct List_h* list, int num, elem_t val) {
	assert(list != NULL);
	assert(list -> nodes != NULL);
	assert ((list -> nodes)[num].free != FREE);
	assert(ListOk(list));

	int pos = FindFreePosition(list, &(list -> nodes));
	struct Node* cur_node = &((list -> nodes)[pos]);	
	cur_node -> data = val;
	cur_node -> prev = num;
       	cur_node -> next = (list -> nodes)[num].next;	
	cur_node -> free = NOT_FREE;

	(list -> nodes)[(list -> nodes)[num].next].prev = pos;
	(list -> nodes)[num].next = pos;

	list -> size ++;
	list -> free ++;
	list -> is_sorted = 0;

	assert(ListOk(list));
	return pos;
}

//creates node in list -> nodes with node.data = val
//adds this node in the beginning of the List_h *list
//returns position of the new node in list -> nodes
int ListAddFirst(struct List_h* list, elem_t val) {	
	assert(list != NULL);
	assert(list -> nodes != NULL);
	assert(ListOk(list));

	int pos = FindFreePosition(list, &(list -> nodes));

	struct Node* cur_node = &((list -> nodes)[pos]);
	cur_node -> data = val;
	cur_node -> prev = HEAD;

	if (list -> size != 0) {
		cur_node -> next = list -> head;
		(list -> nodes)[list -> head].prev = pos;
	}
	else {
		cur_node -> next = TAIL;
		list -> tail = pos;

	}

	cur_node -> free = NOT_FREE;

	list -> head = pos;

	list -> size ++;
	list -> free ++;
	list -> is_sorted = 0;
	assert(ListOk(list));
	return pos;
}

//creates node in list -> nodes with node.data = val
//adds this node in the end of the List_h *list
//returns position of the new node in list -> nodes
int ListAddLast(struct List_h* list, elem_t val) {	
	assert(list != NULL);
	assert(list -> nodes != NULL);
	assert(ListOk(list));

	int pos = FindFreePosition(list, &(list -> nodes));
	struct Node* cur_node = &((list -> nodes)[pos]);
	cur_node -> data = val;
	cur_node -> next = TAIL;
	if (list -> size != 0) {
		cur_node -> prev = list -> tail;
		(list -> nodes)[list -> tail].next = pos;
	}
	else {
		cur_node -> prev = HEAD;
		list -> head = pos;
	}

	cur_node -> free = NOT_FREE;

	list -> tail = pos;

	list -> size ++;
	list -> free ++;
	assert(ListOk(list));
	return pos;
}

//Returns position of the first node
//with node.data = val in list -> nodes  
//Returns -1 if element with such value doesn't exists
int FindPosByValue(struct List_h* list, elem_t val) {
	assert(list != NULL);
	assert(list -> nodes != NULL);
	assert(ListOk(list));

	for (int i = 0; i < list -> size; ++i) {
		if ((list -> nodes)[i].data == val)
			return i;
	}

	return -1;	
}

//Finds and returns position in list -> nodes 
//of the node with logical order == num
//Returns -1 if there is no such node
//If num == 1, finds list -> head
int FindPosByOrder(struct List_h* list, int num) {
	assert(list != NULL);
	assert(list -> nodes != NULL);
	assert(ListOk(list));

	assert(num > 0);
	if (list -> is_sorted) {
		return num;
	}
	if (list -> size < num) {
		return -1;
	}	
	int cur = list -> head;
	int i = 1;
	while ((list -> nodes)[cur].next != TAIL) {
		if (i == num)
			return cur;
		++i;
		cur = (list -> nodes)[cur].next;
	}
	return -1;
}

//Returns position in list -> nodes of the first element in list by order
//-1 if there is no such node
int GetPosFirstOrder(struct List_h* list) {
	assert(list != NULL);
	assert(ListOk(list));
	
	if (list -> size == 0)
		return -1;
	return (list -> head);
}

//Returns position in list -> nodes of the first element in list by order
//-1 if there is no such node
int GetPosLastOrder(struct List_h* list) {
	assert(list != NULL);
	assert(ListOk(list));
	
	if (list -> size == 0)
		return -1;
	return (list -> tail);
}

//Sorts elements in list -> nodes, so that logical and physical oreders
//appeared the same
void ListSort(struct List_h* list) {
	assert(list != NULL);
	assert(list -> nodes != NULL);
	assert(ListOk(list));

	if (list -> is_sorted) 
		return;

	if (list -> size == 0) {
		list -> free = 0;
		list -> is_sorted = 1;
		list -> head = TAIL;
		list -> tail = HEAD;
		return;
	}
	
	
	struct Node* tmp = calloc(list -> maxsize, sizeof(struct Node));	
	assert(tmp != NULL);

	for (int i = 0; i < list -> maxsize; ++i) {	
		tmp[i] = (list -> nodes)[i];	
	}
	int i = 0;
	int cur = list -> head;
	while (cur != TAIL) {
		(list -> nodes)[i].next = i + 1;
		(list -> nodes)[i].prev = i - 1;
		(list -> nodes)[i].data = tmp[cur].data;
		(list -> nodes)[i].free = 1;
		cur = tmp[cur].next;
		++i;
	}	
	for (int j = i; j < list -> maxsize; ++j) {
		(list -> nodes)[j].free = 0;
	}

	list -> head = 0;
	list -> tail = list -> size - 1;
	
	(list -> nodes)[0].prev = HEAD;
	(list -> nodes)[(list -> size) - 1].next = TAIL;	
	
	list -> free = list -> size;
	list -> is_sorted = 1;
	free(tmp);
	assert(ListOk(list));
}

//returns 0 if list is damaged
//otherwise returns 1
int ListOk(struct List_h* list) {
	assert(list != NULL);

	if (list -> nodes == NULL)
		return 0;

	if (list -> size == 0) {
		if ((list -> head != TAIL) || (list -> tail != HEAD)) 
			return 0;
	}

	if (list -> size > list -> maxsize) 
		return 0;

	SECURE {
		if (list -> size == 0) {
			for (int i = 0; i < list -> maxsize; ++i) {
				if ((list -> nodes)[i].free == NOT_FREE)
					return 0;			
			}
		}
		int iter = 0;
		int cur = list -> head;
		while (cur != TAIL) {
			if ((list -> nodes)[cur].next >= list -> maxsize)
				return 0;	
			if ((list -> nodes)[cur].next == HEAD)
				return 0;
			if ((list -> nodes)[cur].prev == TAIL)
				return 0;
			if (iter > list -> size) 
				return 0;
			iter++;
			cur = (list -> nodes)[cur].next;
		}
		if (iter != list -> size) {
			printf("WOOP");
			return 0;
		}
	}
	return 1;
}

//Dump for elem_t == int
int ListDump(struct List_h* list) {
	assert(list != NULL);
	assert(list -> nodes != NULL);
	if (!ListOk(list)) {
		printf("\n!!LIST IS NOT OKAY!!\n");
	}

/*------Dump in terminal---------*/
	printf("\nlist_h dump {\n");
	printf("\t*head = %d\n", list -> head);
	printf("\t*tail = %d\n", list -> tail);
	printf("\tsize = %d\n", list -> size);
	printf("\tmaxsize = %d\n", list -> maxsize);
	printf("\tfree = %d\n", list -> free);
	printf("\t*nodes = %p\n", list -> nodes);
	printf("}\n");
	
	printf("logically:\n");
	if (list -> head != TAIL) {
		printf("head <-> %d\n", (list -> nodes)[list -> head].data);
	}
	else {
		printf("head <-> tail\n");	
	}
	int cur = list -> head;
	if (cur != TAIL) {
		int k = 0;
		while ((list -> nodes)[cur].next != TAIL) {
			if (k == list -> maxsize)
				break;
			//logical number: { value } -> [position in list -> nodes]
			printf("%2d: { %2d } -> [%2d]\n", k, (list -> nodes)[cur].data,
							     cur);
			cur = (list -> nodes)[cur].next;
			++k;
		}
	}
	if (list -> tail != HEAD)
		printf("%d <-> tail", (list -> nodes)[cur].data);

	
	printf("\n\nphysically:\n\n");
	for (int i = 0; i < list -> maxsize; ++i) {
		if ((list -> nodes)[i].free == 0) {
			printf("%d: EMPTY\n", i);
			continue;
		}
		//[position in list -> nodes]: { value } pos of prev -> pos -> pos of next
		printf("%2d: { %6d } %d -> %d -> %d\n", i, 
						      (list -> nodes)[i].data,
						      (list -> nodes)[i].prev,
						      i,
						      (list -> nodes)[i].next); 
	}

	printf("------------------------\n");
	
/*------Dump as png using graphviz-------*/
	FILE* dump_f = fopen("dump", "w");
	fprintf(dump_f, "digraph List {\n");
	fprintf(dump_f, "\tnode [style=\"filled\", margin=\"0.01\"]\n");
	fprintf(dump_f, "\trankdir = \"LR\"\n");
	fprintf(dump_f, "\tedge [dir=\"both\"]\n");
	
	if (list -> size == 0) {
		fprintf(dump_f, "\t\"Head\" -> \"TAIL\"\n");
		fprintf(dump_f, "}");
		fclose(dump_f);
		system("dot -Tpng dump -odump.png");
		system("mimeopen dump.png");
		return 0;	
	}

	fprintf(dump_f, "\t\"Head\" -> \"%d\"\n", (list -> nodes)[list -> head].data);
	cur = list -> head;
	if (cur != TAIL) {
		while ((list -> nodes)[cur].next != TAIL) {
			fprintf(dump_f, "\t\"%d\" -> \"%d\"\n",
				      	(list -> nodes)[cur].data, 
					(list -> nodes)[(list -> nodes)[cur].next].data);

			cur = (list -> nodes)[cur].next;	
		}
	}
	if ((cur != HEAD) && (cur != TAIL)) {
		fprintf(dump_f, "\t\"%d\" -> \"TAIL\"\n", (list -> nodes)[cur].data);	
	}
	fprintf(dump_f, "}");

	fclose(dump_f);

	system("dot -Tpng dump -odump.png");
	system("mimeopen dump.png");
	return 0;
}

#undef __POISON

#endif //_LIST_H
