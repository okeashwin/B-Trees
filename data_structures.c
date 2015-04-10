#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include "data_structures.h"

extern int btree_order;
extern long root_offset;
extern bool debug;
extern FILE *fin;
extern int size_of_tree;
extern long leaf_offset;
stack *top=NULL;


//Stack API
void push(long offset)
{
	stack *new = (stack *)malloc(sizeof(stack));
	new->offset = offset;
	new->next = NULL;
	if(top == NULL)
		top = new;
	else
	{
		new->next = top;
		top = new; 
	}
}

long pop()
{
	if(top==NULL)
		return -1;
	else
	{
		long pop_val = top->offset;
		top = top->next;
		return pop_val;
	}
}

bool stack_is_empty()
{
	if(top==NULL)
		return true;
	else
		return false;
}

long peek()
{
	if(top==NULL)
		return -1;
	else
		return top->offset;
}

int compare(const void *a, const void *b)
{
        int *a1=(int *) a;
        int *b1=(int *) b;

        if(*a1 < *b1)
                return -1;
        else if ( *a1 == *b1)
                return 0;
        else
                return 1;
}


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

//Returns 0 if not found, else returns 1
int find_key_in_btree(char *input_filename, int key)
{
	if(root_offset==-1)	//Tree is empty
		return 0;
	else
	{
		//FILE *fin;
		//fin = fopen (input_filename,"r+b");
		if(fin==NULL)
		{
			perror("Index file open error:");
			return 0;
		}
		//Seek to 0 + sizeof(long) to point to the 1st element
		//fseek(fin,sizeof(long),SEEK_SET);
		//Go to the root
		fseek(fin,root_offset,SEEK_SET);
		//Verify if the root is reached
		if(debug)	printf("In find_key()\n");
		long curr_pos=ftell(fin);
		if(debug)
		{
			printf("Curr position : %ld\n",curr_pos);
			printf("Root offset + sizeof(long) = %ld\n",root_offset);
		}

		int keys_in_node=0;
		int key_to_test=0;
		long child_offset_to_go=0;
		/*fread(&keys_in_node, 1, sizeof(int), fin);
		if(keys_in_node==0)
		{
			//Root has no element
			if(debug)	printf("Root has no element\n");
			return 0;
		}
		*/
		//Iterate now until you hit a leaf
		//Have a pointer point to the offset where children offsets begin
		FILE *f_child_offset;
		f_child_offset = fopen ( input_filename, "rb");
		if(f_child_offset == NULL)
		{
			perror("Index file read error in find_key():");
			exit(-1);
		}
		
		while(1)
		{
			//Compare key to all the node's elements
			//btree_order holds the tree order
			fread(&keys_in_node, sizeof(int), 1, fin);
			if(debug)	printf("find_key() : Keys in node : %d\n",keys_in_node);
			if(keys_in_node==0)
			{
				//Node has no key
				if(debug)	printf("Root has no element\n");
				return 0;
			}

			fseek(f_child_offset, ftell(fin), SEEK_SET);
			//Point child offset to where it should be
			fseek(f_child_offset, (btree_order -1)*sizeof(int) , SEEK_CUR);
			long offset=ftell(f_child_offset);
			if(debug)	printf("f_child_offset : %ld\n",offset);
			if(debug)	printf("f_in : %ld\n",ftell(fin));


			int keys_read=0;
			bool greatest = false;
			bool node_full = false;
			bool smallest = false;
			key_to_test = -10;
			child_offset_to_go=0;
			while(key > key_to_test)
			//for(i=0 ; i < btree_order - 1;i++)
			{
				//Investigate each key and determine where to go
				//Check here if all the keys have been read
				if(keys_read == keys_in_node)
				{
					greatest=true;
					if(keys_read == btree_order - 1)
						node_full = true;
					break;
				}
				if(debug)	printf("find_key() : Reading from offset : %ld\n",ftell(fin));
				fread(&key_to_test, sizeof(int), 1, fin);
				if(debug)	printf("find_key() : Key_to_test : %d\n",key_to_test);
				keys_read++;
				if(key==key_to_test)
					return 1; 
			}
			if(key <= key_to_test && keys_read==1)
				smallest = true;

			//Check if the key is the greatest
			if(node_full)
			{
				//Take the child offset pointer to the last offset field
				fseek( f_child_offset, (btree_order - 1)*sizeof(long), SEEK_CUR);
				long test_off;
				test_off = ftell(f_child_offset);
				if(debug)	printf("Last child offset : %ld\n",test_off);
			}
			
			else
			{
				//Loop broken where key < key_to_test
				//Should be always the case
				if(!smallest)
				{
					if(greatest)
						fseek( f_child_offset, (keys_read)*sizeof(long), SEEK_CUR);
					else
						fseek( f_child_offset, (keys_read - 1)*sizeof(long), SEEK_CUR);
				}
				long test_off;
				test_off = ftell(f_child_offset);
				if(debug)	printf("child offset to go to: %ld\n",test_off);
			}
		
			//Checking if there is a child node at this offset
			fread( &child_offset_to_go, sizeof(long), 1, f_child_offset);
			if(child_offset_to_go==-1)
			{
				if(debug)	printf("find_key() : Returning 0 from the function as the child offset was found to be -1\n");
				return 0;
			}
			//Continue, Set the pointers
			else
			{
				//Set child_offset to the start of file
				fseek(f_child_offset, 0, SEEK_SET);

				//Set fin
				fseek(fin, 0, SEEK_SET);
				//fseek(fin, sizeof(long), SEEK_CUR);
				fseek(fin, child_offset_to_go, SEEK_CUR);
			}
		}
		
	}
}

void write_node(btree_node_ptr node)
{
	if(fin==NULL)
	{
		if(debug)	printf("In write_node() : Received a NULL file pointer\n");
		return;
	}

	//Place the file pointer in the END
	if(root_offset==-1)
		fseek(fin, sizeof(long), SEEK_SET);
	else
		fseek(fin, 0, SEEK_END);

	//btree_node_ptr node = *addr_node;

	//Write the node's #ofkeys
	if(debug)	printf("Node's keys : %d\n",node->no_of_keys);
	fwrite(&(node->no_of_keys), sizeof(int), 1, fin);

	//Write the node's keys
	int i=0;
	if(debug)	printf("Tree order : %d\n",btree_order);
	for(i=0;i < btree_order - 1;i++)
	{
		if(debug)	printf("Writing node value : %d\n",node->keys[i]);
		fwrite(&(node->keys[i]), sizeof(int), 1, fin);
	}

	//Write the children offsets now
	i=0;
	for(i=0;i<btree_order;i++)
	{
		if(debug)	printf("Writing child offset value : %ld\n",node->children_offsets[i]);
		fwrite(&(node->children_offsets[i]), sizeof(long), 1, fin);
	}

	//Place the file pointer at logical 0
	fseek(fin, sizeof(long), SEEK_SET);
}

long *populate_parents(char *input_filename, long *res, int key)
{
	if(root_offset==-1)     //Tree is empty
                return 0;
        else
        {
                if(fin==NULL)
                {
                        perror("Index file open error:");
                        return 0;
                }
                //Go to the root
                fseek(fin,root_offset,SEEK_SET);
                //Verify if the root is reached
                if(debug)       printf("In populate_parents()\n");
                long curr_pos=ftell(fin);
                if(debug)
                {
                        printf("Curr position : %ld\n",curr_pos);
                        printf("Root offset + sizeof(long) = %ld\n",root_offset);
                }

                int keys_in_node=0;
                int key_to_test=0;
                long child_offset_to_go=0;

                FILE *f_child_offset;
                f_child_offset = fopen ( input_filename, "rb");
                if(f_child_offset == NULL)
                {
                        perror("Index file read error in populate_parents():");
                        exit(-1);
                }

		long curr_offset = root_offset;
		long prev_offset = root_offset;
		int curr_node = (root_offset - sizeof(long))/sizeof(btree_node);
		int prev_node = (root_offset - sizeof(long))/sizeof(btree_node); 

                while(1)
                {
                        //Compare key to all the node's elements
                        //btree_order holds the tree order
                        fread(&keys_in_node, sizeof(int), 1, fin);
                        if(debug)       printf("populate_parents() : Keys in node : %d\n",keys_in_node);
                        if(keys_in_node==0)
                        {
                                //Node has no key
                                if(debug)       printf("Root has no element\n");
                                return 0;
                        }

                        fseek(f_child_offset, ftell(fin), SEEK_SET);
                        //Point child offset to where it should be
                        fseek(f_child_offset, (btree_order -1)*sizeof(int) , SEEK_CUR);
                        long offset=ftell(f_child_offset);
                        if(debug)       printf("f_child_offset : %ld\n",offset);
                        if(debug)       printf("f_in : %ld\n",ftell(fin));


                        int keys_read=0;
                        bool greatest = false;
                        bool node_full = false;
                        bool smallest = false;
                        key_to_test = -10;
                        child_offset_to_go=0;

                        while(key > key_to_test)
                        //for(i=0 ; i < btree_order - 1;i++)
                        {
                                //Investigate each key and determine where to go
                                //Check here if all the keys have been read
                                if(keys_read == keys_in_node)
                                {
                                        greatest=true;
                                        if(keys_read == btree_order - 1)
                                                node_full = true;
                                        break;
                                }
                                if(debug)       printf("populate_parents() : Reading from offset : %ld\n",ftell(fin));
                                fread(&key_to_test, sizeof(int), 1, fin);
                                if(debug)       printf("populate_parents() : Key_to_test : %d\n",key_to_test);
                                keys_read++;
                        }
                        if(key <= key_to_test && keys_read==1)
                                smallest = true;

                        //Check if the key is the greatest
                        if(node_full)
                        {
                                //Take the child offset pointer to the last offset field
                                fseek( f_child_offset, (btree_order - 1)*sizeof(long), SEEK_CUR);
                                long test_off;
                                test_off = ftell(f_child_offset);
                                if(debug)       printf("Last child offset : %ld\n",test_off);

				//Root node
				if(curr_node == (root_offset - sizeof(long))/sizeof(btree_node))
				{
					res[curr_node] = -10;	//Parent of root doesnt exist
					push(-10);
				}
				else
				{
					res[curr_node] = prev_offset;
					//Pushing the parent onto the stack
					push(prev_offset);
				}
                        }

                        else
                        {
                                //Loop broken where key < key_to_test
                                //Should be always the case
                                if(!smallest)
                                {
                                        if(greatest)
                                                fseek( f_child_offset, (keys_read)*sizeof(long), SEEK_CUR);
                                        else
                                                fseek( f_child_offset, (keys_read - 1)*sizeof(long), SEEK_CUR);
                                }
                                long test_off;
                                test_off = ftell(f_child_offset);
                                if(debug)       printf("child offset to go to: %ld\n",test_off);

				//Root node
				if(curr_node == (root_offset - sizeof(long))/sizeof(btree_node))
				{
					res[curr_node] = -10;	//Parent of root doesnt exist
					push(-10);
				}
				else
				{
					res[curr_node] = prev_offset;
					push(prev_offset);
				}
                        }

                        //Checking if there is a child node at this offset
                        fread( &child_offset_to_go, sizeof(long), 1, f_child_offset);
                        if(child_offset_to_go==-1)
                        {
                                if(debug)       printf("populate_parents() : Returning 0 from the function as the child offset was found to be -1\n");
				leaf_offset = curr_offset;
                                break;
                        }
                        //Continue, Set the pointers
                        else
                        {
                                //Set child_offset to the start of file
                                fseek(f_child_offset, 0, SEEK_SET);

                                //Set fin
                                fseek(fin, 0, SEEK_SET);
                                //fseek(fin, sizeof(long), SEEK_CUR);
                                fseek(fin, child_offset_to_go, SEEK_CUR);
                        }

			//Update the offset variables
			prev_node = curr_node;
			prev_offset = curr_offset;
			curr_node = (ftell(fin) - sizeof(long))/sizeof(btree_node);
			curr_offset = ftell(fin);
                }

        }

	return res;
}

void insert_in_node( long leaf_offset, int node_keys, int key)
{
	//Place the fp at the 1st integer read
	fseek(fin, leaf_offset + sizeof(int) , SEEK_SET);
        int temp_arr[node_keys + 1];
        int i=0;
        for(i=0;i<node_keys + 1;i++)
        	temp_arr[i]=-1;
        int key_read = 0;
       	//fin points to the 1st integer that we can read
        for(i=0 ; i < node_keys; i++ )
        {
        	fread(&key_read, sizeof(int), 1, fin);
                if(debug)       printf("Add at root : %d\n", key_read);
                temp_arr[i] = key_read;
        }
        
	temp_arr[i] = key;

        //Sort temp_arr
        qsort(temp_arr, node_keys + 1,sizeof(int), compare);

        //Get the fp back
        fseek(fin, (-1 * node_keys )*sizeof(int), SEEK_CUR);
        fseek(fin, -1*sizeof(int), SEEK_CUR);
        int new_keys = node_keys + 1;
        fwrite(&new_keys, sizeof(int), 1, fin);

        i=0;
        //Start writing back
        for(i=0;i < node_keys + 1;i++)
        {
        	if(debug)       printf("Add at root : Key %d at offset %ld\n",temp_arr[i],ftell(fin));
             	fwrite(&temp_arr[i] , sizeof(int), 1, fin);
        }
                                                
	//Increment keys_in_root for this node
        //Reset fp
                                                
	fseek(fin, 0, SEEK_SET);
}

long get_child_after_split(int arr[], int size, int split_value)
{
	if(fin==NULL)	
	{
		printf("get_child_after_split() : Received a NULL file pointer\n");
		return -1;
	}

	int iter=0;
	for (iter =0; arr[iter] <= split_value ;iter++)
	{}

	//Start writing from index iter
	btree_node_ptr new = new_node_init();
	//Update the keys of this new node
	//int size = sizeof(arr) / sizeof(int);
	if(debug)	printf("Size : %d\n",size);
	int k=0;
	for(;iter < size ; iter++)
	{
		(new)->keys[k] = arr[iter];
		if(debug)	printf("Iter : %d\tWrote this to the new node : %d\n",iter,(new)->keys[k]);
		k++;
	}

	//No need to change the node's child offsets;
	//Change the no. of keys value of the node
	(new)->no_of_keys = k;

	//Write this node to the EOF
	fseek(fin, 0, SEEK_END);
	long right_offset = ftell(fin);
	//btree_node_ptr node = *new;
	if(debug)	printf("%d\n",(new)->no_of_keys);
	if(debug)	printf("Right_offset : %ld\n",right_offset);
	write_node(new);
	if(debug)	printf("Returning right_offset : %ld\n",right_offset);
	return right_offset;
}
//Prints a node's contents at offset offset
void print_node_at_offset(long offset)
{
	fseek(fin, offset, SEEK_SET);
	int int_val;
	long long_val;	
	fread(&int_val, sizeof(int), 1, fin);
	printf("Number of keys in the nodes : %d\n",int_val);
	int i=0;
	for(i=0;i < btree_order - 1;i++)
	{
		fread(&int_val, sizeof(int), 1, fin);
		printf("Node's keys : %d\n",int_val);
	}

	for(i=0; i<btree_order; i++)
	{
		fread(&long_val, sizeof(long), 1, fin);
		printf("Node's child offsets : %ld\n",long_val);
	}

	fseek(fin,0,SEEK_SET);
}

void update_left_child(int temp_arr[], int size, long leaf_offset, int split_value)
{
	if(fin==NULL)
	{
		printf("update_left_child(): Received a NULL file pointer \n");
		return;
	}

	fseek(fin,leaf_offset,SEEK_SET);
	//int size = sizeof(temp_arr) / sizeof(int);
	int iter =0;
	for(iter = 0; temp_arr[iter] < split_value ; iter++)
	{}

	//Make all the values now as -1
	int test_key = iter;
	if(debug)	printf("Updated number of keys in the left child : %d\n", test_key);
	fseek(fin, leaf_offset , SEEK_SET);
	
	//Update the 'number of keys in the node'
	fwrite( &test_key, sizeof(int), 1, fin);

	//Take it to split node
	fseek(fin, (iter)*sizeof(int), SEEK_CUR);
	test_key = -1;

	for(; iter < btree_order - 1 ;iter ++)
	{
		if(debug)	printf("%d Written at offset : %ld\n",test_key, ftell(fin));
		fwrite(&test_key, sizeof(int), 1, fin);
	}
	
	//Update the child offsets now
	//Set them to -1
	long long_read = 0;
	long long_write = -1;
	int i=0;
	for(i=0;i<btree_order;i++)
	{
		fread(&long_read, sizeof(long), 1, fin);
		if(long_read != -1)
		{
			fseek(fin, -1*sizeof(long), SEEK_CUR);
			fwrite(&long_write, sizeof(long), 1, fin);
		}
	}
	fseek(fin, 0, SEEK_SET);
}

void check_parent_and_update(int split_value, long left_child_offset, long right_child_offset)
{
	if(fin==NULL)
	{
		printf("check_parent_and_update() : Received a NULL file pointer\n");
		return;
	}
	long off = pop();
	if(debug)	printf("Offset received : %ld\n",off);

}
