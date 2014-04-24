/**
Author   : Deepansh Singh
Handle   : nyble
**/

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <map>
#include <ios>
#include <bitset>

using namespace std;

string code[500][5];                                             // Contains the Parsed Code
//   code[][0]  code[][1]  code[][2]
//   LABEL      OPCODE     OPERAND

long codeinfo[500][3];                                          // Contains the Code Information
// codeinfo[][0]  codeinfo[][1]    codeinfo[][2]
// LOCCTR         OPCODE FORMAT    OPCODE VALUE

map<string,long> SYMTAB;                                        // Symbol Table implemented as a map to give O(logn) search complexity.

string reg = "AXLBSTF";                                         // Contains the Register values

struct op_tab
{
    string name;
    long value;
    long format;

};

op_tab optab[] = {"ADD",24,3,                                    // OPCODE TABLE with Decimal Values and Instruction Format
                  "ADDF",88,3,
                  "ADDR",144,2,
                  "AND",64,3,
                  "CLEAR",180,2,
                  "COMP",40,3,
                  "COMPF",136,3,
                  "COMPR",160,2,
                  "DIV",36,3,
                  "DIVF",100,3,
                  "DIVR",156,2,
                  "FIX",196,1,
                  "FLOAT",192,1,
                  "HIO",244,1,
                  "J",60,3,
                  "JEQ",48,3,
                  "JGT",52,3,
                  "JLT",56,3,
                  "JSUB",72,3,
                  "LDA",0,3,
                  "LDB",104,3,
                  "LDCH",80,3,
                  "LDF",112,3,
                  "LDL",8,3,
                  "LDS",108,3,
                  "LDT",116,3,
                  "LDX",4,3,
                  "LPS",208,3,
                  "MUL",32,3,
                  "MULF",96,3,
                  "MULR",152,2,
                  "NORM",200,1,
                  "OR",68,3,
                  "RD",216,3,
                  "RMO",172,2,
                  "RSUB",76,3,
                  "SHIFTL",164,2,
                  "SHIFTR",168,2,
                  "SIO",240,1,
                  "SSK",236,3,
                  "STA",12,3,
                  "STB",120,3,
                  "STCH",84,3,
                  "STF",128,3,
                  "STI",212,3,
                  "STL",20,3,
                  "STS",124,3,
                  "STSW",232,3,
                  "STT",132,3,
                  "STX",16,3,
                  "SUB",28,3,
                  "SUBF",92,3,
                  "SUBR",148,2,
                  "SVC",176,2,
                  "TD",224,3,
                  "TIO",248,1,
                  "TIX",44,3,
                  "TIXR",184,2,
                  "WD",220,3
                 };

long LOCCTR = 0, rows = 0;                                        // Location Counter and Lines of code.

long startAddress=0,progLength=0;                                 // Program Length and Starting Address.

int op_search(string opcode, int low , int high)                 // Binary Search to Find OPCODE
{
    //int low=0 , high=58;

    int mid= (low+high)/2;
    int res =strcmp(opcode.c_str(),optab[mid].name.c_str());

    if(low==high-1)
    {
        if(!strcmp(opcode.c_str(),optab[high].name.c_str()))
            return high;
        if(!strcmp(opcode.c_str(),optab[low].name.c_str()))
            return low;
        return -1;
    }

    if(!res)
    {
        return mid;
    }

    else if(res<0)
    {
        return op_search(opcode,low,mid);
    }

    else
    {
        return op_search(opcode,mid,high);
    }
}

string decToHex (long decimal)
{
    long arr[6]= {1,16,256,4096,65536,1048576};

    string hexdec;
    char str[17] = "0123456789ABCDEF";

    if(decimal>16777215)                            // Format 4 instruction
    {
        hexdec.append(1u,str[decimal/268435456]);
        decimal=decimal%268435456;

        hexdec.append(1u,str[decimal/16777216]);
        decimal=decimal%16777216;
    }

    for(int i=5; i>=0; i--)
    {
        hexdec.append(1u,str[decimal/arr[i]]);
        decimal=decimal%arr[i];
    }

    return hexdec;
}

string code1[500][5];

int parser(string source)
{
    ifstream fs;

    char c[100];
    int i = -1 , col = 0;
    c[0]='\0';

    fs.open(source,ifstream::in);                          // Input Source Program

    while (!fs.eof())
    {
        do
        {
            i++;
            c[i] = fs.get();

            while(c[i]=='.')                                     // Remove Comments
            {
                while(c[i]!='\n')c[i] = fs.get();
                c[i] = fs.get();
            }

            if(c[i]=='\'')                                       // Read String Constants
            {
                do
                {
                    i++;
                    c[i] = fs.get();
                }
                while(c[i]!='\'');
            }

        }
        while(c[i]!=' '&&c[i]!='\t'&&c[i]!='\n'&&!fs.eof());

        code[rows][col].assign(c,0,i);
        code1[rows][col].assign(c,0,i);

        while(c[i]==' '||c[i]=='\t')c[i] = fs.get();             // Remove Extra Whitespaces

        if(fs.eof())break;

        else if(c[i]=='\n')
        {
            rows++;
            col=0;
            c[0]='\0';
            i=-1;
        }
        else
        {
            col++;
            c[0]=c[i];
            i=0;
        }
    }

    fs.close();

    return rows;
}

int pass1(string source)
{
    parser(source);

    if(!strcmp(code[0][1].c_str(),"START"))
    {
        LOCCTR =  strtol(code[0][2].c_str(),nullptr,16);         //Initialize LOCCTR and Starting Address
        startAddress = LOCCTR;
    }

    codeinfo[0][0] = LOCCTR;                                     //Initialize the first instruction with LOCCTR
    codeinfo[1][0] = LOCCTR;
    code1[0][3] = decToHex(LOCCTR).substr(2,4);
    code1[1][3] = code1[0][3];

    for(int i=1; i<rows; i++)
    {

        if(!code[i][0].empty())                                  // If LABEL is found
        {
            if(SYMTAB[code[i][0]]==0)                            // If LABEL not Duplicate
                SYMTAB[code[i][0]]=LOCCTR;

            else
            {
                cout<<"ERROR :\nDUPLICATE SYMBOL FOUND : ( "<<code[i][0]<<" )\nASSEMBLY ABORTED !\n";
                exit(1);
                break;
            }

        }

        if(code[i][1][0]=='+')                                   // Format 4 instruction
        {
            code[i][1].erase(code[i][1].begin());
            codeinfo[i][1] = 4;
        }

        int index = op_search(code[i][1],0,58);

        if(index != -1)
        {
            if(codeinfo[i][1]==0)codeinfo[i][1]=optab[index].format;

            LOCCTR+= codeinfo[i][1];
        }

        else if(!code[i][1].compare("WORD"))
            LOCCTR+=3;

        else if(!strcmp(code[i][1].c_str(),"RESW"))
        {
            LOCCTR+=3*strtol(code[i][2].c_str(),nullptr,10);
        }

        else if(!strcmp(code[i][1].c_str(),"RESB"))
        {
            LOCCTR+=strtol(code[i][2].c_str(),nullptr,10);
        }

        else if(!strcmp(code[i][1].c_str(),"BYTE"))
        {

            if(code[i][2][0]=='C')
            {
                LOCCTR+=code[i][2].length()-3;
            }

            else
            {
                LOCCTR+=(code[i][2].length()-3)/2;
            }

        }

        else if(!code[i][1].compare("BASE")||!code[i][1].compare("NOBASE")) {}

        else if(!strcmp(code[i][1].c_str(),"END"))
        {
            codeinfo[i][0] = LOCCTR;
            codeinfo[i][2] = -1;
            rows=i+1;                                            //Ignore further lines
            break;
        }

        else
        {
            cout<<"ERROR :\nOPCODE : ( "<<code[i][1]<<" ) NOT FOUND !\nASSEMBLY ABORTED !\n";
            exit(1);
            break;
        }

        codeinfo[i][2] = ( index==-1 )? -1 : optab[index].value ;

        codeinfo[i+1][0] = LOCCTR;                                      // Store Location Counter

        code1[i+1][3] = decToHex(LOCCTR).substr(2,4);
    }

    progLength = LOCCTR - startAddress;                          // Program Length

    return 1;

}

int baseOrPc ( int ta, int regB ,int pc , bool canUseBaseAsReg )
{
    int disp=0;
    disp = ta - pc ;
    if( disp>=(-2048) && disp<=2047 )
    {
        return 1;  // PC Relative
    }
    else if(!canUseBaseAsReg && (disp = ta - regB)>=0 && disp<=4095)
    {
        return 0;  // Base Relative
    }
    else
    {
        cout<<ta<<" "<<pc<<" "<<regB<<endl;
        cout<<disp<<endl;
        cout<<"ERROR :\n OPERAND TOO LARGE AND EXTENDED FORMAT NOT SPECIFIED !! "<<endl;
        exit(1);
        return -1;
    }
}

int pass2()
{
    ofstream fs;
    fs.open("objectCode.o",ios_base::out);

    string textRecord[100],header = "H^",endRecord = "E^";
    string record = "      ";
    int t=0,recordLength=0, regB = 0;
    bool canUseBaseAsReg = true;

    if(!strcmp(code[0][1].c_str(),"START"))
    {
        record.replace(record.begin(),record.begin()+code[0][0].length(),code[0][0]);
        header.append(record);
        header.append("^");
        header.append(decToHex(codeinfo[0][0]));
        header.append("^");
        header.append(decToHex(progLength));
        fs<<header<<endl;
    }


    textRecord[t]="T^";                                       // Initialize Text Record
    textRecord[t].append(decToHex(codeinfo[1][0]));

    for(int i=1; i<=rows; i++)
    {
        string obcodeStr;
        bool newRecord = false, newLine = false;

        if(codeinfo[i][2]!=-1)
        {
            int n=1, im=1, x=0, b=0, p=0, e=0;                      // Initialize Flags
            bitset<8> byte1 = codeinfo[i][2];
            bitset<16> byte2 ;
            bitset<24> byte3 ;

            if(codeinfo[i][1]==1)                                   // Format 1 Instruction
            {
                obcodeStr = decToHex(codeinfo[i][2]).substr(4,2);

                code1[i][4] = obcodeStr;

                if((recordLength+2)<=60)                        // Write Current Record
                {
                    textRecord[t].append("^");
                    textRecord[t].append(obcodeStr);
                    recordLength+=2;
                }
                else
                    newRecord=true;

            }
            else if(codeinfo[i][1]==2)                              // Format 2 Instruction
            {
                char R1='\0',R2='\0';
                sscanf(code[i][2].c_str(),"%c,%c",&R1,&R2);

                if(R2=='\0')
                {
                    obcodeStr = decToHex(codeinfo[i][2]).substr(4,2);
                    if(reg.find(R1)!=string::npos)
                    {
                        if(reg[reg.find(R1)]=='B' && !canUseBaseAsReg)
                        {
                            cout<<"ERROR : Base Register Cannot Be Used Right Now !!"<<endl;
                            exit(1);
                        }
                        obcodeStr.append(1u,48+reg.find(R1));
                    }
                    else
                        obcodeStr.append(1u,48+R1);
                    obcodeStr.append(1u,'0');
                }
                else
                {
                    obcodeStr = decToHex(codeinfo[i][2]).substr(4,2);
                    if(reg[reg.find(R1)]=='B' && !canUseBaseAsReg)
                    {
                        cout<<"ERROR : Base Register Cannot Be Used Right Now !!"<<endl;
                        exit(1);
                    }
                    obcodeStr.append(1u,48+reg.find(R1));
                    if(reg.find(R2)!=string::npos)
                    {
                        if(reg[reg.find(R2)]=='B' && !canUseBaseAsReg)
                        {
                            cout<<"ERROR : Base Register Cannot Be Used Right Now !!"<<endl;
                            exit(1);
                        }
                        obcodeStr.append(1u,48+reg.find(R2));
                    }

                    else
                        obcodeStr.append(1u,48+R2-1);
                }

                code1[i][4] = obcodeStr;

                if((recordLength+4)<=60)                        // Write Current Record
                {
                    textRecord[t].append("^");
                    textRecord[t].append(obcodeStr);
                    recordLength+=4;
                }
                else
                    newRecord=true;
            }
            else if(codeinfo[i][1]==3)                              // Format 3 Instruction
            {
                int ta = 0, pc = codeinfo[i+1][0];

                if(code[i][2][0]=='#')                              // Immediate Addressing
                {
                    n = 0;
                    code[i][2].erase(code[i][2].begin());

                    if(isalpha(code[i][2][0]))                      // If operand is Label
                    {
                        ta = SYMTAB[code[i][2]] ;
                        if( p = baseOrPc( ta , regB , pc ,canUseBaseAsReg))     // Base Or PC Relative
                            byte2 = ta - pc;
                        else
                        {
                            b=1;
                            byte2 = ta - regB;
                        }
                    }
                    else                                             // If operand is Numeric
                    {
                        byte2 = strtol(code[i][2].c_str(),nullptr,10);
                    }
                }

                else if(code[i][2][0]=='@')                         // Indirect Addressing
                {
                    im=0;
                    code[i][2].erase(code[i][2].begin());
                    ta = SYMTAB[code[i][2]] ;
                    if( p = baseOrPc( ta , regB , pc ,canUseBaseAsReg))     // Base Or PC Relative
                        byte2 = ta - pc;
                    else
                    {
                        b=1;
                        byte2 = ta - regB;
                    }

                }

                else if(isalpha(code[i][2][0]))                           //Simple Addressing
                {
                    if(code[i][2].find(",X") != string::npos)            // Indexed Addressing
                    {
                        code[i][2].erase(code[i][2].end()-2,code[i][2].end());
                        x = 1;
                    }
                    ta = SYMTAB[code[i][2]] ;
                    if( p = baseOrPc( ta , regB , pc ,canUseBaseAsReg))     // Base Or PC Relative
                        byte2 = ta - pc;
                    else
                    {
                        b=1;
                        byte2 = ta - regB;
                    }
                }

                byte1.set(1,n);                                 //Set Flags
                byte1.set(0,i);
                byte2.set(15,x);
                byte2.set(14,b);
                byte2.set(13,p);
                byte2.set(12,e);
                obcodeStr.append(decToHex(byte1.to_ulong()).substr(4,2));
                obcodeStr.append(decToHex(byte2.to_ulong()).substr(2,4));

                code1[i][4] = obcodeStr;

                if((recordLength+6)<=60)                        // Write Current Record
                {
                    textRecord[t].append("^");
                    textRecord[t].append(obcodeStr);
                    recordLength+=6;
                }
                else
                    newRecord=true;

            }
            else                                                // Format 4 instruction;
            {
                e = 1;
                if(code[i][2][0]=='#')                              // Immediate Addressing
                {
                    n = 0;
                    code[i][2].erase(code[i][2].begin());

                    if(isalpha(code[i][2][0]))                      // If operand is Label
                    {
                        byte3 = SYMTAB[code[i][2]] ;
                    }
                    else                                             // If operand is Numeric
                    {
                        byte3 = strtol(code[i][2].c_str(),nullptr,10);
                    }
                }
                else if(code[i][2][0]=='@')                         // Indirect Addressing
                {
                    im=0;
                    code[i][2].erase(code[i][2].begin());
                    byte3 = SYMTAB[code[i][2]] ;
                }
                else                                                    //Simple Addressing
                {
                    if(code[i][2].find('X') != string::npos)            // Indexed Addressing
                    {
                        code[i][2].erase(code[i][2].end()-2,code[i][2].end());
                        x = 1;
                    }
                    byte3 = SYMTAB[code[i][2]] ;
                }

                byte1.set(1,n);                                 //Set Flags
                byte1.set(0,i);
                byte3.set(23,x);
                byte3.set(22,b);
                byte3.set(21,p);
                byte3.set(20,e);

                obcodeStr.append(decToHex(byte1.to_ulong()).substr(4,2));
                obcodeStr.append(decToHex(byte3.to_ulong()));

                code1[i][4] = obcodeStr;

                if((recordLength+8)<=60)                        // Write Current Record
                {
                    textRecord[t].append("^");
                    textRecord[t].append(obcodeStr);
                    recordLength+=8;
                }
                else
                    newRecord=true;

            }

        }
        else if(!code[i][1].compare("BASE"))
        {
            canUseBaseAsReg = false;
            regB = SYMTAB[ code[i][2] ];
        }
        else if(!code[i][1].compare("NOBASE"))
        {
            canUseBaseAsReg = true;
        }
        else if(!code[i][1].compare("WORD"))
        {
            obcodeStr = decToHex(strtol(code[i][2].c_str(),nullptr,10));
            code1[i][4] = obcodeStr;

            if((recordLength+6)<=60)                        // Write Current Record
            {
                textRecord[t].append("^");
                textRecord[t].append(obcodeStr);
                recordLength+=6;
            }
            else
                newRecord=true;

        }
        else if(!strcmp(code[i][1].c_str(),"BYTE"))
        {
            if(code[i][2][0]=='C')                          // If String or Character Constant
            {
                code[i][2].erase(0,2);
                code[i][2].erase(code[i][2].end()-1);
                for(int j = 0; j<code[i][2].length(); j++)
                {
                    obcodeStr.append(decToHex(code[i][2][j]).substr(4,2));
                }
            }
            else                                            // If Hexadecimal Constant
            {
                code[i][2].erase(0,2);
                code[i][2].erase(code[i][2].end()-1);
                obcodeStr = code[i][2] ;
            }

            code1[i][4] = obcodeStr;

            if((recordLength+obcodeStr.length())<=60)                        // Write Current Record
            {
                textRecord[t].append("^");
                textRecord[t].append(obcodeStr);
                recordLength+=obcodeStr.length();
            }
            else
                newRecord=true;

        }
        else if(!strcmp(code[i][1].c_str(),"RESW")||!strcmp(code[i][1].c_str(),"RESB"))
        {
            if(codeinfo[i+1][0]-codeinfo[i][0]>=4096)newLine=true;
        }
        else if(!strcmp(code[i][1].c_str(),"END"))
        {
            textRecord[t].insert(8,"^");
            textRecord[t].insert(9,decToHex(recordLength/2).substr(4,2));    // Insert Last Text Record's Total Length
            fs<<textRecord[t]<<endl;                                        // Write Last Text Record

            endRecord.append( decToHex( SYMTAB[ code[i][2] ] ) );                     // Write End Record
            fs<<endRecord<<endl;

            break;
        }

        if(recordLength==60 || newRecord || newLine)
        {
            textRecord[t].insert(8,"^");
            textRecord[t].insert(9,decToHex(recordLength/2).substr(4,2));    // Insert Current Text Record's Total Length
            fs<<textRecord[t]<<endl;                                        // Write Current Text Record
            recordLength = 0;
            t++;
            textRecord[t]="T^";                                             // Initialize New Text Record
            textRecord[t].append(decToHex(codeinfo[i+1][0]));
        }

        if(newRecord)
        {
            textRecord[t].replace(2,7,decToHex(codeinfo[i][0]));            // Replace with last Record's Starting address
            textRecord[t].append("^");
            textRecord[t].append(obcodeStr);                                // if previous text record was full before inserting last record.
            recordLength+=obcodeStr.length();
        }
        //cout<<textRecord[t]<<endl;
    }

    fs.close();

    cout<<"\n\nLOCCTR\t\tLABEL\tOPCODE\tOPERAND\t\tOBJECT CODE\n";                                  // Print Source Code
    cout<<"----------------------------------------------------------------"<<endl;
    for(int r=0; r<rows; r++)
    {
        cout<<code1[r][3]<<"\t\t"<<code1[r][0]<<"\t"<<code1[r][1]<<"\t"<<code1[r][2]<<"\t\t"<<code1[r][4]<<endl;
    }

    map<string,long> :: iterator it;                                                            // Print Symbol Table
    cout<<"\n\n\n    SYMBOL TABLE"<<endl;
    cout<<"---------------------------"<<endl;
    cout<<"LABEL\t\tVALUE"<<endl;
    cout<<"---------------------------"<<endl;
    for(it = SYMTAB.begin(); it!=SYMTAB.end(); it++)
    {
        cout<<it->first<<"\t:\t"<<decToHex(it->second)<<endl;
    }
    cout<<"\n\nOBJECT CODE\n"<<endl;                                                            // Print Object Code
    cout<<"----------------------------------------------------------------"<<endl;
    cout<<header<<endl;
    for(int r=0; r<=t; r++)cout<<textRecord[r]<<endl;
    cout<<endRecord<<endl;

    return 1;
}

int main(int argc, char* argv[])
{
    string source;
    if(argc==1)
    {
        cout<<"Enter Source Filename with Path : "<<endl;
        cin>>source;
        if(source[0]=='1')source.assign("source.sicxe");
    }
    else
    {
        source.assign(argv[1]);
    }
    freopen ("assemblerOutput.txt","w",stdout);                     // Redirect all output to file
    pass1(source);
    pass2();
    return 0;
}
