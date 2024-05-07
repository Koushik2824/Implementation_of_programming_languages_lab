%{
    void yyerror (const char *s);
    int yylex();
    int yyval;
    #include<stdbool.h>
    extern int yylineno;
    #define num_integer 0
    #define num_float 1
    #define num_undefined 2
    #define TABSIZE 100
    struct Node
	{
		char* token_specifier;
        int var_type;
        bool is_initialized;
        bool is_id;
		struct Node* next;
	};
    struct Node* symbol_table[TABSIZE];
    /* hash function implementation, dj2b hash function */
	unsigned long djb2_hash(char *str) 
	{
    	unsigned long hash = 5876;
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
    int insert(char* token_specifier,int sym_type)
    {
        // Allocate memory for the token specifier
        char* specifier = (char*)malloc(strlen(token_specifier));
        if (specifier == NULL) {
            printf("Memory allocation failed.\n");
            return 0;
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
                return 0;
            }
        //inserting at the start of the index
        pointer->token_specifier = specifier;
        pointer->var_type=sym_type;
        pointer->is_initialized=0;
        pointer->is_id=1;
        pointer->next = symbol_table[hash_val];
        symbol_table[hash_val] = pointer;
        } else {
            free(specifier); // Free allocated memory if already exists
            return 0;//as it was already inserted into table before
        }
	    return 1;
    }
    int insert_other(char* token_specifier,bool isid,int var_type)
    {
        // Allocate memory for the token specifier
        char* specifier = (char*)malloc(strlen(token_specifier));
        if (specifier == NULL) {
            printf("Memory allocation failed.\n");
            return 0;
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
                return 0;
            }
        //inserting at the start of the index
        pointer->token_specifier = specifier;
        pointer->var_type=var_type;
        if(isid==0)
        pointer->is_initialized=1;
        else
        pointer->is_initialized=0;
        pointer->is_id=isid;
        pointer->next = symbol_table[hash_val];
        symbol_table[hash_val] = pointer;
        } else {
            free(specifier); // Free allocated memory if already exists
            return 0;//as it was already inserted into table before
        }
	    return 1;
    }
    //to get the type of identifier
    int get_type(char* key)
    {
        int index = hash_function(key);
        struct Node *pointer=symbol_table[index];
		while(pointer!=NULL)
		{
			if(strcmp(key,pointer->token_specifier)==0)
			{
				return pointer->var_type;
			}
			pointer=pointer->next;
		}
        return 3;
    }
    void print_hash_table()
	{
		FILE* f=fopen("HashTable.txt","w");
		fprintf(f,"Each index has list of Nodes, Each Node has token_specifier \n");
		fprintf(f,"I have used dj2b hash function, to find the index of each identifier and constants(0 for integer and 1 for real, 2 for undefined)\n");
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
					fprintf(f,"-->[%-10s:type : %-4d:is_identifier : %-4d:is_initialized:%-4d]",pointer->token_specifier,pointer->var_type,pointer->is_id,pointer->is_initialized);
					pointer=pointer->next;

				}
				fprintf(f,"\n");
			}
		}
		fclose(f);
		return;
	}
    int ERROR_FLAG = 0;
    int install_id(char* key);
    //for all declarations involved in pascal
    int declare_type(const char *str, int var_type) {
        char *token_specifier;
        char *str_copy = strdup(str);
        if (str_copy == NULL) {
            printf("Memory allocation failed.\n");
            return 0;
        }
        token_specifier = strtok(str_copy, " ,");
        int res=1;
        while (token_specifier != NULL) {
            if (insert(token_specifier, var_type) == 0) {
                printf("The symbol which was redeclared is %s\n",token_specifier);
                res=-1;
            } else {
            }
        token_specifier = strtok(NULL, " ,");
        }
        free(str_copy);
        return res;
    }
    // to check if the ids have their typr already declared
    int verify_ids(const char *str) {
        char *token_specifier;
        char *str_copy = strdup(str);
        if (str_copy == NULL) {
            printf("Memory allocation failed.\n");
            return 0;
        }
        token_specifier = strtok(str_copy, ",");
        while (token_specifier != NULL) {
            if (get_type(token_specifier) == num_undefined) {
                printf("Variable %s type is undeclared.\n", token_specifier);
                free(str_copy);
                return -1;
            }
            else if(get_type(token_specifier)==3)
            {
                printf("var is undeclared:%s\n",token_specifier);
                return -1;
            }
            token_specifier = strtok(NULL, ",");
        }
        free(str_copy);
        return 1;
    }
    // to set type during variable/identifier declaration
    void set_type(char* str)
    {
        int index = hash_function(str);
        struct Node *pointer=symbol_table[index];
		while(pointer!=NULL)
		{
			if(strcmp(str,pointer->token_specifier)==0)
			{
				pointer->is_initialized=1;
                return;
			}
			pointer=pointer->next;
		}
        return;
    }
    //to check for identifier initialisation
    int is_initialized(char* str)
    {
        int index = hash_function(str);
        struct Node *pointer=symbol_table[index];
		while(pointer!=NULL)
		{
			if(strcmp(str,pointer->token_specifier)==0)
			{
				if(pointer->is_initialized) return 1;
                return -1;
			}
			pointer=pointer->next;
		}
        return -1;
    }
    // for read all vars will become initialised
    void read_initilialisations(char* str)
    {
        char *token_specifier;
        char *str_copy = strdup(str);
        if (str_copy == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
        token_specifier = strtok(str_copy, ",");
        while (token_specifier != NULL) {
            set_type(token_specifier);
            token_specifier = strtok(NULL, ",");
        }
        free(str_copy);
        return;
    }
    // to check if the string is integer
    bool is_integer(const char *str) 
    {
        char *endptr;
        strtol(str, &endptr, 10); 
        return *endptr == '\0';
    }
    // to check if the string is float
    bool is_float(const char *str) 
    {
        char *endptr;
        strtof(str, &endptr); 
        return *endptr == '\0';
    }
    // to extract integer from string
    int extract_integer(const char *str) 
    {
        char *endptr;
        float value = strtof(str, &endptr);
        if (*endptr == '\0') {
            return (int)value; 
        } else {
            return -1; 
        }
    }
    //to extract float from string
    float extract_float(const char *str) 
    {
        char *endptr;
        return strtof(str, &endptr); 
    }
%}

%start prog
%define parse.error verbose
%union {
    char str[1024];
    int type;
}


%token PROGRAM
%token VAR
%token BEGIN2
%token END
%token END_DOT
%token INTEGER
%token REAL
%token FOR
%token READ
%token WRITE
%token TO
%token DO
%token SEMI_COLON
%token COLON
%token COMMA
%token ASSIGNMENT
%token PLUS
%token MINUS
%token MULTIPLY
%token DIV
%token OPENBRACKET
%token CLOSEBRACKET
%token CHARACTER
%token <str> ID
%type <str> idlist
%type <type> type
%type <type> exp
%type <type> factor
%type <type> term


%% 
prog : PROGRAM progname VAR declist BEGIN2 stmtlist END_DOT 
    ;

progname : ID   {
                    if(is_float($1))
                    {
                        insert_other($1,0,1);
                        printf("Wrong (int|float) was given as program name\n");
                        ERROR_FLAG=1;
                    }
                    else if(is_integer($1))
                    {
                        insert_other($1,0,0);
                        printf("Wrong (int|float) was given as program name\n");
                        ERROR_FLAG=1;
                    }
                    else
                    {
                        insert_other($1,1,2);
                    }
                }
    ;

declist : dec
    | declist SEMI_COLON dec
    | error
    ;

dec : idlist COLON type {
                            // insert the declarations with their types into the hash table to check for reoccurances
                            if(declare_type($1, $3) < 0)
                            {
                                printf ("In Line %d,a symbol is redeclared\n",yylineno);
                                ERROR_FLAG = 1;
                            }
                        }
    ;

type : INTEGER          {
                            $$ = num_integer;// to set the type to integer of the identifier
                        }

    | REAL              {
                            $$ = num_float;// to set the type to float for identifer
                        }
    ;

idlist : ID             {
                            if(is_float($1))
                            {
                                insert_other($1,0,1);
                                printf("Wrong (int|float) was given as identifier\n");
                                ERROR_FLAG=1;
                            }
                            else if(is_integer($1))
                            {
                                insert_other($1,0,0);
                                printf("Wrong (int|float) was given as identifier\n");
                                ERROR_FLAG=1;
                            }
                            else
                            {
                                strcpy($$, $1);//directly copy the ID into idlist
                            }
                        }

    | idlist COMMA ID   {
                            if(is_float($3))
                            {
                                insert_other($3,0,1);
                                printf("Wrong (int|float) was given as identifier\n");
                                ERROR_FLAG=1;
                            }
                            else if(is_integer($3))
                            {
                                insert_other($3,0,0);
                                printf("Wrong (int|float) was given as identifier\n");
                                ERROR_FLAG=1;
                            }
                            else
                            {
                                strcpy($$, $1); 
                                strcat($$, ","); //else add with a comma
                                strcat($$, $3);
                            }
                        }
    ;

stmtlist : stmt 
    | stmtlist SEMI_COLON stmt
    | error
    ;

stmt : assign 
    | read
    | write
    | for
    ;

assign : ID ASSIGNMENT exp  {
                                if(is_float($1))
                                {
                                    insert_other($1,0,1);
                                    printf("Wrong (int|float) was given as identifier as left of assignment\n");
                                    ERROR_FLAG=1;
                                }
                                else if(is_integer($1))
                                {
                                    insert_other($1,0,0);
                                    printf("Wrong (int|float) was given as identifier, as left of assignment\n");
                                    ERROR_FLAG=1;
                                }
                                else
                                {
                                    insert_other($1,1,2);
                                    // if LHS not declared
                                    if(get_type($1) == num_undefined)
                                    {   
                                        printf ("In Line %d,a symbol is undeclared\n",yylineno);
                                        ERROR_FLAG = 1;
                                    }
                                    // if mismatch in LHS and RHS type
                                    else if(get_type($1) != $3 && $3 != num_undefined)
                                    {
                                        printf ("In Line %d,types of the expression assignment dont match\n",yylineno);
                                        ERROR_FLAG = 1;
                                    }
                                    else if($3==num_integer)
                                    {   
                                        set_type($1);
                                    }
                                    else if($3==num_float)
                                    {
                                        set_type($1);
                                    }
                                }
                            }
    ;

exp : term              {
                            $$ = $1;
                        }
    | exp PLUS term      {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != num_undefined && $1 != num_undefined)
                                    printf ("In Line %d,types of the expression assignment dont match\n",yylineno);
                                $$ = num_undefined;
                                ERROR_FLAG = 1;
                            }
                            else 
                                $$ = $1;
                        }
    | exp MINUS term      {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != num_undefined && $1 != num_undefined)
                                    printf ("In Line %d,types of the expression assignment dont match\n",yylineno);
                                $$ = num_undefined;
                                ERROR_FLAG = 1;
                            }
                            else 
                                $$ = $1;
                        }
    ;

term : factor           {
                            $$ = $1;
                        }
    | term MULTIPLY factor   {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != num_undefined && $1 != num_undefined)
                                    printf ("In Line %d,types of the expression assignment dont match\n",yylineno);
                                $$ = num_undefined;
                                ERROR_FLAG = 1;
                            }
                            else 
                                $$ = $1;
                        }
    | term DIV factor   {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != num_undefined && $1 != num_undefined)
                                    printf ("In Line %d,types of the expression assignment dont match\n",yylineno);
                                $$ = num_undefined;
                                ERROR_FLAG = 1;
                            }
                            else 
                                $$ = $1;
                        }
    ;

factor : ID                         {

                                        if(is_integer($1))
                                        {
                                            insert_other($1,0,0);
                                            $$=num_integer;
                                        }
                                        else if(is_float($1))
                                        {
                                            insert_other($1,0,1);
                                            $$=num_float;
                                        }
                                        else
                                        {
                                            insert_other($1,1,2);
                                            $$ = get_type($1);
                                            // if the symbol is not defined
                                            if($$ == num_undefined)
                                            {
                                                printf ("In Line %d,a symbol is undeclared\n",yylineno);
                                                ERROR_FLAG = 1;
                                            }
                                            else if(is_initialized($1)<0)
                                            {
                                                printf ("In Line %d,a symbol (%s) is uninitialised\n",yylineno,$1);
                                                ERROR_FLAG = 1;
                                            }
                                        }
                                    }
    | OPENBRACKET exp CLOSEBRACKET  {
                                        $$ = $2;
                                    }
    ;

read : READ OPENBRACKET idlist CLOSEBRACKET     {
                                                    // check if all ids are valid
                                                    if(verify_ids($3) < 0)
                                                    {
                                                        printf ("In Line %d,a symbol is undeclared\n",yylineno);
                                                        ERROR_FLAG = 1;
                                                    }
                                                    else
                                                    {
                                                        read_initilialisations($3);//to set the initialisations as they are read
                                                    }
                                                }
    ;

write : WRITE OPENBRACKET idlist CLOSEBRACKET   {
                                                    // check if all ids are valid
                                                    if(verify_ids($3) <  0)
                                                    {
                                                        printf ("In Line %d,a symbol is undeclared\n",yylineno);
                                                        ERROR_FLAG = 1;
                                                    }
                                                }
    ;

for : FOR indexexp DO body
    | error
    ;

indexexp : ID ASSIGNMENT exp TO exp {
                                        if(is_float($1))
                                        {
                                            insert_other($1,0,1);
                                            printf("Wrong (int|float) was given as identifier\n");
                                            ERROR_FLAG=1;
                                        }
                                        else if(is_integer($1))
                                        {   
                                            insert_other($1,0,0);
                                            printf("Wrong (int|float) was given as identifier\n");
                                            ERROR_FLAG=1;
                                        }
                                        else
                                        {
                                            insert_other($1,1,2);
                                            // check if id not declared
                                            if(get_type($1) == num_undefined)
                                            {
                                                printf ("In Line %d,a symbol is undeclared (this symbol is loop iterator)\n",yylineno);
                                                ERROR_FLAG = 1;
                                            }
                                            // check for type mismatch
                                            else if(get_type($1) != $3 || get_type($1) != $5)
                                            {
                                                printf ("In Line %d,The iterator type doesnot match with types of expressions.\n",yylineno);
                                                ERROR_FLAG = 1;
                                            }
                                            else if($3!=$5)
                                            {
                                                printf ("In Line %d,Expression types dont match.\n",yylineno);
                                                ERROR_FLAG = 1;
                                            }
                                            else if($3==num_undefined||$5==num_undefined)
                                            {
                                                printf("In Line %d,Expression type is undeclared\n",yylineno);
                                                ERROR_FLAG = 1;
                                            }
                                            else
                                            {
                                                read_initilialisations($1);
                                            }
                                        }
                                    }
    ;

body : stmt 
    | BEGIN2 stmtlist END
    ;
%% 

// called when a syntax error is encountered
void yyerror (const char *s) 
{
    fprintf (stderr, "Line %d : %s\n", yylineno, s);
    ERROR_FLAG = 1;
} 

int main (void) 
{
    for(int i=0;i<TABSIZE;i++) symbol_table[i]=NULL;//to set the table
	yyparse ();
    // check if no error found
    if(ERROR_FLAG == 0)
    {
        printf("Program compilation was successfull without error.\n");
        //return 0;
    }
    // print all the symbols
    print_hash_table();
    return 0;
}