#include<stdio.h>
#include<stdlib.h>
typedef struct
{
	int no_of_keys;	//The number of keys this node holds
	int *keys;	//Pointer to the Node's keys
	long *children_offsets;	//Pointer to the childrens' offsets in the file
}btree_node, *btree_node_ptr;

//Returns a new initialized node
//All keys set to -1, All offsets set to -1
btree_node_ptr new_node_init();
	
