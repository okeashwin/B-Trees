#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include "data_structures.h"

extern int btree_order;
extern long root_offset;

btree_node_ptr new_node_init()
{
	btree_node_ptr new= (btree_node_ptr)malloc(sizeof(btree_node));
	new->no_of_keys = 0;
	new-> keys = ( int *) malloc(sizeof(int)*(btree_order - 1));
	//Initialize all of them to -1
	int i=0;
	for(i=0;i<btree_order-1;i++)
		new->keys[i] = -1;
	
	new->children_offsets = (long *) malloc(sizeof(long)*btree_order);
	//Initialize all of them to -1
	for(i=0;i<btree_order;i++)
		new->children_offsets[i] = -1;

	return new;
}
