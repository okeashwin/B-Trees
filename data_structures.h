#include<stdio.h>
#include<stdlib.h>
typedef struct
{
	int no_of_keys;	//The number of keys this node holds
	int *keys;	//Pointer to the Node's keys
	long *children_offsets;	//Pointer to the childrens' offsets in the file
}btree_node, *btree_node_ptr, **btree_node_dptr;

//Returns a new initialized node
//All keys set to -1, All offsets set to -1
btree_node_dptr new_node_init();

//find a given key in the btree
int find_key_in_btree(char *input_filename,int key);

//Write a node's contents to the index file
void write_node(btree_node_ptr node);
	
