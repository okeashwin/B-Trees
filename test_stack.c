#include<stdio.h>
#include<stdlib.h>
#include "data_structures.h"
int btree_order = 0;
long root_offset = 0;
bool debug=true;
FILE *fin = NULL;
int size_of_tree=0;
long leaf_offset = 0;

int main()
{
	long i=0;
	for(i=0;i<10;i++)
		push(i);
	for(i=0;i<20;i++)
		printf("%ld\n",pop());
	return 0;
}
