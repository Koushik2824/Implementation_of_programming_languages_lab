#include<bits/stdc++.h>
#include<vector>
using namespace std;
set<string> controlSections;
vector<string> memory (( 1 << 20 ), ".." );//to store the memory bytes,sic/xe has 2^20 bytes
bool is_number(string s)
{
	auto it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}
bool ishexstring(string s)//to check if the string is actually hex
{
    for(int i=0;i<s.length();i++)
    {
        if((s[i]<'0'||s[i]>'9')&&(s[i]<'A'||s[i]>'F')) return false;
    }
    return true;
}
class LinkingLoader
{
    public:
    string programFile;
    int PROGADDR,FIRST,LAST;//to store first memory byte and last memory byte required for the program
    map<string,pair<string,int>> estab;//external symbol table,label,CSECT name,value
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
    bool checkForDuplicateControlSections(string label)
    {
        if(controlSections.find(label)!=controlSections.end()) return true;
        controlSections.insert(label);
        return false;
    }
    int hextoint(string hexs)
    {
        istringstream ss(hexs);
        int val;
        ss>>hex>>val;
        return val;
    }
    vector<pair<string,int>> symbols(string s)//in extdef all defined labels are found
    {
        vector<pair<string,int>> res;
        int length=s.length();
        string symbol;
        int value;
        for(int i = 1; i < length; i += 12)
	    {
		    symbol = removeLeadingTrailingSpaces(s.substr(i, 6));
            value=hextoint(s.substr(i+6,6));
		    res.push_back({symbol,value});
	    }
        return res;
    }
    bool pass1();
    bool pass2();
};

bool LinkingLoader::pass1()
{
    ifstream fin(programFile);
    if(!fin.is_open())
    {
        cout<<"Error opening file"<<endl;
        return false;
    }
    if(istreambuf_iterator<char>(fin) == istreambuf_iterator<char>()) //to check for empty file
    {
        cout<<"Empty file"<<endl;
        return 0;
    }
    int CSADDR=PROGADDR;
    int CSLTH=0;
    int STARTADDR=0;
    bool notset=true;
    string CSECT;
    map<string,bool> checkd,checkr;
    while(!fin.eof())
    {
        string rec;
        getline(fin,rec);
        if(rec.empty()) continue;
        if(rec[0]=='H')//header then store the name of CSECT
        {
            CSADDR+=CSLTH;//old control sections
            CSLTH=hextoint(rec.substr(13,6));
            if(notset) 
            {
                STARTADDR=hextoint(rec.substr(7,6));
                notset=false;
            }
            CSECT=removeLeadingTrailingSpaces(rec.substr(1,6));
            if(checkForDuplicateControlSections(CSECT))
            {
                cout<<"Duplicate Control section found"<<endl;
                return 0;
            }
            if(estab.find(CSECT)==estab.end())
            {
                estab[CSECT]={CSECT,CSADDR};
                //estab["CSLTHOF"+CSECT]={CSECT,CSLTH};
            }
            else
            {
                cout<<"External symbol redefined"<<endl;
                cout<<CSECT<<endl;
                return false;
            }
        } else if(rec[0]=='D')
        {
            if(checkd.find(CSECT)!=checkd.end())
            {
                cout<<"Error duplicate definitions found"<<endl;
            }
            else checkd[CSECT]=true;
            vector<pair<string,int>> syms=symbols(rec);
            for(auto it:syms)//store in the external symbol table
            {
                if(estab.find(it.first)==estab.end())
                {
                    estab[it.first]={CSECT,it.second+CSADDR};
                }
                else
                {
                    cout<<"External symbol redefined"<<endl;
                    cout<<it.first<<endl;
                    return false;
                }
            }
        }else if(rec[0]!='T'&&rec[0]!='R'&&rec[0]!='E'&&rec[0]!='M'&&!rec.empty())
        {
            cout<<"Wrong input object program"<<endl;
            cout<<rec<<endl;
            return false;
            if(rec[0]=='R')
            {
                if(checkr.find(CSECT)!=checkr.end())
                {
                    cout<<"Error duplicate external references found"<<endl;
                }
                else checkr[CSECT]=true;
            }
        }
        // else if(rec[0]=='T')
        // {

        //     int start=hextoint(rec.substr(1,6));
        //     start+=CSADDR;
        //     int temp=0;
        //     for(int i=9;i<rec.length();i+=2)
        //     {
        //         memory[start+temp]=rec.substr(i,2);
        //         temp++;
        //     }
        // }
    }
    LAST = CSADDR + CSLTH + STARTADDR;
    fin.close();
    return true;
}
void printmemory(int first,int last,bool pass1)
{
    ofstream fout;
    if(pass1)
    fout.open("memAfterPass1.txt");
    else
    fout.open("memAfterPass2.txt");
    fout<<"-----------------Memory---------------------"<<endl;
    fout<<left<<setw(15)<<setfill(' ')<<"Memory";
    fout<<left<<setw(15)<<setfill(' ')<<"Contents";
    fout<<endl;
    fout<<right<<hex<<setw(4)<<setfill('0')<<uppercase<<first;
    for(int i=first;i<=last;i+=16)
    {
        fout<<"  ";
        for(int j=i;j<i+16;j+=4)
        {
            fout<<memory[j]<<memory[j+1]<<memory[j+2]<<memory[j+3]<<"  ";
        }
        fout<<endl;
        fout<<right<<hex<<setw(4)<<setfill('0')<<uppercase<<(i+16);
    }
    fout.close();
}
bool LinkingLoader::pass2()
{
    ifstream fin(programFile);
    if(!fin.is_open())
    {
        cout<<"Error opening file"<<endl;
        return false;
    }
    int CSADDR=PROGADDR;
    int CSLTH=0;
    int EXECADDR=PROGADDR;

    string CSECT="";
    while(!fin.eof())
    {
        string rec;
        getline(fin,rec);
        if(rec[0]=='H')
        {
            CSLTH=hextoint(rec.substr(13,6));
            CSECT=removeLeadingTrailingSpaces(rec.substr(1,6));
        }else if(rec[0]=='T')
        {

            int start=hextoint(rec.substr(1,6));
            start+=CSADDR;
            int temp=0;
            for(int i=9;i<rec.length();i+=2)//storing the memory in correct format
            {
                memory[start+temp++]=rec.substr(i,2);
            }
        }else if(rec[0]=='M')
        {
            string extsym = removeLeadingTrailingSpaces(rec.substr(10,6));
            if(estab.find(extsym)!=estab.end())
            {
                int address=hextoint(rec.substr(1,6));
                int size=hextoint(rec.substr(7,2));//this is in half bytes
                address+=CSADDR;
                string val = "";
                char saved;
				for(int i = 0; i < (size + 1)/ 2; i++)
					val += memory[address + i];
                if(size%2)
                {
                    saved=memory[address][0];
                }
                //cout<<"address:"<<address<<":extsym:"<<extsym<<":"<<endl;
                cout<<"---------------------------------------------------"<<endl;
                cout<<"Before modification:"<<val<<endl;
                int value=hextoint(val);
                char sign=rec[9];
                if(sign=='+') value += estab[extsym].second;//modification
                else value -= estab[extsym].second;
                cout<<"The value being modified by:"<<uppercase<<hex<<estab[extsym].second<<endl;
                //for negative values
                int mask = 0;
				for(int i = 0; i < 24; i++)
				{
					mask |= 1;
					mask = mask << 1;
				}
				mask = mask >> 1;
				value = value & mask;
                stringstream ss;
                ss<<right<<setw(size+(size%2))<<setfill('0') <<hex << uppercase <<value;
                cout<<"The modified value at the given address:"<<ss.str()<<endl;
                for(int i = 0; i < size; i += 2)//storing back into memory
					memory[address + i / 2] = ss.str().substr(i, 2);
                if(size%2) memory[address][0]=saved;
            }
            else
            {
                cout<<"Undefined external symbol"<<endl;
                cout<<extsym<<":"<<endl;
                cout<<estab[extsym].second<<endl;
                return false;
            }
        }else if(rec[0]=='E')
        {
            FIRST=PROGADDR;
            if(rec.length()>1)
            {
                stringstream ss(rec.substr(1,6));
                ss>>hex>>FIRST;
            }
            CSADDR+=CSLTH;
        }else if(rec[0]!='R'&&rec[0]!='D'&&!rec.empty())
        {
            cout<<"Input is wrong"<<endl;
            cout<<rec<<endl;
            return false;
        }
    }
    fin.close();
    return true;
}
int main()
{
    LinkingLoader linkingloader;
    string programaddress;
    cout<<"Enter the objectFile name:";
    cin>>linkingloader.programFile;
    cout<<"Enter the program address where the program should be loaded to(in hex):";
    cin>>programaddress;
    if(!ishexstring(programaddress))
    {
        cout<<"You didnt enter proper address in hex format."<<endl;
        return 0;
    }
    linkingloader.PROGADDR=linkingloader.hextoint(programaddress);
    if(linkingloader.PROGADDR>=(1<<20))
    {
        cout<<"The program address you entered is very large, hence cant fit in memory."<<endl;
        return 0;
    }
    if(!linkingloader.pass1())
    {
        cout<<"Pass1 failed due to errors"<<endl;
        return 0;
    }
    cout<<"The External symbol table:"<<endl;
    cout<<left<<setfill(' ')<<setw(15)<<"SYMBOL";
    cout<<left<<setfill(' ')<<setw(15)<<"|VALUE";
    cout<<left<<setfill(' ')<<setw(15)<<"|CRITICAL SECTION NAME";
    cout<<endl;
    cout<<left<<setfill(' ')<<setw(15)<<"---------------";
    cout<<left<<setfill(' ')<<setw(15)<<"---------------";
    cout<<left<<setfill(' ')<<setw(15)<<"---------------";
    cout<<endl;
    for(auto it:linkingloader.estab)
    {
        cout<<left<<setfill(' ')<<setw(15)<<(it.first+":");
        cout<<left<<setfill(' ')<<setw(15)<<("|"+to_string(it.second.second));
        cout<<left<<setfill(' ')<<setw(15)<<("|"+it.second.first);
        cout<<endl;
    }
    if(linkingloader.LAST>=(1<<20))
    {
        cout<<"The program address is very large that file won't fit in memory or program itself is very large."<<endl;
        return 0;
    }
    //printmemory(linkingloader.PROGADDR,linkingloader.LAST,1);
    if(!linkingloader.pass2())
    {
        cout<<"Pass2 failed due to errors"<<endl;
        return 0;
    }
    cout<<"-------------------------------------------------"<<endl;
    cout<<"The execution should start at:"<<hex<<uppercase<<linkingloader.FIRST<<endl;
    cout<<"-------------------------------------------------"<<endl;
    printmemory(linkingloader.PROGADDR,linkingloader.LAST,0);

    return 0;
}