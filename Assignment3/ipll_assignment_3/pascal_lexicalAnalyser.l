/*The below block contains c code useful for lexical analysis*/
%{
	#include<stdio.h>
	#include<stdlib.h>
	#include<string.h>
	#include<stdbool.h>
	#define TABSIZE 100
	bool isVarEncountered = false;
	bool isBeginEncountered = false;
	bool isSecondTokenEncountered =false;
	int yyval;
	bool first;
	struct Node
	{
		char* token_specifier;
		int token_code;
		struct Node* next;
	};
	/* hash table implementation */
	struct Node* symbol_table[TABSIZE];
	/* hash function implementation, dj2b hash function */
	unsigned long djb2_hash(char *str) 
	{
    	unsigned long hash = 5381;
    	int c;
    	while ((c = *str++)) 
    	{
        	hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    	}
    	return hash;
	}
	int hash_function(char *key) 
	{
    	return djb2_hash(key) % TABSIZE;
	}
	/* for lookup if the symbol or literal is already present */
	struct Node* lookup(char* key)
	{
		int index=hash_function(key);
		struct Node *pointer=symbol_table[index];
		while(pointer!=NULL)
		{
			if(strcmp(key,pointer->token_specifier)==0)
			{
				return pointer;
			}
			pointer=pointer->next;
		}
		return NULL;
	}
	void print_hash_table()
	{
		FILE* f=fopen("HashTable.txt","w");
		fprintf(f,"Each index has list of Nodes, Each Node has token_specifier and token_code, both are printing below for all Nodes\n");
		fprintf(f,"I have used dj2b hash function, to find the index of each identifier and constants\n");
		fprintf(f,"%-15s\n","Index");
		struct Node* pointer;
		for(int i=0;i<TABSIZE;i++)
		{
			if(symbol_table[i]!=NULL)
			{
				fprintf(f,"%-15d",i);
				pointer=symbol_table[i];
				while(pointer!=NULL)
				{	
					fprintf(f,"-->%*s %5d",10,pointer->token_specifier,pointer->token_code);
					pointer=pointer->next;
				}
				fprintf(f,"\n");
			}
		}
		fclose(f);
		return;
	}
	/* auxiliary functions facilitating the installation of identifiers and constants */
	int install_id(char* token_specifier);
	void install_num(char* token_specifier);
%}

/*regular expressions*/
delim		[ \t\n]
LETTER		[A-Za-z]
DIGIT		[0-9]
id			{LETTER}({LETTER}|{DIGIT})*
int			{DIGIT}+

/*rules*/
%%
\n			{ /*to print in the listing file the line number*/ return 24;}
[ \t]		{ /* no need to return anything here */	              }	
PROGRAM		{  return 1;										  }				
VAR			{  return 2;									      }
BEGIN		{  return 3;										  }
END			{  return 4;										  }
END\.		{  return 5;										  }
INTEGER		{  return 6;		   								  }
FOR			{  return 7;										  }
READ		{  return 8;									      }
WRITE		{  return 9;									      }
TO			{  return 10;										  }
DO			{  return 11;		 								  }
\;			{  return 12;		 								  }
\:			{  return 13;										  }
\,			{  return 14;										  }
\:\=		{  return 15;										  }
\+			{  return 16;										  }
\-			{  return 17;										  }
\*			{  return 18;										  }
DIV			{  return 19;										  }
\(			{  return 20;										  }
\)			{  return 21;										  }
{id}		{ 
				if(!isVarEncountered && isSecondTokenEncountered)
				{
					return 26;//as no variable should be used before declaration other than program name
				}
				char* specifier = malloc(strlen(yytext) + 2); // Allocate memory for the specifier
                if (specifier == NULL) {
                    printf("Memory allocation failed.\n");
                    exit(EXIT_FAILURE);
                }
                strcpy(specifier + 1, yytext); // Copy yytext into specifier starting from index 1
                specifier[0] = '^'; 
                yyval=install_id(specifier);
                free(specifier); 
				if(yyval==1)
				{
					return 26;
				}
                return 22;
			}
{int}		{
				char* specifier = malloc(strlen(yytext) + 2); // Allocate memory for the specifier
                if (specifier == NULL) {
                    printf("Memory allocation failed.\n");
                    exit(EXIT_FAILURE);
                }
                strcpy(specifier + 1, yytext); // Copy yytext into specifier starting from index 1
                specifier[0] = '#'; 
                install_num(specifier);
                free(specifier);
                return 23;
            }
.			{  return 25;											}
%%

int main()
{
	//to initialise the hash table
	for(int i=0;i<TABSIZE;i++)
	{
		symbol_table[i]=NULL;
	}
	FILE* fout = fopen("listing.txt","w");
	int token_code;
	fprintf(fout,"Listing line by line\n");
	fprintf(fout,"%-15s %-15s %-15s\n","  Line-Number","  Token-Code","Type-Specifier");
	fprintf(fout,"--------------- --------------- ---------------\n");
	int line_num=1;
	bool line_changed=true;
	first=false;
	//to print the listing file correctly, while also printing line number and token specifiers wherever required
	while((token_code=yylex())!=0)
	{
		//to ensure first word is PROGRAM and second word is Program name
		if(first==false)
		{
			first=true;
			if(token_code!=1)
			{
				fprintf(fout,"PROGRAM wasnot encountered as first token\n");
				continue;
			}
		}
		else if(isSecondTokenEncountered==false)
		{
			isSecondTokenEncountered=true;
			if(token_code!=22)
			{
				fprintf(fout,"identifier is not encountered as second token\n");
				continue;
			}
		}
		if(token_code==2)
		{
			isVarEncountered = true;
		}
		if(token_code==3)
		{
			isBeginEncountered = true;
		}
		if(token_code==24)
		{
			line_num++;
			line_changed=true;
		}
		else if(token_code==22)
		{
			if(line_changed)
			{
				line_changed=false;
				fprintf(fout,"%-15d %-15d ^%-s\n",line_num,token_code,yytext);
			}
			else
			{
				fprintf(fout,"%-15s %-15d ^%-s\n"," ",token_code,yytext);
			}
		} 
		else if(token_code==23)
		{
			if(line_changed)
			{
				line_changed=false;
				fprintf(fout,"%-15d %-15d #%-s\n",line_num,token_code,yytext);
			}
			else
			{
				fprintf(fout,"%-15s %-15d #%-s\n"," ",token_code,yytext);
			}
		}
		else if(token_code==25)
		{
			fprintf(fout,"Unknown token encountered %s\n",yytext);
		}
		else if(token_code==26)
		{
			fprintf(fout,"Undeclared identifier encountered %s\n",yytext);
		}
		else
		{
			if(line_changed)
			{
				line_changed=false;
				fprintf(fout,"%-15d %-15d \n",line_num,token_code);
			}
			else
			{
				fprintf(fout,"%-15s %-15d \n"," ",token_code);
			}
		}
	}
	fclose(fout);
	//to print the hash table in a file
	print_hash_table();
	return 0;
}

int install_id(char* token_specifier)
{
    // Allocate memory for the token specifier
    char* specifier = malloc(strlen(token_specifier) + 1);
    if (specifier == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }
    // Copy the token specifier into the allocated memory
    strcpy(specifier, token_specifier);
    struct Node* pointer;
    int hash_val = hash_function(specifier);
    if ((pointer = lookup(specifier)) == NULL) {
		if(isBeginEncountered)
		{
			return 1;
		}
        pointer = (struct Node*)malloc(sizeof(struct Node));
        if (pointer == NULL) {
            printf("Out of memory\n");
            free(specifier);
            return 0;
        }
        //inserting at the start of the index
        pointer->token_specifier = specifier;
        pointer->token_code = 22;
        pointer->next = symbol_table[hash_val];
        symbol_table[hash_val] = pointer;
    } else {
        free(specifier); // Free allocated memory if already exists
    }
	return 0;
}

void install_num(char* token_specifier)
{
    // Allocate memory for the token specifier
    char* specifier = malloc(strlen(token_specifier) + 1);
    if (specifier == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    // Copy the token specifier into the allocated memory
    strcpy(specifier, token_specifier);
    struct Node* pointer;
    int hash_val = hash_function(specifier);
    if ((pointer = lookup(specifier)) == NULL) {
        pointer = (struct Node*)malloc(sizeof(struct Node));
        if (pointer == NULL) {
            printf("Out of memory\n");
            free(specifier);
            return;
        }
        pointer->token_specifier = specifier;
        pointer->token_code = 23;
        pointer->next = symbol_table[hash_val];
        symbol_table[hash_val] = pointer;
    } else {
        free(specifier); // Free allocated memory if already exists
    }
}


		
