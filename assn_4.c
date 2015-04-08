#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<errno.h>
#include "data_structures.h"
#define DEF 100
#define CMD_LEN 1024

int btree_order = 0;
long root_offset = 0;
bool debug=true;
FILE *fin = NULL;

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


int main(int argc, char *argv[])
{
	if(argc < 3)
	{
		printf("Insufficient number of arguments supplied\n");
		exit(-1);
	}

	if(argc > 3)
	{
		printf("Too many arguments supplied\n");
		exit(-1);
	}

	char index_file[DEF];
	memset(index_file,0,sizeof(char)*DEF);
	if(argv[1]==NULL)
	{
		printf("Received a NULL index filename");
		exit(-1);
	}
	//Index file
	strcpy(index_file,argv[1]);

	if(argv[2]==NULL)
	{
		printf("Received a NULL B Tree order");
		exit(-1);
	}
	//B tree order
	btree_order=atoi(argv[2]);

	//debug bool
	//bool debug=true;
	if(debug)	printf("Index filename received : %s\n",index_file);
	if(debug)	printf("B Tree order : %d\n",btree_order);
	bool cmdIsValid=false;
	//File needs to be opened here
	//FILE *fin;
	fin = fopen (index_file, "r+b");
	if(fin == NULL)
	{
		root_offset = -1;
		//Write -1 to the file
		fin = fopen (index_file, "w+b");
		if(fin == NULL)
		{
			perror("Index file open error: ");
			exit(-1);
		}
		//else
		//	fwrite(&root_offset, sizeof(long), 1, fin);
	}
	else
		fread(&root_offset, sizeof(long), 1, fin);

	char *cmd=(char *)malloc(sizeof(char)*CMD_LEN);
	char *temp_cmd=(char *)malloc(sizeof(char)*CMD_LEN);
	char *temp2_cmd=(char *)malloc(sizeof(char)*CMD_LEN);
	memset(cmd,0,sizeof(char)*CMD_LEN);
	memset(temp_cmd,0,sizeof(char)*CMD_LEN);
	memset(temp2_cmd,0,sizeof(char)*CMD_LEN);

	while(1)
	{
		cmdIsValid=false;
		int ret;
		size_t len = sizeof(char)*CMD_LEN;
		memset(cmd,0,sizeof(char)*CMD_LEN);
		memset(temp_cmd,0,sizeof(char)*CMD_LEN);
		memset(temp2_cmd,0,sizeof(char)*CMD_LEN);
		ret=getline(&cmd,&len,stdin);
		if(ret==-1)
		{
			perror("Command Read Error :");
			exit(-1);
		}
		
		//Strip \n
		cmd[strlen(cmd)-1]='\0';
		if(strlen(cmd)==0)	continue;
	
		if(debug)	printf("getline returns : %s\n",cmd);
		strcpy(temp_cmd,cmd);
		strcpy(temp2_cmd,cmd);
		char *tok=strtok(temp_cmd," ");
		if(strcmp(tok,"add")==0 || strcmp(tok,"find")==0 || strcmp(tok,"print")==0 || strcmp(tok,"end")==0)
			cmdIsValid=true;
		if(cmdIsValid)
		{
		if(strcmp(tok,"add")==0)
		{
			tok=strtok(NULL," ");
			if(tok==NULL)
			{
				printf("Key value not supplied to 'add' utility\n");
				continue;
			}
			int key=atoi(tok);
			int found=0;
			found=find_key_in_btree(index_file, key);
			if(found)
				printf("Entry with key=%d already exists\n",key);
			else
			{
				//Add logic begins here
				if(root_offset==-1)
				{
					//The tree is empty. Construct the root;
					btree_node_dptr root = new_node_init();
					(*root)->keys[0] = key;
					(*root)->no_of_keys++;
					if(debug)	printf("Root data : %d\t%d\n",(*root)->keys[0],(*root)->no_of_keys);
					//Write this node to the file
					btree_node_ptr node= *root;
					//memcpy(node, *root, sizeof(btree_node));
					write_node(node);
					root_offset = 0;
				}
				
				//Tree is not empty
				else
				{
					//Check if the root is not full
					fseek(fin, sizeof(long), SEEK_SET);
					fseek(fin, root_offset, SEEK_CUR);
					int keys_in_root=0;
					fread(&keys_in_root, sizeof(int), 1, fin);
					if(debug)	printf("keys_in_root : %d\n",keys_in_root);
					if(keys_in_root == btree_order - 1)	//Root is Full
						printf("Root is Full \n");
					else
					{
						int temp_arr[keys_in_root + 1];
						int i=0;
						for(i=0;i<keys_in_root + 1;i++)
							temp_arr[i]=-1;
						int key_read = 0;
						//fin points to the 1st integer that we can read
						for(i=0 ; i < keys_in_root; i++ )
						{
							fread(&key_read, sizeof(int), 1, fin);
							if(debug)	printf("Add at root : %d\n", key_read);
							temp_arr[i] = key_read;
						}
						temp_arr[i] = key;

						//Sort temp_arr
						qsort(temp_arr, keys_in_root + 1,sizeof(int), compare);

						//Get the fp back
						fseek(fin, (-1 * keys_in_root )*sizeof(int), SEEK_CUR);
						fseek(fin, -1*sizeof(int), SEEK_CUR);
						int new_keys = keys_in_root + 1;
						fwrite(&new_keys, sizeof(int), 1, fin);

						i=0;
						//Start writing back
						for(i=0;i < keys_in_root + 1;i++)
						{
							if(debug)	printf("Add at root : Key %d at offset %ld\n",temp_arr[i],ftell(fin));
							fwrite(&temp_arr[i] , sizeof(int), 1, fin);
						}
						//Increment keys_in_root for this node
						//Reset fp
						fseek(fin, 0, SEEK_SET);
					} 
				}
			}
			
		}

		else if(strcmp(tok,"find")==0)
		{
			
			tok=strtok(NULL," ");
			if(tok==NULL)
			{
				printf("Key value not supplied to 'find' utility\n");
				continue;
			}
			int key=atoi(tok);
			bool found=find_key_in_btree(index_file, key);
			if(found)
				printf("Entry with key=%d exists\n",key);
			else
				printf("Entry with key=%d does not exist\n",key);
		}

		else if(strcmp(tok,"print")==0)
		{
		}

		else if(strcmp(tok,"end")==0)
		{
			//Write the updates root offset
			fseek(fin, 0, SEEK_SET);
			fwrite(&root_offset, sizeof(long), 1, fin);
			fclose(fin);
			break;
		}
	
		else
			printf("Invalid command received\n");
		
		}
		else
			printf("Invalid command received\n");
	}
	return 0;
}
