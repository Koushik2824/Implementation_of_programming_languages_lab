%{
    #include "pascal_parserGenerator.c"/* to know about the tokens used by yacc program*/
%}
%option yylineno
/*regular expressions*/
delim		[ \t\n]
LETTER		[A-Za-z]
DIGIT		[0-9]
id			({LETTER}({LETTER}|{DIGIT})*)|({DIGIT}+)|({DIGIT}+(\.{DIGIT}+)?(E[+-]?{DIGIT}+)?)
errorint	{DIGIT}+{LETTER}+{LETTER}*({DIGIT}*{LETTER}*)*

/*rules*/
%%
\n			{  /* no need to return anything here */	          }
[ \t]		{ /* no need to return anything here */	              }	
PROGRAM		{  return PROGRAM;								      }				
VAR			{  return VAR;									      }
BEGIN		{  return BEGIN2;									  }
END			{  return END;										  }
END\.		{  return END_DOT;									  }
INTEGER		{  return INTEGER;		   							  }
REAL		{  return REAL;								          }
FOR			{  return FOR;										  }
READ		{  return READ;									      }
WRITE		{  return WRITE;									  }
TO			{  return TO;										  }
DO			{  return DO;		 								  }
\;			{  return SEMI_COLON;		 						  }
\:			{  return COLON;									  }
\,			{  return COMMA;									  }
\:\=		{  return ASSIGNMENT;								  }
\+			{  return PLUS;										  }
\-			{  return MINUS;									  }
\*			{  return MULTIPLY;									  }
DIV			{  return DIV;										  }
\(			{  return OPENBRACKET;								  }
\)			{  return CLOSEBRACKET; 							  }
{id}		{ 
                strcpy(yylval.str, yytext);
                return ID;// all 3 ids,integers,floats return same token ID
			}
{errorint}  {
                printf("Wrong integer/real number is given with alphabets in between\n");
                                                                   }
.			{  printf("Unexpected token encountered\n");
               return CHARACTER;		                           }
%%
// called by lex when input is exhausted
int yywrap(void) 
{
    return 1;
}






