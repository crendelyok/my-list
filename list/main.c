#include <stdio.h>
#include "list.c"

int main() {
	struct List_h* list = ListInit();
	for (int i = 0; i < 10; ++i) {
		ListAddLast(list, i);
	}
	for (int i = 0; i < 5; ++i) {
		NodeDestroy(list, i);
	}
	for (int i = 20; i < 30; ++i) {
		ListAddFirst(list, i);
	}
	ListDump(list);
	ListSort(list);
	fflush(stdout);
	ListDestroy(list);
	return 0;
}
