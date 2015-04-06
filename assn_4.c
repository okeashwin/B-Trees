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
	bool debug=true;
	if(debug)	printf("Index filename received : %s\n",index_file);
	if(debug)	printf("B Tree order : %d\n",btree_order);
	bool cmdIsValid=false;
	//File needs to be opened here
	FILE *fin;
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
		else
			fwrite(&root_offset, sizeof(long), 1, fin);
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
		}

		else if(strcmp(tok,"print")==0)
		{
		}

		else if(strcmp(tok,"end")==0)
		{
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