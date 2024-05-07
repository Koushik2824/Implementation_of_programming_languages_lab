#include <bits/stdc++.h>
// to give colors to errors which occur when there are redefined symbols,or,undefined symbols or undefined opcode
#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
using namespace std;
// assembler class which implements pass1,pass2,initialises the file name,start address,LOCCTR
class Assembler
{
public:
    // variables to store required values of startAddress,Program Size etc
    int LOCCTR, startAddress, objectProgramSize;
    int numberOfInvalidOpcodeErrors, numberOfUndefinedSymbolErrors, numberofReDefinedSymbolErrors;
    char inputFile[15];
    char nameOfProgram[10];
    char bufferArray[65536];
    bool moreWords;
    // opcode table which is intialized before pass1
    map<string, string> optab;
    // symbol table which is filled during pass1
    map<string, int> symtab;
    // File pointers to read and write into various files
    FILE *fr;
    FILE *fw;
    FILE *fobject;
    // function to fill opcode table with required values
    void setOptab()
    {
        optab["LDA"] = "00";
        optab["LDX"] = "04";
        optab["LDL"] = "08";
        optab["STA"] = "0C";
        optab["STX"] = "10";
        optab["STL"] = "14";
        optab["LDCH"] = "50";
        optab["STCH"] = "54";
        optab["ADD"] = "18";
        optab["SUB"] = "1C";
        optab["MUL"] = "20";
        optab["DIV"] = "24";
        optab["COMP"] = "28";
        optab["J"] = "3C";
        optab["JLT"] = "38";
        optab["JEQ"] = "30";
        optab["JGT"] = "34";
        optab["JSUB"] = "48";
        optab["RSUB"] = "4C";
        optab["TIX"] = "2C";
        optab["TD"] = "E0";
        optab["RD"] = "D8";
        optab["WD"] = "DC";
    }
    // assembler constructor
    Assembler()
    {
        numberOfInvalidOpcodeErrors = 0;
        numberofReDefinedSymbolErrors = 0;
        numberOfInvalidOpcodeErrors = 0;
        setOptab();
    }
    // to give the input file name which should be assembled
    bool setFile(char fileName[])
    {
        strcpy(inputFile, fileName);
        if (!(fr = fopen(fileName, "r")))
        {
            cout << "Error Opening file" << endl;
            return false;
        }
        // to print the file in terminal
        cout << "--------------------------------InputFile.txt---------------------------------\n";
        char line[60];
        while (fgets(line, 60, fr))
        {
            cout << line;
        }
        cout << "\nFile Ends above" << endl;
        fclose(fr);
        moreWords=false;
        return true;
    }
    // destructor clears symbol table,as it's only related to the input given
    ~Assembler()
    {
        symtab.clear();
    }
    // pass1
    bool pass1();
    // pass2
    bool pass2();
    // to check whether the given line in input file is comment or not
    bool notacomment(char *line)
    {
        // those lines which start with '.' are comments
        for (int i = 0; i < strlen(line); i++)
        {
            if (line[i] != ' ')
            {
                if (line[i] == '.')
                    return false;
                return true;
            }
        }
        return true;
    }
    // to parse in pass1
    int parsePass1(char *line,char* templine, char *label, char *op, char *operand,FILE* fr,FILE* fw)
    {
        char* temp1;
        char* temp2;
        char* temp3;
        char* temp4;
        temp1 = strtok(line, " \t\n");
        while (temp1 == nullptr)
        {
            if (fgets(line, 50, fr) == NULL)
            {
                return 1;
            }
            while (notacomment(line) == 0)
            {
                fprintf(fw, "%s", line);
                if (fgets(line, 50, fr) == NULL)
                {
                    return 1;
                }
            }
            removeTrailingSpaces(line);
            strcpy(templine, line);
            temp1 = strtok(line, " \t\n");
        }
        temp2 = strtok(nullptr, " \t\n");
        if (temp2 == nullptr)
        {
            strcpy(op, temp1);
        }
        else
        {
            temp3 = strtok(nullptr, " \t\n");
            if (temp3 == nullptr)
            {
                strcpy(op, temp1);
                strcpy(operand, temp2);
            }
            else
            {
                strcpy(label, temp1);
                strcpy(op, temp2);
                strcpy(operand, temp3);
                if((temp4=strtok(nullptr,"\n"))!=nullptr &&(strcmp(label,"BYTE")!=0&&strcmp(op,"BYTE")!=0))
                {
                    moreWords=true;
                }
            }
        }
        return 0;
    }
    // to parse in pass2
    int parsePass2(char *line,char* memaddress, char *label, char *op, char *operand)
    {
        char*temp1;
        char*temp2;
        char*temp3;
        char*temp4;
        temp1 = strtok(line, " \t\n");    // string parsing and storing the results in appropraite character arrays
        temp2 = strtok(nullptr, " \t\n"); // empty lines
        if (temp2 == nullptr)
        {
            strcpy(label, temp1);
        }
        else
        {
            temp3 = strtok(nullptr, " \t\n");
            if (temp3 == nullptr)
            {
                if (strcmp(temp1, "END") != 0) // if only 2 are present and tempstr1 is not END
                {
                    strcpy(memaddress, temp1);
                    strcpy(op, temp2);
                }
                else
                {
                    strcpy(op, temp1);
                    strcpy(operand, temp2);
                }
            }
            else
            {
                temp4 = strtok(nullptr, " \t\n");
                if (temp4 == nullptr) // here only memaddress,opcode,operand will be present
                {
                    strcpy(memaddress, temp1);
                    strcpy(op, temp2);
                    strcpy(operand, temp3);
                }
                else
                {
                    strcpy(memaddress, temp1);
                    strcpy(label, temp2);
                    strcpy(op, temp3);
                    strcpy(operand, temp4);
                }
            }
        }
        return 0;
    }
    // to remove the trailing empty spaces
    void removeTrailingSpaces(char *str)
    {
        // Find the last non-space character in the string
        int length = strlen(str);
        int lastNonSpaceIndex = length - 1;
        if (str[lastNonSpaceIndex] == '\n')
            lastNonSpaceIndex--;
        while (lastNonSpaceIndex >= 0 && str[lastNonSpaceIndex] == ' ')
        {
            lastNonSpaceIndex--;
        }

        // Null-terminate the string at the last non-space character
        str[lastNonSpaceIndex + 1] = '\0';
    }
    // if byte is opcode then we must do the below
    void byteForPass1(char *line, char *label, char *op, char *operand)
    {
        char *temp1;
        char *temp2;
        char *temp3;
        bool bytePresent=false;
        int length;
        if (strcmp(label, "BYTE") == 0)
        {
            length=sizeof(label);
            memset(label, 0,length);
            temp1 = strtok(line, " \t\n");
            temp2 = strtok(nullptr, "\n");
            strcpy(op, temp1);
            strcpy(operand, temp2);
            bytePresent=true;
        }
        else if (strcmp(op, "BYTE") == 0)
        {
            if (strlen(label) != 0)
            {
                temp1 = strtok(line, " \t\n");
                temp2 = strtok(nullptr, " \t\n");
                temp3 = strtok(nullptr, "\n");
                strcpy(operand, temp3);
                bytePresent=true;
            }
        }
        if(bytePresent)
        {
            if(strlen(operand)>=2&&(operand[0]=='C'||operand[0]=='X')&&operand[1]=='\'')
            {
                for(int i=2;i<strlen(operand);i++)
                {
                    if(operand[i]=='\''&&operand[i-1]!='\\')
                    {
                        if(i!=strlen(operand)-1)
                        {
                            moreWords=true;
                            break;
                        }
                    }
                }
            }
        }
    }
    void byteForPass2(char *line, char *memaddress, char *label, char *op, char *operand)
    {
        char *temp1;
        char *temp2;
        char *temp3;
        char *temp4;
        int length;
        if (strcmp(label, "BYTE") == 0)
        {
            length=sizeof(label);
            memset(label, 0,length);
            temp4 = strtok(line, " \t\n");
            temp1 = strtok(nullptr, " \t\n");
            temp2 = strtok(nullptr, "\n");
            strcpy(op, temp1);
            strcpy(operand, temp2);
        }
        else if (strcmp(op, "BYTE") == 0)
        {
            if (strlen(label) != 0)
            {
                temp4 = strtok(line, " \t\n");
                temp1 = strtok(nullptr, " \t\n");
                temp2 = strtok(nullptr, " \t\n");
                temp3 = strtok(nullptr, "\n");
                strcpy(operand, temp3);
            }
        }
    }
};
// pass1 implementation which returns true,when the pass1 is succesful
// in pass1 Address of each of the instruction is printed in intermediarte.txt file and
// symbol table is also filled and checked for any errors like redefined labels
// or undefined opcode errors and appropriate actions are taken
bool Assembler::pass1()
{
    fw = fopen("intermediate.txt", "w");
    fr = fopen(inputFile, "r");
    bool correctFile = true;
    bool cond = true;                    // to check if there is any error while reading the file
    char label[15], op[10], operand[30]; // to store each of the label,opcode,operand after parsing
    char line[50];                       // to store each line separately
    char templine[50];
    // to avoid edge cases,three temporary strings are created
    char *tempstr1;
    char *tempstr2;
    char *tempstr3;
    // line read from input file
    if (fgets(line, 50, fr) == NULL)
    {
        return 1;
    }
    // check if the line is comment or not
    while (notacomment(line) == 0)
    {
        fprintf(fw, "%s", line);
        if (fgets(line, 50, fr) == NULL)
        {
            return 1;
        }
    }
    removeTrailingSpaces(line);
    strcpy(templine, line);
    // next line taken and parsing is done
    if(parsePass1(line,templine,label,op,operand,fr,fw)) return 1;
    byteForPass1(templine, label, op, operand);
    // cout<<"Reached here"<<endl;
    // string parsing completed
    if (strcmp(op, "START") == 0)
    {
        strcpy(nameOfProgram, label);
        startAddress = strtoul(operand, nullptr, 16); // hex string to decimal number converter
        LOCCTR = startAddress; // initialise LOCCTR
        fprintf(fw, "%04X\t%s\t%s\t%s\n", LOCCTR, label, op, operand); // printing to intermediate.txt file
        if (fgets(line, 50, fr) == NULL)
        {
            return 1;
        }
        while (notacomment(line) == 0)
        {
            fprintf(fw, "%s", line);
            if (fgets(line, 50, fr) == NULL)
            {
                return 1;
            }
        }
        removeTrailingSpaces(line);
        strcpy(templine, line);
        // next line taken and parsing is done
        memset(label, 0, sizeof(label));
        if (parsePass1(line, templine, label, op, operand, fr, fw)) return 1;
        byteForPass1(templine, label, op, operand);
    }
    else
    {
        cout << "No start detected,so proceeding without file name,with start address as 0" << endl;
        startAddress = 0;
        LOCCTR = 0;
        strcpy(nameOfProgram, "untitled");
    }
    int templocctr; // to store the Previous iterations LOCCTR value
    // continuing the loop till END line is seen
    while (strcmp(op, "END") != 0)
    {
        templocctr = LOCCTR;
        if (strlen(label) != 0) // that is there is symbol in label field
        {
            if (symtab.find(label) != symtab.end()) // if label is already present then its redefined
            {
                // set the error flag to true
                numberofReDefinedSymbolErrors++;
                cout << RED_COLOR << "Error:" << RESET_COLOR << label << " is redefined" << endl;
                correctFile = false;
            }
            else
            {
                symtab[label] = LOCCTR;
            }
        }
        // adding to LOCCTR to get address of next instruction
        if (optab.find(op) != optab.end())
        {
            LOCCTR += 3;
        }
        else if (strcmp(op, "WORD") == 0)
        {
            LOCCTR += 3;
        }
        else if (strcmp(op, "RESW") == 0)
        {
            LOCCTR += (3 * atoi(operand));
        }
        else if (strcmp(op, "RESB") == 0)
        {
            LOCCTR += (atoi(operand));
        }
        else if (strcmp(op, "BYTE") == 0)
        {
            if (operand[0] == 'X')
            {
                if(strlen(operand)<=3)
                {
                    cout << RED_COLOR << "Error-" << RESET_COLOR << operand << "- format is incorrect" << endl;
                    correctFile = false;
                }
                else if(moreWords==false&&operand[strlen(operand)-1]!='\'')
                {
                    cout << RED_COLOR << "Error-" << RESET_COLOR << operand << "- format is incorrect" << endl;
                    correctFile = false;
                }
                else
                {
                    LOCCTR += (strlen(operand) - 3) / 2;
                }
            }
            else if (operand[0] == 'C')
            {
                if (strlen(operand) <= 3)
                {
                    cout << RED_COLOR << "Error-" << RESET_COLOR << operand << "- format is incorrect" << endl;
                    correctFile = false;
                }
                else if (moreWords == false && operand[strlen(operand) - 1] != '\'')
                {
                    cout << RED_COLOR << "Error-" << RESET_COLOR << operand << "- format is incorrect" << endl;
                    correctFile = false;
                }
                else
                LOCCTR += (strlen(operand) - 3);
            }
            else
            {
                LOCCTR += (strlen(operand));
            }
        }
        else
        {
            numberOfInvalidOpcodeErrors++;
            cout << RED_COLOR << "Error:" << RESET_COLOR << op << " is undefined" << endl;
            correctFile = false;
        }
        fprintf(fw, "%04X\t", templocctr); // writing to intermediatefile.txt
        fprintf(fw, "%s\t%s\t%s\n", label, op, operand);
        memset(label, 0, sizeof(label)); // clearing the arrays
        memset(op, 0, sizeof(op));
        memset(operand, 0, sizeof(operand));
        if (fgets(line, 50, fr) == NULL)
        {
            cond = false;
            break;
        }
        while (notacomment(line) == 0)
        {
            fprintf(fw, "%s", line);
            if (fgets(line, 50, fr) == NULL)
            {
                cond = false;
                break;
            }
        }
        if (cond == false)
            break;
        removeTrailingSpaces(line);
        strcpy(templine, line);
        //next line parsing is done
        if (parsePass1(line, templine, label, op, operand, fr, fw)) return 1;
        byteForPass1(templine, label, op, operand);
        // cout<<"label:"<<label<<":op:"<<op<<":operand:"<<operand<<endl;
    }
    if (cond == false)
    {
        cout << "No END detected" << endl;
    }
    else
        fprintf(fw, "\t%s\t%s\t%s\n", label, op, operand);
    if (correctFile == false)
    {
        cout << "Errors found\n";
        return false;
    }
    cout << "--------------------------------IntermediateFile.txt---------------------------------\n";
    fclose(fw);
    fclose(fr);
    fw = fopen("intermediate.txt", "r"); // printing the intermediate file with addresses in terminal
    while (fgets(line, 50, fw))
    {
        cout << line;
    }
    fclose(fw);
    objectProgramSize = LOCCTR - startAddress;
    cout<<"Object Program size is (in bytes)"<<objectProgramSize<<endl;
    return true;
}
bool Assembler::pass2()
{
    fr = fopen("intermediate.txt", "r");
    fw = fopen(nameOfProgram, "w");                // to write the object code
    FILE *fw2 = fopen("assemblylisting.txt", "w"); // to write the assembly listing
    bool cond = true;
    char label[15], op[10], operand[30], memaddress[8], firstaddress[4];
    char line[70];
    char templine[70];
    if (fgets(line, 50, fr) == NULL)
    {
        cond = true;
    }
    while (notacomment(line) == 0)
    {
        fprintf(fw2, "%s", line);
        if (fgets(line, 50, fr) == NULL)
        {
            cond = false;
            break;
        }
    }
    if (cond == false)
    {
        cout << "Error while reading the file" << endl;
        return 0;
    }
    removeTrailingSpaces(line);
    strcpy(templine, line);
    parsePass2(line,memaddress,label,op,operand);//to parse the line
    byteForPass2(templine, memaddress, label, op, operand);
    // cout<<"memaddress:"<<memaddress<<":label:"<<label<<":opcode:"<<op<<":operand:"<<operand<<endl;
    if (strcmp(op, "START") == 0)
    {
        fprintf(fw2, "%s\t%s\t%s\t%s\n", memaddress, label, op, operand);
        if (fgets(line, 70, fr) == NULL)
        {
            cond = false;
        }
        while (notacomment(line) == 0)
        {
            fprintf(fw, "%s", line);
            if (fgets(line, 70, fr) == NULL)
            {
                cond = false;
                break;
            }
        }
        if (cond == true)
        {
            removeTrailingSpaces(line);
            strcpy(templine, line);
            parsePass2(line, memaddress, label, op, operand);
            byteForPass2(templine, memaddress, label, op, operand);
            while ((strcmp(op, "RESW") == 0) || (strcmp(op, "RESB") == 0)) // these dont generate object code so handled accordingly
            {
                fprintf(fw2, "%s\t%s\t%s\t%s\t\n", memaddress, label, op, operand);
                if (fgets(line, 70, fr) == NULL)
                {
                    cond = false;
                    break;
                }
                while (notacomment(line) == 0)
                {
                    fprintf(fw2, "%s", line);
                    if (fgets(line, 70, fr) == NULL)
                    {
                        cond = false;
                        break;
                    }
                }
                if (cond == false)
                    break;
                removeTrailingSpaces(line);
                strcpy(templine, line);
                memset(label, 0, sizeof(label));
                memset(op, 0, sizeof(op));
                memset(operand, 0, sizeof(operand));
                memset(memaddress, 0, sizeof(memaddress));
                parsePass2(line, memaddress, label, op, operand);
                byteForPass2(templine, memaddress, label, op, operand);
            }
        }
        // cout << "memaddress:" << memaddress << ":label:" << label << ":opcode:" << op << ":operand:" << operand << endl;
    }
    // cout << "memaddress:" << memaddress << ":label:" << label << ":opcode:" << op << ":operand:" << operand << endl;

    if (strlen(nameOfProgram) < 6)
    {
        fprintf(fw, "H^%s", nameOfProgram); // writing to output file
        for (int i = 0; i < 6 - strlen(nameOfProgram); i++)
            fprintf(fw, " ");
    }
    else
    {
        fprintf(fw, "H^"); // writing to output file
        for (int i = 0; i < 6; i++)
        {
            fprintf(fw, "%c", nameOfProgram[i]);
        }
    }
    fprintf(fw, "^%06X^%06X\n", startAddress, objectProgramSize);
    stringstream textRecord; // to store text record and printing it in object file only when its size has exceeded 30
    stringstream objectCode;
    int ctr = 0; // to keep the size of text record
    LOCCTR = startAddress;
    int prevloc;                    // current instructions LOCCTR
    int prevaddress = startAddress; // text records first instructions address
    while (strcmp(op, "END") != 0)
    {
        objectCode.str("");      // current instructions object code
        objectCode << uppercase; // to make hex in all capitals
        prevloc = LOCCTR;
        if (notacomment(line))
        {
            fprintf(fw2, "%s\t%s\t%s\t%s\t", memaddress, label, op, operand); // writing to assembly listing file
            // cout << "memaddress:" << memaddress << ":label:" << label << ":opcode:" << op << ":operand:" << operand << endl;
            if (optab.find(op) != optab.end())
            {
                if (strlen(operand) != 0)
                {
                    if (operand[strlen(operand) - 1] == 'X' && operand[strlen(operand) - 2] == ',') // this is for indexed addressing
                    {
                        char actualoperand[strlen(operand) - 2 + 1];
                        for (int i = 0; i < strlen(operand) - 2; i++)
                        {
                            actualoperand[i] = operand[i];
                        }
                        actualoperand[strlen(operand) - 2] = '\0';
                        if (symtab.find(actualoperand) != symtab.end())
                        {
                            fprintf(fw2, "%s%04X\n", optab[op].c_str(), (symtab[actualoperand] | (1 << 15)));
                            objectCode << optab[op] << setw(4) << setfill('0') << hex << (symtab[actualoperand] | (1 << 15)); // saved to object code
                        }
                        else
                        {
                            fprintf(fw2, "%s%04X\n", optab[op].c_str(), 0);
                            objectCode << optab[op] << setw(4) << setfill('0') << hex << 0;
                            cout << RED_COLOR << "Error:" << RESET_COLOR << actualoperand << " is undefined symbol/label" << endl;
                            numberOfUndefinedSymbolErrors++;
                        }
                    }
                    else
                    {
                        if (symtab.find(operand) != symtab.end())
                        {
                            fprintf(fw2, "%s%04X\n", optab[op].c_str(), symtab[operand]);
                            objectCode << optab[op] << setw(4) << setfill('0') << hex << symtab[operand];
                        }
                        else
                        {
                            fprintf(fw2, "%s%04X\n", optab[op].c_str(), 0);
                            objectCode << optab[op] << setw(4) << setfill('0') << hex << 0;
                            cout << RED_COLOR << "Error:" << RESET_COLOR << operand << " is undefined symbol/label" << endl;
                            numberOfUndefinedSymbolErrors++;
                        }
                    }
                }
                else
                {
                    fprintf(fw2, "%s%04X\n", optab[op].c_str(), 0); // 0 taken as symbols value,if its not present
                    objectCode << optab[op] << setw(4) << setfill('0') << hex << 0;
                }
                ctr += 3;
                LOCCTR += 3;
            }
            else if (strcmp(op, "BYTE") == 0 || strcmp(op, "WORD") == 0) // object code is generated accordingly
            {
                if (operand[0] == 'C')
                {
                    for (int i = 2; i < strlen(operand) - 1; i++)
                        objectCode << hex << (int)operand[i];//when C is there it must be converted to integer ascii
                    fprintf(fw2, "%s\n", objectCode.str().c_str());
                    ctr += strlen(operand) - 3;
                    LOCCTR += strlen(operand) - 3;
                }
                else if (operand[0] == 'X')
                {
                    for (int i = 2; i < strlen(operand) - 1; i++)
                        objectCode << operand[i]; // here directly the operand is stored as object code
                    fprintf(fw2, "%s\n", objectCode.str().c_str());
                    ctr += (strlen(operand) - 3) / 2;
                    LOCCTR += (strlen(operand) - 3) / 2;
                }
                else
                {
                    objectCode << setw(6) << setfill('0') << hex << atoi(operand);
                    fprintf(fw2, "%s\n", objectCode.str().c_str());
                    ctr += 3;
                    LOCCTR += 3;
                }
            }
            if (ctr > 30) // text record is full so keep it in object program
            {
                textRecord << "\n";
                fprintf(fw, "T^%06X^%02X%s", prevaddress, ctr - (LOCCTR - prevloc), textRecord.str().c_str()); // starting address of text record and size of it is also stored at the starting
                prevaddress = prevloc;                                                                         // for next record
                textRecord.str("");
                ctr = LOCCTR - prevloc;
            }
            textRecord << "^" << objectCode.str().c_str();
        }
        if (fgets(line, 70, fr) == NULL) // next line is fetched to generate its object code
        {
            cond = false;
        }
        while (notacomment(line) == 0)
        {
            fprintf(fw2, "%s", line);
            if (fgets(line, 70, fr) == NULL)
            {
                cond = false;
                break;
            }
        }
        if (cond == false)
            break;
        removeTrailingSpaces(line);
        strcpy(templine, line);
        memset(label, 0, sizeof(label));
        memset(op, 0, sizeof(op));
        memset(operand, 0, sizeof(operand));
        memset(memaddress, 0, sizeof(memaddress));
        parsePass2(line, memaddress, label, op, operand);
        byteForPass2(templine, memaddress, label, op, operand);
        if ((strcmp(op, "RESW") == 0) || (strcmp(op, "RESB") == 0)) // no object code is generated and text record stops
        {
            textRecord << "\n";
            fprintf(fw, "T^%06X^%02X%s", prevaddress, ctr, textRecord.str().c_str());
            textRecord.str("");
            ctr = 0;
        }
        bool entered = false;
        while ((strcmp(op, "RESW") == 0) || (strcmp(op, "RESB") == 0))
        {
            entered = true;
            fprintf(fw2, "%s\t%s\t%s\t%s\t\n", memaddress, label, op, operand);
            if (fgets(line, 70, fr) == NULL)
            {
                cond = false;
                break;
            }
            while (notacomment(line) == 0)
            {
                fprintf(fw2, "%s", line);
                if (fgets(line, 70, fr) == NULL)
                {
                    cond = false;
                    break;
                }
            }
            if (cond == false)
                break;
            removeTrailingSpaces(line);
            strcpy(templine, line);
            memset(label, 0, sizeof(label));
            memset(op, 0, sizeof(op));
            memset(operand, 0, sizeof(operand));
            memset(memaddress, 0, sizeof(memaddress));
            parsePass2(line, memaddress, label, op, operand);
            byteForPass2(templine, memaddress, label, op, operand);
        }
        if (entered)
        {
            // set loc counter to memaddress which would then be set to prevloc
            stringstream hexstringtodec(memaddress);
            hexstringtodec >> hex >> LOCCTR;
            prevaddress = LOCCTR;
        }
        if (feof(fr) && (strcmp(op, "RESB") == 0 || strcmp(op, "RESW") == 0)) // end of file has reached and no object code to generate then stop the execution
        {
            break;
        }
    }
    // if more text record is still left,then print it in object program
    if (strcmp(textRecord.str().c_str(), "") != 0)
    {
        textRecord << "\n";
        fprintf(fw, "T^%06X^%02X%s", prevaddress, ctr, textRecord.str().c_str());
        textRecord.str("");
    }
    fprintf(fw, "E^%06X\n", startAddress);
    if (strcmp(op, "END") == 0)//else the line would be already written to the file
    {
        fprintf(fw2, "%s\t%s\t%s\t%s\t\n", memaddress, label, op, operand);
    }
    fclose(fr);
    fclose(fw);
    fclose(fw2);
    cout << "--------------------------------assemblylisting.txt---------------------------------\n";
    fw = fopen("assemblylisting.txt", "r");
    while (fgets(line, 50, fw))
    {
        cout << line;
    }
    fclose(fw);
    cout << "--------------------------------Final program---------------------------------\n";
    fw = fopen(nameOfProgram, "r");
    while (fgets(line, 50, fw))
    {
        cout << line;
    }
    fclose(fw);
    return true;
}
int main()
{
    char fileName[15];
    cout << "Instructions:\n";
    cout << "1.Enter the valid file name." << endl;
    cout << "2.The file should have correct machine instructions,else the execution of the program stops before generating full object code.\n";
    cout << "3.If opcode is undefined or label is redefined those errors would be printed after the first pass.\n";
    cout << "4.Undefined label errors would be printed after pass2" << endl;
    cout << "5.If end is not detected,still the program runs\n";
    cout << "6.All errors are printed in the terminal" << endl;
    cout << endl;
    cout << "Enter the file name for which you want object program:";
    cin >> fileName;
    Assembler assembler;
    if (assembler.setFile(fileName))
    {
        cout << "Successful reading from the file" << endl;
    }
    else
    {
        return 0;
    }
    if (assembler.pass1())
    {
        cout << "Pass1 is successfully completed\n";
        cout<< "Symbal Table stored is:\n";
        for (auto it : assembler.symtab)
        {
            cout << it.first << ":" << it.second << ":\n";
        }
        cout << "------------------------------------------------------------------------------------\n";
    }
    else
    {
        cout << RED_COLOR << "Number of redefined labels:" << RESET_COLOR << assembler.numberofReDefinedSymbolErrors << endl;
        cout << RED_COLOR << "Number of undefined opcodes:" << RESET_COLOR << assembler.numberOfInvalidOpcodeErrors << endl;
        return 0;
    }
    //if more than 3 words are there stop the program
    if(assembler.moreWords)
    {
        cout << RED_COLOR << "Number of redefined labels:" << RESET_COLOR << assembler.numberofReDefinedSymbolErrors << endl;
        cout << RED_COLOR << "Number of undefined opcodes:" << RESET_COLOR << assembler.numberOfInvalidOpcodeErrors << endl;
        cout<<"More words than expected\n";
        return 0;
    }
    if (assembler.pass2())
    {
        cout << "Pass2 is successfully completed\n";
    }
    cout << RED_COLOR << "Number of redefined labels:" << RESET_COLOR << assembler.numberofReDefinedSymbolErrors << endl;
    cout << RED_COLOR << "Number of undefined opcodes:" << RESET_COLOR << assembler.numberOfInvalidOpcodeErrors << endl;
    cout << RED_COLOR << "Number of undefined labels:" << RESET_COLOR << assembler.numberOfUndefinedSymbolErrors << endl;
    return 0;
}
