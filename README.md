# README

This repository contains solutions to four programming assignments for the CS-348 course, each focusing on different aspects of programming languages and tools. Below is an overview of each assignment along with the tools and concepts utilized, followed by instructions on running and testing the solutions.

## Programming Assignment #1

**Objective**: Implement a 2-pass assembler for SIC machine assembly language.

**Tools & Concepts**:
- Language: C
- Assembler Logic: Two-pass algorithm based on pseudo code provided in the textbook.
- Instruction Set: LDA, LDX, LDL, STA, STX, STL, LDCH, STCH, ADD, SUB, MUL, DIV, COMP, J, JLT, JEQ, JGT, JSUB, RSUB, TIX, TD, RD, WD, RESW, RESB, WORD, BYTE.

## Programming Assignment #2

**Objective**: Modify the assembler from Assignment #1 for SIC/XE machine, and implement a 2-pass linking loader.

**Tools & Concepts**:
- Language: C
- Assembler Modification: Support for SIC/XE machine, generate Revised Modification records, Define records, and Refer records.
- Linking Loader: Implement a 2-pass linking loader based on provided pseudo code.
- Instruction Set: Extended to handle SIC/XE machine instructions like LDB, LDT, COMPR, CLEAR, TIXR, etc.

## Programming Assignment #3

**Objective**: Construct a lexical analyzer for a simplified PASCAL language.

**Tools & Concepts**:
- Language: LEX
- Tokenization: Define tokens based on the provided BNF grammar and token coding scheme.
- Symbol Table: Implement hashed symbol table organization with auxiliary functions install-id() and install-num().

## Programming Assignment #4

**Objective**: Generate a parser for a simplified PASCAL language.

**Tools & Concepts**:
- Language: YACC (Bison) & LEX
- Parser Generation: Use YACC to generate a parser based on the augmented BNF grammar.
- Syntax & Semantic Error Handling: Augment the grammar to handle syntax and semantic errors.
- Tokenization: Utilize LEX for tokenization based on the provided token coding scheme.

## Running & Testing Instructions

1. Clone the repository:
`git clone [<repository_url>](https://github.com/Koushik2824/Implementation_of_programming_languages_lab.git)`


2. Verify the output against expected results provided in the assignment description or sample outputs.

## Additional Notes

- Ensure that necessary dependencies (e.g., GCC for C programs, YACC/Bison, LEX/Flex) are installed.
- Review the source code, comments, and documentation for a deeper understanding of the implementation details.
