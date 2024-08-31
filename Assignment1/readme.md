Commands to run the file:
1. g++ 210101069_1.cpp
   
Instructions & Information:
1. Input file should be in the same directory as cpp file.
2. Enter the input file name after waiting for some time. Initial Loading of the file till start
    of execution may take some time.
3. Sample input and output files are included in the zip folder, same can be run/assembled.
4. The assembled output program's fields are separated by '^', for better visualizations.
5. Many edge cases are appropriately handled like comments, empty lines, no End statement in the input file.
6. Both pass1, pass2 are independently implemented.
7. In pass1 , if any errors such as undefined opcode or redefined symbol are found, the program stops the execution before the start of pass2.
8. All such errors are appropriately printed.
9. Maps are created to implement hash tables.
10. Error case of C'TEST STRING' is also handled, by checking for opcode BYTE
11. Even if no end is there END record is added
12. If name of program is more than 6 characters length, only first 6 are printed.
13. In pass1, all symbols and there addresses are stored in symbol table.
14. In pass2, all symbol references are resolved and object code and object program are generated.
15. When End is detected the program stops execution.
16. Comments should always start with '.', rest are not considered comments.
17. When RESW,RESB are present, text record breaks, there, to show memory being reserved.
18. End record shows the memory address of first instruction
