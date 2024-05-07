#include<bits/stdc++.h>
using namespace std;
// to give colors to errors which occur when there are redefined symbols,or,undefined symbols or undefined opcode
#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
map<string, string> optab;
set<string> controlSections;
void setOptab()
{
    optab["LDA"] = "00";
    optab["LDX"] = "04";
    optab["LDL"] = "08";
    optab["LDB"]="68";
    optab["LDT"]="74";
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
    optab["COMPR"]="A0";
    optab["CLEAR"]="B4";
    optab["J"] = "3C";
    optab["JLT"] = "38";
    optab["JEQ"] = "30";
    optab["JGT"] = "34";
    optab["JSUB"] = "48";
    optab["RSUB"] = "4C";
    optab["TIX"] = "2C";
    optab["TIXR"]="B8";
    optab["TD"] = "E0";
    optab["RD"] = "D8";
    optab["WD"] = "DC";
}
// symbol table which is filled during pass1
map<pair<string,string>,pair<int,int>> symtab;// CSECT,symbol and startaddress ,value
map<pair<string,string>,pair<bool,int>> littab;//similar to symbols for value 0-absolute,1-relative
set<string>extref;//changed after encounter of each criticla section
bool is_number(string s)
{
	auto it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

class Assembler
{
    public:
    int LOCCTR;
    // opcode table which is intialized before pass1
    string inputFile,programName,endoperand;
    vector<string> parsePass1(string line)
    {
        vector<string> tokens;
        string label,opcode,operands;
        int length=line.length();
        label="";
        opcode="";
        operands="";
        if(line.length()>0) label = removeLeadingTrailingSpaces(line.substr(0,min(10,length)));
        if(line.length()>11) opcode = removeLeadingTrailingSpaces(line.substr(11,min(10,length-11)));
        if(line.length()>22) operands=removeLeadingTrailingSpaces(line.substr(22,line.length()-22));
        tokens.push_back(label);
        tokens.push_back(opcode);
        tokens.push_back(operands);
        return tokens;
    }
    vector<string>parsePass2(string line)
    {
        vector<string> tokens;
        string mem="",label="",opcode="",operands="";
        int length=line.length();
        if(length>0)
        {
        if(length>0)mem = removeLeadingTrailingSpaces(line.substr(0,min(10,length)));
        //cout<<mem<<endl;
        if(length>10)label = removeLeadingTrailingSpaces(line.substr(10,min(10,length-10)));
        if(length>21)opcode = removeLeadingTrailingSpaces(line.substr(21,min(10,length-21)));
        if(length>32)operands=removeLeadingTrailingSpaces(line.substr(32,length-32));}
        //cout<<"operand:"<<operands<<endl;
        tokens.push_back(mem);
        tokens.push_back(label);
        tokens.push_back(opcode);
        tokens.push_back(operands);
        return tokens;
    }
    vector<string> separate(string op)
    {
        vector<string> ans;
		int i = 0;
		int l =  op.length();
		while(i < l)
		{
			int j = i + 1;
			while(j < l && op[j] != ',')
				j++;
			ans.push_back(op.substr(i, j - i));
			i = j + 1;
		}
		return ans;
    }
    void printmem(int address,bool empty,ofstream &fout)
    {
        if(empty==false)
        {
            fout<<left<<setfill(' ')<<setw(10)<<"";
        }
        else
        {
            stringstream ss;
            ss<<hex << uppercase << setfill('0') << setw(4) << address;
            fout<<left<<setfill(' ')<<setw(10)<< ss.str();//4 or 5 based on mem size
        }
        return;
    }
    vector<string> operatorsSeparate(string exp)
    {
		vector<string>res;
        int i=0,j;
        int length=exp.length();
        while(i<length)
        {
            if(exp[i]=='+'||exp[i]=='-'||exp[i]=='/'||exp[i]=='*'||exp[i]=='('||exp[i]==')')
            {
                res.push_back(exp.substr(i,1));
                i++;
            }
            else
            {
                j=i;
                while(j<length && !(exp[j]=='+'||exp[j]=='-'||exp[j]=='/'||exp[j]=='*'||exp[j]=='('||exp[j]==')')) j++;
                res.push_back(exp.substr(i,j-i));
                i=j;
            }
        }
        return res;

    }
    bool pass1();
    bool pass2();
    bool checkForDuplicateControlSections(string label)
    {
        if(controlSections.find(label)!=controlSections.end()) return true;
        controlSections.insert(label);
        return false;
    }
    void negativecheck(int &num,int length)
    {
        if(num<0 && length>2)
        {
            int mask = 0;
			for(int i = 0; i < ((length == 3) ? 12 : 20); i++)
			{
				mask |= 1;
				mask = mask << 1;
			}
			mask = mask >> 1;
			num =num & mask;
        }
    }
    void insertRegMnemonics(string CSECT)
    {
        symtab[{CSECT,"A"}]={0,0};
        symtab[{CSECT,"X"}]={0,1};
        symtab[{CSECT,"L"}]={0,2};
        symtab[{CSECT,"PC"}]={0,8};
        symtab[{CSECT,"SW"}]={0,9};
        symtab[{CSECT,"B"}]={0,3};
        symtab[{CSECT,"S"}]={0,4};
        symtab[{CSECT,"T"}]={0,5};
        symtab[{CSECT,"F"}]={0,6};
    }
    string removeLeadingTrailingSpaces(string str) {
    size_t start = str.find_first_not_of(" \t");
    if (start != string::npos) {
        str = str.substr(start);
    } else {
        str.clear();
        return str;
    }
    size_t end = str.find_last_not_of(" \t");
    if (end != string::npos) {
        str = str.substr(0, end + 1);
    }
    return str;}
};
class ExpressionEvaluator
{
    public:
    int value;
    bool isrelative;
    string exp;
    string CSECT;
    int preced(string op)
    {
        if(op=="+") return 1;
        if(op=="-") return 0;
        if(op=="*") return 2;
        if(op=="/") return 3;
        return -1;
    }
    ExpressionEvaluator(string CSECT,string exp)
    {
        this->exp=exp;
        this->value=0;
        this->isrelative=false;
        this->CSECT=CSECT;
    }
    pair<int,int> apply(pair<int,int> o1,pair<int,int> o2,string op)
    {
        if(op == "+") return make_pair(o1.first | o2.first, o1.second + o2.second);
		if(op == "-") return make_pair(o1.first ^ o2.first, o1.second - o2.second);
		if(op == "*") return make_pair(o1.first | o2.first, o1.second * o2.second);
		if(op == "/") return make_pair(o1.first | o2.first, o1.second / o2.second);
		return make_pair(0, 0);
    }
    vector<string> tokenize()
	{
		vector<string>res;
        int i=0,j;
        int length=exp.length();
        while(i<length)
        {
            if(exp[i]=='+'||exp[i]=='-'||exp[i]=='/'||exp[i]=='*'||exp[i]=='('||exp[i]==')')
            {
                res.push_back(exp.substr(i,1));
                i++;
            }
            else
            {
                j=i;
                while(j<length && !(exp[j]=='+'||exp[j]=='-'||exp[j]=='/'||exp[j]=='*'||exp[j]=='('||exp[j]==')')) j++;
                res.push_back(exp.substr(i,j-i));
                i=j;
            }
        }
        return res;
	}
    bool evaluate()
    {
        //cout<<"exp:"<<exp<<endl;
        vector<string> tokens=tokenize();
        stack<string> ops;
        stack<pair<int,int>> operands;
        int size=tokens.size();
        //cout<<"size:"<<size<<endl;
        for(int i=0;i<size;i++)
        {
            if(is_number(tokens[i])) 
            {
                int tempval=stoi(tokens[i]);
                bool temptype=0;
                if(operands.empty())
                {
                    if(!ops.empty())
                    {
                        if(ops.top()=="-")
                        {
                            tempval=-1*tempval;
                        }
                        ops.pop();
                    }
                }
                operands.push({temptype,tempval});
            }
            else if(tokens[i] == "(") ops.push(tokens[i]);
            else if(tokens[i]==")")
            {
                while(ops.empty()==false&&ops.top()!="(")
                {
                    pair<int, int> operand2 = operands.top();
					operands.pop();
                    if(operands.empty())
                    {
                        if(ops.empty()) this->value=operand2.second;
                        else if(ops.top()=="-")
                        {this->value=-1*operand2.second;ops.pop();}
                        else if(ops.top()=="+")
                        {this->value=operand2.second;ops.pop();}
                        this->isrelative=operand2.first;
                        operands.push(operand2);
                        continue;
                    }
					pair<int, int> operand1 = operands.top();
					operands.pop();
                    //if(operand1.first^operand2.first) {cout<<operand1.second<<endl;cout<<operand2.second<<endl;cout<<"Invalid Expressions, abs + rel,abs - rel,abs/rel,abs*rel are not allowed"<<endl;return false;}
					string op = ops.top();
                    if((op=="/"||op=="*")&&(operand1.first==1||operand2.first==1))
                    {
                        cout<<"Relative terms cant be multipled or divided"<<endl;
                        return false;
                    }
                    if(op=="-"&&operand1.first==0&&operand2.first==1)
                    {
                        cout<<"Ansolute-Relative is not allowed"<<endl;
                        return false;
                    }
					ops.pop();
					operands.push(apply(operand1, operand2, op));
                }
                if(ops.empty()==false) ops.pop();
            }
            else if(tokens[i]=="+"||tokens[i]=="-"||tokens[i]=="/"||tokens[i]=="*")
            {
                while(!ops.empty()&&preced(ops.top())>=preced(tokens[i]))
                {
                    pair<int, int> operand2 = operands.top();
					operands.pop();
                    if(operands.empty())
                    {
                        if(ops.empty()) this->value=operand2.second;
                        else if(ops.top()=="-")
                        {this->value=-1*operand2.second;ops.pop();}
                        else if(ops.top()=="+")
                        {this->value=operand2.second;ops.pop();}
                        this->isrelative=operand2.first;
                        operands.push(operand2);
                        ops.push(tokens[i]);
                        continue;
                    }
					pair<int, int> operand1 = operands.top();
					operands.pop();
                    //if(operand1.first^operand2.first) {cout<<operand1.second<<endl;cout<<operand2.second<<endl;cout<<"Invalid Expressions, abs + rel,abs - rel,abs/rel,abs*rel are not allowed"<<endl;return false;}
					string op = ops.top();
                    if((op=="/"||op=="*")&&(operand1.first==1||operand2.first==1))
                    {
                        cout<<"Relative terms cant be multipled or divided"<<endl;
                        return false;
                    }
                    if(op=="-"&&operand1.first==0&&operand2.first==1)
                    {
                        cout<<"Ansolute-Relative is not allowed"<<endl;
                        return false;
                    }
					ops.pop();
					operands.push(apply(operand1, operand2, op));
                }
                ops.push(tokens[i]);
            }
            else//using symbols
            {
                if(symtab.find({CSECT,tokens[i]})!=symtab.end())
                {
                    int tempval=symtab[{CSECT,tokens[i]}].second;
                    bool temptype=symtab[{CSECT,tokens[i]}].first;
                    if(operands.empty())
                    {
                        if(!ops.empty())
                        {
                            if(ops.top()=="-")
                            {
                                tempval=-1*tempval;
                            }
                            ops.pop();
                        }
                    }
                    //cout<<ops.size()<<endl;
                    operands.push({temptype,tempval});
                    //cout<<"tokens[i]"<<tokens[i]<<":"<<symtab[{CSECT,tokens[i]}].second<<endl;
                }
                else
                {
                    //operands.push({0,0});//if not there then assumed that it must be external reference,it will later be checked too
                    if(!ops.empty()) ops.pop();
                }
                
            }
        }
        //cout<<".."<<endl;
        //cout<<operands.size()<<endl;
        //cout<<ops.size()<<endl;
        while(!ops.empty())
        {
            pair<int, int> operand2 = operands.top();
			operands.pop();
            if(operands.empty())
            {
                //cout<<"//"<<endl;
                if(ops.empty()) this->value=operand2.second;
                else if(ops.top()=="-")
                    {this->value=-1*operand2.second;ops.pop();}
                else if(ops.top()=="+")
                    {this->value=operand2.second;ops.pop();}
                this->isrelative=operand2.first;
                break;
            }
			pair<int, int> operand1 = operands.top();
			operands.pop();
            //cout<<"-----------\n";
            //cout<<hex<<operand2.second<<endl;
            //cout<<hex<<operand1.second<<endl;
            //if(operand1.first^operand2.first) {cout<<operand1.second<<endl;cout<<operand2.second<<endl;cout<<"Invalid Expressions, abs + rel,abs - rel,abs/rel,abs*rel are not allowed"<<endl;return false;}
			string op = ops.top();
            if((op=="/"||op=="*")&&(operand1.first==1||operand2.first==1))
            {
                cout<<"Relative terms cant be multipled or divided"<<endl;
                return false;
            }
            if(op=="-"&&operand1.first==0&&operand2.first==1)
            {
                cout<<"Ansolute-Relative is not allowed"<<endl;
                return false;
            }
			ops.pop();
			operands.push(apply(operand1, operand2, op));
            //cout<<"+++++++++++\n";
            //cout<<apply(operand1,operand2,op).second<<endl;
        }
        if(!operands.empty())
        {
            this->isrelative=operands.top().first;
            this->value=operands.top().second;
        }
        //cout<<value<<endl;
        return true;
    }
};
bool Assembler::pass1()
{
    LOCCTR=0;
    int startAddress=0,sizeOfProgram;
    ifstream fin(inputFile);
    ofstream fout("intermediate.txt");
    if(!fin.is_open())
    {
        cout<<"error opening input file"<<endl;
        return false;
    }
    if(!fout.is_open())
    {
        cout<<"error opening file"<<endl;
        return false;
    }
    string label,opcode,operand;
    string prevCSECT="";
    if(istreambuf_iterator<char>(fin) == istreambuf_iterator<char>()) 
    {
        cout<<"Empty file"<<endl;
        return 0;
    }
    while(!fin.eof())
    {
        string instr;
        getline(fin,instr);
        if(instr.empty()) continue;
        int n,i,x,b,p,e;
        bool isliteral=false;//if = in before the operand then true
        if(instr[21] != ' ')//to set the instruction type,addressing modes and decide if literal is there or not
	    {
		    char x = instr[21];
		    if(x == '#')
		    {
			    i = 1;
			    n = 0;
			    if(is_number(operand))
			    {
				    p = 0;
				    b = 0;
			    }
		    }
		    else if(x == '@')
		    {
			    i = 0;
			    n = 1;
		    }
		    else if(x == '=') isliteral=true;
	    } 
        vector<string> tokens=parsePass1(instr);
        label=tokens[0];
        opcode=tokens[1];
        operand=tokens[2];
        vector<string> operands=separate(operand);//as many operands could be declared in EXTREF or indexed addressing etc
        if(label[0]!='.')//not a comment
        {
            if(opcode=="START"||opcode=="CSECT")
            {
                if(checkForDuplicateControlSections(label))
                {
                    cout<<"Duplicate Control sectiond found"<<endl;
                    return 0;
                }
                if(prevCSECT!="") symtab[{prevCSECT,prevCSECT}]={startAddress,LOCCTR-startAddress};
                prevCSECT=label;
                if(operand!="") startAddress=stoi(operand);
                else startAddress=0;
                LOCCTR=startAddress;
                insertRegMnemonics(label);
                printmem(LOCCTR,true,fout);
                fout<<instr<<endl;
            }
            else if(opcode=="BASE" || opcode=="EXTREF" || opcode=="EXTDEF") //The operand of BASE specifies the label or address whose value will be loaded into the base register.
            {
                //only assembler directive so no mem
                printmem(0,false,fout);
                fout<<instr<<endl;
            }
            else if(opcode=="LTORG"||opcode=="END")//then all literals must now be given instruction format
            {
                //as LTORG(END) is only assembler directive no address is assigned to this
                printmem(0,false,fout);//4 or 5 based on mem size
                fout<<instr<<endl;
                //printing all the literals available till then
                for(auto &ele:littab)
                {
                    if(ele.second.second==-1)
                    {
                    ele.second.second=LOCCTR;
                    printmem(LOCCTR,true,fout);//for mem
                    fout<<left<<setfill(' ')<<setw(10)<< "*";//for label
                    fout<<"="<<left<<setfill(' ')<<setw(10)<< ele.first.second;//for opcode
                    fout<<left<<setfill(' ')<<setw(10)<< "";//for operand
                    fout<<endl;
					if(ele.first.second[0] == 'C')//increasing the LOCCTR according to the literal stored
						LOCCTR += (ele.first.second.length() - 3);
					else if(ele.first.second[0] == 'X')
						LOCCTR += ((ele.first.second.length() - 3)/2);
					else
						LOCCTR += 3;
                    }
                }
                if(opcode=="END")
                {
                    symtab[{prevCSECT,prevCSECT}]={startAddress,LOCCTR-startAddress};//total program ended,so last CSECT also should be entered
                    //as it is also an external symbol
                    endoperand=operand;
                }
                
            }
            else if(opcode=="EQU")//then adding to the symtab
            {
                if(label!="")
                {
                    if(symtab.find({prevCSECT,label})==symtab.end())
                    {
                        if(operand=="*")//rel and LOCCTR
                        {
                            symtab[{prevCSECT,label}]={1,LOCCTR};
                            printmem(LOCCTR,true,fout);
                            fout<<instr<<endl;
                        }else if(is_number(operand))
                        {
                            symtab[{prevCSECT,label}]={0,stoi(operand)};
                            printmem(stoi(operand),true,fout);
                            fout<<instr<<endl;
                        }
                        else
                        {
                            //then we have to evaluate the expression
                            ExpressionEvaluator ex(prevCSECT,operand);
                            if(!ex.evaluate())
                            {
                                return false;
                            }
                            int value=ex.value;
                            //cout<<hex<<value<<endl;
                            bool type=ex.isrelative;
                            symtab[{prevCSECT,label}]={type,value};
                            printmem(value,true,fout);
                            fout<<instr<<endl;
                        }
                    }
                    else
                    {
                        cout<<"Duplicate symbol is encountered"<<endl;
                        return false;
                    }
                }else
                {
                    cout<<"Incorrect instruction, no label for EQU operand"<<endl;
                    return false;
                }
            }
            else
            {
                printmem(LOCCTR,true,fout);
                fout<<instr<<endl;
                if(label!="")
                {
                    if(symtab.find({prevCSECT,label})!=symtab.end())
                    {
                        cout<<"label redefined"<<endl;
                        return false;
                    }
                    else  symtab[{prevCSECT,label}]={1,LOCCTR};
                }
                if(isliteral)
                {
                    if(littab.find({prevCSECT,operand})==littab.end()) littab[{prevCSECT,operand}]={1,-1};
                }
                //LOCCTR increment
                if (optab.find(opcode) != optab.end()) 
                {
                    LOCCTR += 3;
                    if(instr[10]=='+') LOCCTR++;//accounting for other instruction formats,for SIC/XE 4 length is only when + is there or check for e bit
                    if(opcode=="COMPR"||opcode=="CLEAR"||opcode=="TIXR") LOCCTR--;//only these opcodes have length of 2 as its instruction size
                }
                else if (opcode=="WORD") LOCCTR += 3;
                else if (opcode=="RESW")LOCCTR += (3 * stoi(operand));
                else if (opcode=="RESB") LOCCTR += (stoi(operand));
                else if (opcode=="BYTE")
                {
                    if (operand[0] == 'X')
                    {
                        LOCCTR += (operand.length()- 3) / 2;
                    }
                    else if (operand[0] == 'C')
                    {
                        LOCCTR += (operand.length() - 3);
                    }
                    else
                    {
                        LOCCTR += (operand.length());
                    }
                }
                else
                {
                    cout<<"Invalid opcode :"<<opcode<<": was given"<<endl;
                    return false;
                }
            }
        }
        else
        {
            fout<<instr<<endl;
        }
    }
    fin.close();
    fout.close();
    return true;
}
bool Assembler::pass2()
{
    LOCCTR=0;
    int BASE=0,PC=0,startAddress=0;
    ifstream fin("intermediate.txt");
    ofstream fout("assemblyListing.txt");
    ofstream fobject("objectProgram.txt");
    if(!fin.is_open())
    {
        cout<<"error opening input file"<<endl;
        return false;
    }
    if(!fout.is_open())
    {
        cout<<"error opening file"<<endl;
        return false;
    }
    if(!fobject.is_open())
    {
        cout<<"error opening file"<<endl;
        return false;
    }
    string prevCSECT="";
    string mem,label,opcode,operand;
    string currentText="";
    set<string> extref;
    map<string,vector<string>> modificationRecords;
    int currentStart,currentLength=0;
    bool firstOver =false;
    int prevLOCCTR;
    while(!fin.eof())
    {
        string instr;
        getline(fin,instr);
        cout<<instr<<endl;
        if(instr.empty()) continue;
        string commentcheck=removeLeadingTrailingSpaces(instr);
        if(commentcheck[0]=='.')
        {
            fout<<commentcheck<<endl;
            continue;
        }
        int n=1,i=1,x=0,b=0,p=1,e=0;
        bool isliteral=false;//if = in before the operand then true
        vector<string> tokens=parsePass2(instr);
        mem=tokens[0];
        label=tokens[1];
        opcode=tokens[2];
        operand=tokens[3];
        if(instr[20] == '+') e = 1,p = 0,b = 0,n = 1,i = 1;
        if(instr[31] != ' ')//to set the instruction type,addressing modes and decide if literal is there or not
	    {
		    char x = instr[31];
		    if(x == '#')
		    {
			    i = 1;
			    n = 0;
			    if(is_number(operand))
			    {
				    p = 0;
				    b = 0;
			    }
		    }
		    else if(x == '@')
		    {
			    i = 0;
			    n = 1;
		    }
		    else if(x == '=') isliteral=true;
	    }
        vector<string> operands=separate(operand);//as many operands could be declared in EXTREF or indexed addressing etc
        if(label[0]!='.')//not a comment
        {
            if(opcode=="START")
            {
                prevCSECT=label;
                fout<<instr<<endl;
                programName=label;
                if(operand!="") startAddress=stoi(operand);
                else startAddress=0;
                LOCCTR=startAddress;
                currentStart=LOCCTR;
                fobject<<"H"<<left<<setfill(' ')<<setw(6)<<label;
                fobject<<"";
                fobject<<right<<hex << uppercase << setw(6) <<setfill('0') << startAddress;
                fobject<<"";
                fobject<<right<<hex << uppercase << setw(6)<<setfill('0') << symtab[{label,label}].second;//length
                fobject<<endl;
                //criticalSections.push_back(label);
            }
            else if(opcode=="CSECT")
            {
                fout<<instr<<endl;
                if(currentText!="")
                {
                    stringstream ss;
                    ss<<"T"<<right<<hex << uppercase << setw(6)<<setfill('0') << currentStart;
                    ss<<""<<right<<hex << uppercase <<setw(2)<< setfill('0') << (currentLength/2);
                    ss<<""<<currentText;
                    fobject<<ss.str()<<endl;
                    currentStart=0;
                    currentText="";
                    currentLength=0;
                }
                for(auto it:modificationRecords[prevCSECT])
                {
                    fobject<<it<<endl;
                }
                fobject<<"E";
                if(firstOver==false)
                {
                    firstOver=true;
                    if(endoperand!="")
                    {
                        fobject<<""<<right<<hex << uppercase << setw(6)<<setfill('0') << symtab[{prevCSECT,endoperand}].second;
                    }
                }
                fobject<<endl;
                prevCSECT=label;
                // criticalSections.push_back(label);
                if(operand!="") startAddress=stoi(operand);
                else startAddress=0;
                LOCCTR=startAddress;
                currentStart=LOCCTR;
                stringstream h;
                h<<"H"<<left<<setfill(' ')<<setw(6)<<label;
                h<<"";
                h<<right<<hex << uppercase << setw(6)<<setfill('0')  << startAddress;
                h<<"";
                h<<right<<hex << uppercase << setw(6)<<setfill('0') << symtab[{label,label}].second;//length
                fobject<<h.str()<<endl;
            }
            else if(opcode=="END")
            {
                fout<<instr<<endl;
            }
            else if(opcode == "EXTDEF")
            {
                fout<<instr<<endl;
                fobject<<"D";
                for(auto ele:operands)
                {
                    fobject<<""<<left<<setfill(' ')<<setw(6)<<ele;
                    int address=0;
                    if(symtab.find({prevCSECT,ele})!=symtab.end()) address=symtab[{prevCSECT,ele}].second;
                    fobject<<""<<right<<setw(6)<<setfill('0') <<hex << uppercase <<address;
                }
                fobject<<endl;
            } else if(opcode=="EXTREF")
            {
                fout<<instr<<endl;
                fobject << "R";
                extref.clear();
				for(string ele : operands)
                {
                    extref.insert(ele);
					fobject <<""<< left<<setfill(' ')<<setw(6)<<ele;
                }
                fobject<<endl;
            } else if(opcode=="LTORG" || opcode=="EQU")
            {
                fout<<instr<<endl;
            } else if(opcode=="BASE")
            {
                fout<<instr<<endl;
                if(symtab.find({prevCSECT,operand})!=symtab.end()) BASE=symtab[{prevCSECT,operand}].second;
            } else if(opcode=="ORG")
            {
                fout<<instr<<endl;
                LOCCTR=stoi(operand,nullptr,16);//reset the LOCCTR
            } else
            {
                if(mem!="")
                {
                    LOCCTR=stoi(mem,nullptr,16);
                    prevLOCCTR=LOCCTR;
                }
                stringstream objectcode;
                if(optab.find(opcode)!=optab.end())
                {
                    LOCCTR += 3;
                    if(e==1) LOCCTR++;//accounting for other instruction formats,for SIC/XE 4 length is only when + is there or check for e bit
                    if(opcode=="COMPR"||opcode=="CLEAR"||opcode=="TIXR") LOCCTR--;
                    PC=LOCCTR;
                    int operandcode=0;
                    if(operand!="")
                    {
                        if(operands.size()>1)
                        {
                            if(opcode=="COMPR"||opcode=="CLEAR"||opcode=="TIXR")//only 2 bytes ,so reg-reg
                            {
                                for(auto ele:operands)
                                {
                                    if(symtab.find({prevCSECT,ele})!=symtab.end())
                                    {
                                        operandcode=operandcode<<4;
                                        operandcode+=symtab[{prevCSECT,ele}].second;
                                    }
                                    else
                                    {
                                        cout<<"The register mnemonic is not found in symbol table:"<<ele<<endl;
                                        return false;
                                    }
                                }
                            }
                            else 
                            {
                                if(operands[1]=="X")
                                {
                                    if(i==1&&!n)
                                    {
                                        cout<<"Immediate addressing is not allowed with indexed addressing"<<endl;
                                        return false;
                                    }
                                    if(symtab.find({prevCSECT,operands[0]})!=symtab.end())
                                    {
                                        operandcode=symtab[{prevCSECT,operands[0]}].second;
                                        if(e!=1)
                                        {
                                            if(operandcode-PC>=-2048&&operandcode-PC<=2047)
                                            {
                                                operandcode=operandcode-PC;
                                                b=0;
                                                p=1;
                                            }
                                            else if(operandcode-BASE>=0&&operandcode-BASE<=4095)
                                            {
                                                operandcode=operandcode-BASE;
                                                b=1;
                                                p=0;
                                            }
                                            else
                                            {
                                                cout<<"The instruction cant fit into3-length format"<<endl;
                                                return false;
                                            }
                                        }
                                    }
                                    else 
                                    {
                                        if(extref.find(operands[0])!=extref.end())
                                        {
                                            stringstream m;
                                            m<<"M"<<right<<setw(6)<<setfill('0') <<hex << uppercase <<(LOCCTR-3);//start address of the symbol
                                            if(e==1) m<<"05+";//5 half bytes length
                                            else m<<"03+";
                                            m<<left<<setfill(' ')<<setw(6)<<operands[0];
                                            modificationRecords[prevCSECT].push_back(m.str());
                                        }
                                        else
                                        {
                                            cout<<"Unspecified external reference encountered"<<endl;
                                            return false;
                                        }
                                    }
                                    x=1;
                                }
                                else 
                                {
                                    cout<<"Incorrect Operand with commas"<<endl;
                                    return false;
                                }
                            }
                        }
                        else//no multiple operands but operand is there
                        {
                            if(isliteral)
                            {
                                if(littab.find({prevCSECT,operand})!=littab.end())
                                {
                                    operandcode=littab[{prevCSECT,operand}].second;
                                    if(e!=1)
                                    {
                                        if(operandcode-PC>=-2048&&operandcode-PC<=2047)
                                        {
                                            operandcode=operandcode-PC;
                                            p=1;
                                            b=0;
                                        }
                                        else if(operandcode-BASE>=0&&operandcode-BASE<=4095)
                                        {
                                            operandcode=operandcode-BASE;
                                            b=1;
                                            p=0;
                                        }
                                        else
                                        {
                                            cout<<"The instruction cant fit into3-length format"<<endl;
                                            return false;
                                        }
                                    }
                                }
                                else
                                {
                                    cout<<"The literal was not found"<<endl;
                                    return false;
                                }
                            }
                            else if(symtab.find({prevCSECT,operand})!=symtab.end())
                            {
                                operandcode=symtab[{prevCSECT,operand}].second;
                                if(opcode=="COMPR"||opcode=="CLEAR"||opcode=="TIXR")
                                {
                                    operandcode=operandcode<<4;
                                }
                                else
                                {
                                    if(e!=1)
                                    {
                                        if(operandcode-PC>=-2048&&operandcode-PC<=2047)
                                        {
                                            operandcode=operandcode-PC;
                                            b=0;
                                            p=1;
                                        }
                                        else if(operandcode-BASE>=0&&operandcode-BASE<=4095)
                                        {
                                            operandcode=operandcode-BASE;
                                            b=1;
                                            p=0;
                                        }
                                        else
                                        {
                                            cout<<"The instruction cant fit into3-length format"<<endl;
                                            return false;
                                        }
                                    }
                                }
                            }else
                            {
                                // if(opcode=="LDA")
                                // {
                                //     cout<<operand<<endl;
                                //     cout<<"i="<<i<<endl;
                                // }
                                if(i&&!n)
                                {
                                    if(is_number(operand))
                                        operandcode=stoi(operand);
                                    else
                                    {
                                        ExpressionEvaluator e(prevCSECT,operand);//if external ref ,directly gives 0
                                        if(!e.evaluate())
                                        {
                                            //cout<<"Invalid Expressions, abs + rel,abs - rel,abs/rel,abs*rel are not allowed"<<endl;
                                            return false;
                                        }
                                        int value=e.value;
                                        bool type=e.isrelative;
                                        if(value<0)
                                        {
                                            int mask = 0;//for negative cases
		                                    for(int i = 0; i < 24; i++)
		                                    {
			                                    mask |= 1;
			                                    mask = mask << 1;
		                                    }
		                                    mask = mask >> 1;
		                                    value = value & mask;
                                        }
                                        operandcode=value;
                                        //objectcode << right<<uppercase<<setw(6) << setfill('0') << hex << value;
                                        //cout<<"--------------\n";
                                        vector<string> operands2=operatorsSeparate(operand);
                                        for(int i = 0; i < operands2.size(); i++)
							            {
                                            //cout<<operands[i]<<endl;
								            if(extref.find(operands2[i]) != extref.end())
								            {
									            stringstream m;
									            m << "M";
									            m << right<<uppercase<<setw(6) << setfill('0') << hex << prevLOCCTR;
									            m << "06";//word is 3 bytes
									            if(i > 0 && operands2[i-1] == "-")
										            m<< "-";
									            else
										            m << "+";
									            m << ""<<left<<uppercase<<setfill(' ')<<setw(6)<<operands2[i];
									            modificationRecords[prevCSECT].push_back(m.str());
								            }
                                            else if(operands2[i]=="*"||operands2[i]=="/"||operands2[i]=="+"||operands2[i]=="-"||operands2[i]=="("||operands2[i]==")")
                                            {

                                            }
                                            else
                                            {
                                                cout<<"External reference error"<<endl;
                                            //     return false;
                                            }
							            }
                                        if(type==0)
                                        {
                                            b=0;
                                            p=0;
                                        }
                                        else
                                        {
                                            if(operandcode-PC>=-2048&&operandcode-PC<=2047)
                                            {
                                                operandcode=operandcode-PC;
                                                b=0;
                                                p=1;
                                            }
                                            else if(operandcode-BASE>=0&&operandcode-BASE<=4095)
                                            {
                                                operandcode=operandcode-BASE;
                                                b=1;
                                                p=0;
                                            }
                                        }
                                    }
                                }
                                else if(e==1)
                                {
                                    ExpressionEvaluator e2(prevCSECT,operand);//if external ref ,directly gives 0
                                    if(!e2.evaluate())
                                    {
                                        //cout<<"Invalid Expressions, abs + rel,abs - rel,abs/rel,abs*rel are not allowed"<<endl;
                                        return false;
                                    }
                                    int value=e2.value;
                                    bool type=e2.isrelative;
                                    if(value<0)
                                    {
                                        int mask = 0;//for negative cases
		                                for(int i = 0; i < 24; i++)
		                                {
			                                mask |= 1;
			                                mask = mask << 1;
		                                }
		                                mask = mask >> 1;
		                                value = value & mask;
                                    }
                                    operandcode=value;
                                    //objectcode << right<<uppercase<<setw(6) << setfill('0') << hex << value;
                                    //cout<<"--------------\n";
                                    vector<string> operands2=operatorsSeparate(operand);
                                    for(int i = 0; i < operands2.size(); i++)
							        {
                                        //cout<<operands[i]<<endl;
								        if(extref.find(operands2[i]) != extref.end())
								        {
									        stringstream m;
									        m<<"M"<<right<<setw(6)<<setfill('0') <<hex << uppercase <<(LOCCTR-3);
                                            if(e==1) m<<"05";//5 half bytes length
                                            else m<<"03";
									        if(i > 0 && operands2[i-1] == "-")
										        m<< "-";
									        else
										        m << "+";
									        m << ""<<left<<uppercase<<setfill(' ')<<setw(6)<<operands2[i];
									        modificationRecords[prevCSECT].push_back(m.str());
								        }
                                        else if(operands2[i]=="*"||operands2[i]=="/"||operands2[i]=="+"||operands2[i]=="-"||operands2[i]=="("||operands2[i]==")")
                                        {

                                        }
                                        else
                                        {
                                            cout<<"External reference error"<<endl;
                                        //     return false;
                                        }
							        }
                                }
                                else if(extref.find(operands[0])!=extref.end())
                                {
                                    stringstream m;
                                    m<<"M"<<right<<setw(6)<<setfill('0') <<hex << uppercase <<(LOCCTR-3);
                                    if(e==1) m<<"05+";//5 half bytes length
                                    else m<<"03+";
                                    m<<left<<setfill(' ')<<setw(6)<<operand;
                                    modificationRecords[prevCSECT].push_back(m.str());
                                } 
                                else 
                                {
                                    cout<<"Unspecified external reference encountered"<<endl;
                                //     return false;
                                }
                                
                            }
                        }
                    }
                    //opcode is there, then all cases considered based on operands (there or not,if there multiple or not)
                    int opcodei=0;
                    stringstream ss(optab[opcode]);
                    ss>>hex>>opcodei;
                    negativecheck(operandcode,LOCCTR-prevLOCCTR);
                    if(operand.empty()) b=0,p=0,x=0;
                    if(!e&& LOCCTR-prevLOCCTR>2 )//if 2 is the length it doesnt have n,i,x,b,p,e 
			        {
				        operandcode |= (p << 13);
				        operandcode |= (b << 14);
				        operandcode |= (x << 15);
                        opcodei |= (i);
                        opcodei |= (n<<1);
			        }
			        else if(e==1 && LOCCTR-prevLOCCTR>2)
			        {
				        operandcode |= (e << 20);
				        operandcode |= (p << 21);
				        operandcode |= (b << 22);
				        operandcode |= (x << 23);
                        opcodei |= (i);
                        opcodei |= (n<<1);
			        }
                    fout<<instr;
                    objectcode<<right<<setw(2)<<setfill('0') <<hex << uppercase <<opcodei;
                    int len=2*(LOCCTR-prevLOCCTR)-2;
                    objectcode<<right<<setw(len)<<setfill('0') <<hex << uppercase <<operandcode;
                    fout<<objectcode.str()<<endl;
                    //if(opcode=="J") cout<<operand<<":"<<objectcode.str()<<":"<<LOCCTR-prevLOCCTR<<":"<<len<<":"<<e<<":opreandcode:"<<operandcode<<endl;

                }
                else
                {
                    if(label=="*")//literals
                    {
                        string literal = opcode.substr(2,opcode.length()-3);
                        if(opcode[0]=='X')
                        {
                            objectcode<<literal;
                            LOCCTR+=(literal.length()-3)/2;
                        }
                        else if(opcode[0]=='C')
                        {
                            for (int i = 2; i < opcode.length() - 1; i++)
                                objectcode << hex <<uppercase<< (int)opcode[i];
                            LOCCTR+=(literal.length()-3);
                        }
                        else
                        {
                            objectcode << right<<uppercase<<setw(6) << setfill('0') << hex << stoi(opcode);
                            LOCCTR+=opcode.length();
                        }
                    }
                    if(opcode=="WORD")
                    {
                        LOCCTR+=3;
                        if(!is_number(operand))
                        {
                            ExpressionEvaluator e(prevCSECT,operand);//if external ref ,directly gives 0
                            if(!e.evaluate())
                            {
                                //cout<<"Invalid Expressions, abs + rel,abs - rel,abs/rel,abs*rel are not allowed"<<endl;
                                return false;
                            }
                            int value=e.value;
                            bool type=e.isrelative;
                            if(value<0)
                            {
                                int mask = 0;//for negative cases
		                        for(int i = 0; i < 24; i++)
		                        {
			                        mask |= 1;
			                        mask = mask << 1;
		                        }
		                        mask = mask >> 1;
		                        value = value & mask;
                            }
                            objectcode << right<<uppercase<<setw(6) << setfill('0') << hex << value;
                            //cout<<"--------------\n";
                            vector<string> operands2=operatorsSeparate(operand);
                            for(int i = 0; i < operands2.size(); i++)
							{
                                //cout<<operands[i]<<endl;
								if(extref.find(operands2[i]) != extref.end())
								{
									stringstream m;
									m << "M";
									m << right<<uppercase<<setw(6) << setfill('0') << hex << prevLOCCTR;
									m << "06";//word is 3 bytes
									if(i > 0 && operands2[i-1] == "-")
										m<< "-";
									else
										m << "+";
									m << ""<<left<<uppercase<<setfill(' ')<<setw(6)<<operands2[i];
									modificationRecords[prevCSECT].push_back(m.str());
								}
                                else if(operands2[i]=="*"||operands2[i]=="/"||operands2[i]=="+"||operands2[i]=="-"||operands2[i]=="("||operands2[i]==")")
                                {

                                }
                                else
                                {
                                    cout<<"External reference error"<<endl;
                                //     return false;
                                }
							}
                        }
                    }else if(opcode=="BYTE")
                    {
                        string literal = operand.substr(2,operand.length()-3);
                        if(operand[0]=='X')
                        {
                            objectcode<<literal;
                            LOCCTR+=(literal.length()-3)/2;
                        }
                        else if(operand[0]=='C')
                        {
                            for (int i = 2; i < operand.length() - 1; i++)
                                objectcode << hex<< uppercase<< (int)operand[i];
                            LOCCTR+=(literal.length()-3);
                        }
                        else
                        {
                            objectcode << right<<uppercase<<setw(6) << setfill('0') << hex << stoi(operand);
                            LOCCTR+=(literal.length());
                        }
                    }
                    fout<<instr;
                    fout<<objectcode.str()<<endl;
                }
                if(currentText==""&&opcode!="RESW"&&opcode!="RESB") {currentLength=0;currentStart=prevLOCCTR;}
                //if(prevCSECT=="WRREC"&&!objectcode.str().empty()) cout<<objectcode.str()<<endl;
                if(currentLength+objectcode.str().length()<=60 && opcode!="RESW"&&opcode!="RESB"&&!objectcode.str().empty())
                {
                    if(currentText!="")
                    currentText=currentText+""+objectcode.str();
                    else currentText=objectcode.str();
                    currentLength+=objectcode.str().length();
                }
                else
                {
                    if(currentText!="")
                    {
                        fobject<<"T";
                        fobject<<right<<hex << uppercase << setw(6)<<setfill('0')  << currentStart;
                        fobject<<""<<right<<hex << uppercase << setw(2)<<setfill('0')  << currentLength/2;
                        fobject<<""<<currentText<<endl;
                        currentStart=prevLOCCTR;
                        currentText=objectcode.str();
                        currentLength=objectcode.str().length();
                    }
                }
            } 
            
        }
        else
        {
            fout<<instr<<endl;//only comment so,print it directly
        }
    }
    if(currentText!="")
    {
        //cout<<currentText<<endl;
        fobject<<"T";
        fobject<<right<<hex << uppercase << setw(6)<<setfill('0')  << currentStart;
        fobject<<""<<right<<hex << uppercase << setw(2)<<setfill('0')  << currentLength/2;
        fobject<<""<<currentText<<endl;
    }
    for(auto it:modificationRecords[prevCSECT])
    {
        fobject<<it<<endl;
    }
    fobject<<"E";
    if(firstOver=false)
    {
        if(endoperand!="")
        {
            fobject<<""<<right<<hex << uppercase <<setw(6)<< setfill('0')  << symtab[{prevCSECT,endoperand}].second;
        } 
    }
    fobject<<endl;
    fin.close();
    fout.close();
    fobject.close();
    return true;
}
void printtables()
{
    ofstream fout("tables.txt");
    if(!fout.is_open())
    {
        cout<<"Error opening file"<<endl;
        return;
    }
    fout<<"The symbol table:"<<endl;
    fout<<left<<setfill(' ')<<setw(15)<<"SYMBOL";
    fout<<left<<setfill(' ')<<setw(15)<<"|VALUE";
    fout<<left<<setfill(' ')<<setw(30)<<"|CRITICAL SECTION NAME";
    fout<<left<<setfill(' ')<<setw(15)<<"|RELATIVE?";
    fout<<endl;
    fout<<left<<setfill(' ')<<setw(15)<<"---------------";
    fout<<left<<setfill(' ')<<setw(15)<<"---------------";
    fout<<left<<setfill(' ')<<setw(15)<<"---------------";
    fout<<endl;
    for(auto it:symtab)
    {
        fout<<left<<setfill(' ')<<setw(15)<<it.first.second;
        fout<<left<<setfill(' ')<<setw(15)<<("|"+to_string(it.second.second));
        fout<<left<<setfill(' ')<<setw(30)<<("|"+it.first.first);
        fout<<left<<setfill(' ')<<setw(15)<<("|"+to_string(it.second.first));
        fout<<endl;
    }
    fout<<"-----------------------------------------------"<<endl;
    fout<<"The literal table:"<<endl;
    fout<<left<<setfill(' ')<<setw(15)<<"LITERAL";
    fout<<left<<setfill(' ')<<setw(15)<<"|VALUE";
    fout<<left<<setfill(' ')<<setw(15)<<"|CRITICAL SECTION NAME";
    fout<<endl;
    fout<<left<<setfill(' ')<<setw(15)<<"---------------";
    fout<<left<<setfill(' ')<<setw(15)<<"---------------";
    fout<<left<<setfill(' ')<<setw(15)<<"---------------";
    fout<<endl;
    for(auto it:littab)
    {
        fout<<left<<setfill(' ')<<setw(15)<<it.first.second;
        fout<<left<<setfill(' ')<<setw(15)<<("|"+to_string(it.second.second));
        fout<<left<<setfill(' ')<<setw(15)<<("|"+it.first.first);
        fout<<endl;
    }
    fout<<"-----------------------------------------------"<<endl;
    fout.close();
    return;
}
int main()
{
    cout<<"Instructions:"<<endl;
    cout<<"1.The label and opcode must not exceed 10 columns and operand must not exceed 30 columns"<<endl;
    cout<<"2.label from 0 to 9, opcode from 11 to 20 , operand from 22 to end of the line(operand size must not exceed 30)"<<endl;
    cout<<"3.If there is any error encounterd(except external references), the program would stop execution.You can print all errors by commenting return false statements in both passes.I have also printed the instructions for easing error correction."<<endl;
    setOptab();
    string inputFile;
    cout<<"Enter the file name you want to find the object program of:";
    cin>>inputFile;
    Assembler assembler;
    assembler.inputFile=inputFile;
    if(!assembler.pass1())
    {
        cout<<"Pass1 failed because of errors in the inputFile"<<endl;
        return 0;
    }
    else cout<<"Pass1 successful"<<endl;
    //printtables();
    if(!assembler.pass2())
    {
        cout<<"Pass2 failed because of errors in the inputFile"<<endl;
        return 0;
    }else cout<<"Pass2 successful"<<endl;
    printtables();
    return 0;
}