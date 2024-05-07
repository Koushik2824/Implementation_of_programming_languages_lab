### Contents of the folder

1.`pascal_lexicalAnalyser.l`
2.`pascal_lexicalAnalyser.c`
3.`listing.txt`
4.`HashTable.txt`
5.`input.pas`

### General overview of the problem and explanation of lex:
1. In the given problem, we are asked to generate lexical analyser for PASCAL using lex.LEX is a software which generates lexical analyser.  
2. Any lexical analyser generally used to recognise keywords,operators,identifiers,integers,floating-point numbers etc. So,the lexical analyser reads all statements line by line of the source code, to find all tokens(Here tokens imply keywords,operators,identifiers etc).  
3. So,the lexical analyser must first have rules and regular expressions to correctly identify the tokens.Regular expressions are written for identifiers, constants,digits,letter,delimiters.The token codes are given in the question. So,the rules are specified which return token codes to the calling function.(Here calling function is yylex())  
4. Hash table is created to store all identifiers and constanst and their corresponding token codes.djb2 hash function is used to get the index.100 is taken as the size of hash table.  
5. `install_id()` and `install_num()` are auxiliary functions implemented to insert into the hash table (identifiers or constants), if they are not already present.Identifiers and constants are distinguished by use of `^` before identifiers and `#` before constants.  
6. Final output is printed in two files `listing.txt` and `HashTable.txt`.`listing.txt` contains each line and corresponding token codes generated.If the token code corresponds to identifier or constant, there token specifiers are also printed.`HashTable.txt` shows the table,indices and their contents.  
7. Here only Integers are allowed as constants as mentioned in the BNF grammar. So,Only integers are considered for constants.Any other invalid characters\tokens not following the given BNF grammar will result in wrong output being produced.  
8. I have used flex to generate the lexical analyser, so the flex must be installed.  
9. Empty lines are skipped. In Pascal all variables used must be declared before Begin, else error message would be printed in `listing.txt`.

### To run the given program(This program runs in linux environment)

0.`sudo apt install flex`
1.`flex -o pascal_lexicalAnalyser.c pascal_lexicalAnalyser.l`
2.`gcc -o pascal_lexicalAnalyser -lfl pascal_lexicalAnalyser.c` (This links the flex library)
3.`./pascal_lexicalAnalyser < input.pas`