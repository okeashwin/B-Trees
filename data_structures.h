#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

typedef struct
{
	int no_of_keys;	//The number of keys this node holds
	int *keys;	//Pointer to the Node's keys
	long *children_offsets;	//Pointer to the childrens' offsets in the file
}btree_node, *btree_node_ptr, **btree_node_dptr;

typedef struct node
{
	long offset;
	struct node *next;
}stack;


//-----------------------Stack API
void push(long offset);

//Returns -1 if stack is empty
long pop();

//Returns true if empty
bool stack_is_empty();

//Returns the top of the stack, -1 if stack is empty
long peek();

//Returns a new initialized node
//All keys set to -1, All offsets set to -1
btree_node_ptr new_node_init();

//find a given key in the btree
int find_key_in_btree(char *input_filename,int key);

//Write a node's contents to the index file
void write_node(btree_node_ptr node);

//Populate parent pointers during a tree walk
long* populate_parents(char *input_filename, long *res, int key);

int compare(const void *a, const void *b);

void insert_in_node( long leaf_offset, int node_keys, int key);

//Creates a new node for the right child and returns its offset
long get_child_after_split(int arr[], int size, int split_value);

//Prints a node's contents at offset offset
void print_node_at_offset(long offset);

//Updates the left child of the btree_node in case of a split
void update_left_child(int temp_arr[], int size, long leaf_offset, int split_value);

//Change root node. *
void check_parent_and_update(int split_value, long left_child_offset, long right_child_offset);
	
