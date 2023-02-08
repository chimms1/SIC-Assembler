// Contributed by - Yash Deshpande and Shreyas Joshi

// Some comments may be wrong and there are simpler methods to implement many functionalities.

/*

Copy Start 2000
First STL Retadr
    LDB #length
    BASE length
    +JSUB RDREC
    LDA length
    COMP #0
    JEQ Endfil
    +JSUB WRREC
    TIXR X
    J CLOOP
Endfil LDA EOF
    STA Buffer
    LDA #3
    STA length
    +JSUB WRREC
    J @Retadr
    End First

LDA 74 3
STL 50 3
LDB 54 3
JSUB 2C 3
COMP 38 3
JEQ AB 3
J 2B 3
STA 1D 3
TIXR 20 2

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXOP 100   /* Size of strings and string arrays */

static int start_f = 0; // start_flag is set to 1 if Start instruction is present in input program, else 0

// Structure to hold components of instruction
struct instruction
{
    char *label;
    char *opcode;
    char *operand;
};

// Structure to hold components of optab entry
struct optabcode
{
    char *opcode;
    int objectcode;
    int size;
};

// Function to display an instruction
void displayInstruction(struct instruction inst);

// Function to display an optab entry
void displayOPTABLE(struct optabcode oplist[],int n);

// Function to tokenize instruction from a line (string) and return structure of instruction
struct instruction tokenizeInstruction(char line[]);

// Function to tokenize optable entry from a line (string) and return structure of optab
struct optabcode tokenizeOpcode(char line[]);

// Function returns number of entries in OPTABLE after reading from file
// and storing opcode and objectcode in arrays
int getOPTAB(FILE *fp4,char opcodearray[][MAXOP],int codearray[],int sizearray[]);

// Returning Address as Hexadecimal integer even though in struct it is a string
void getAddress(struct instruction inst,int *address);

// Updates address Based on Pass 1 Algorithm of SIC
void updateAddress(int *address,struct instruction temp_inst,char opcodearray[MAXOP][MAXOP],int codearray[],int sizearray[]);

// Search symbol in symbol table, if not found then add it in the end with its address
void addSYMTAB(int *address,FILE *fp3, struct instruction inst);

// This function is called before ProcessLines
// Reads first line expecting "Start" opcode. Address is updated in LOCCTR.
// linecount is returned.
int ProcessFirstLine(FILE *fp1,FILE *fp2,FILE *fp3,struct instruction temp_inst,int *address,char opcodearray[][MAXOP],int codearray[],int sizearray[]);

// Function to processes every line after first line till EOF. Tokenize every instruction,
// checks and adds symbols in SYMTAB and updates address
int ProcessLines(FILE *fp1,FILE *fp2,FILE *fp3,struct instruction temp_inst,int *address,char opcodearray[][MAXOP],int codearray[],int sizearray[]);



int main()
{
    FILE *fp1;  // Read file pointer
    FILE *fp2;  // Write file pointer
    FILE *fp3;  // SYMTAB file pointer
    FILE *fp4;  // OPTAB file pointer
    FILE *fp5;  // File to store program length

    fp1=fopen("input.txt","r");
    fp2=fopen("intermediate-file.txt","w");
    fp3=fopen("SYMTAB.txt","w+");
    fp4=fopen("OPTAB.txt","r");
    fp5=fopen("temp.txt","w");


    // File opening Exceptions
    if(fp1==NULL){
        printf("  => Error opening the read file\n\n");
        return 1;
    }
    if(fp2==NULL){
        printf("  => Error opening the write file\n\n");
        return 2;
    }
    if(fp3==NULL){
        printf("  => Error opening the Symbol Table file\n\n");
        return 3;
    }
    if(fp4==NULL){
        printf("  => Error opening the Opcode Table file\n\n");
        return 4;
    }
    if(fp5==NULL){
        printf("  => Error opening the Temp file\n\n");
        return 5;
    }

    printf("  -Files opened- \n\n");

    int optable_size;   // Size of OPTAB
    char opcodearray[MAXOP][MAXOP]; // Array of strings to hold operation names
    int codearray[MAXOP];   // Array of int to hold corresponding object codes
    int sizearray[MAXOP];   // Array of int to hold size of instructions

    optable_size = getOPTAB(fp4,opcodearray,codearray,sizearray);

    printf("\n\noptable_size = %d\n\n",optable_size);

    printf("Contents of Read file is: \n");
    printf("--------------------------\n\n");

    struct instruction temp_inst;
    int starting_address;
    int address;
    int program_length;
    int linecount;

    // Read First Line to check for Start address
    linecount = ProcessFirstLine(fp1,fp2,fp3,temp_inst,&address,opcodearray,codearray,sizearray);
    starting_address = address;

    if(start_f==0)
    {
        starting_address=0;
    }


    // Read Remaining Lines
    linecount += ProcessLines(fp1,fp2,fp3,temp_inst,&address,opcodearray,codearray,sizearray);

    program_length = address-starting_address;

    printf("\nStarting Address = Hex: %x, Decimal: %d\n",starting_address,starting_address);
    printf("Address = Hex: %x, Decimal: %d\n",address,address);
    printf("Program Length = Hex: %x, Decimal: %d\n",program_length,program_length);
    fprintf(fp5,"%x",program_length);
    printf("\n\n--------------------------\n");
    printf("\nNumber of lines is %d\n",linecount);
    printf("\n\n==> Read and Write Complete \n");

    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);


    printf("\n  -Files closed- \n\n");

    return 0; // Execution Completed Successfully
}



// Function to display an instruction
void displayInstruction(struct instruction inst)
{
    printf("%s %s %s\n",inst.label,inst.opcode,inst.operand);
}

// Function to display an instruction
void displayOPTABLE(struct optabcode oplist[],int n)
{
    for(int i=0;i<n;i++)
    {
        //fflush(stdout);
        printf("%s %X\n",oplist[i].opcode,oplist[i].objectcode);
    }
}


// Function to tokenize instruction from a line (string) and return structure of instruction
struct instruction tokenizeInstruction(char line[])
{
    int count=0;
    struct instruction inst;
    char *token1=NULL;
    char *token2=NULL;
    char *token3=NULL;

    token1=strtok(line," ");
    token2=strtok(NULL," ");
    token3=strtok(NULL,"");

    if(token2==NULL)
        count=1;

    if(token3==NULL && token2!=NULL)
        count=2;

    if(token3!=NULL)
        count=3;

    if(count==1)
    {
        inst.label="";
        inst.opcode=token1;
        inst.operand="";
    }

    if(count==2)
    {
        inst.label="";
        inst.opcode=token1;
        inst.operand=token2;
    }

    if(count==3)
    {
        inst.label=token1;
        inst.opcode=token2;
        inst.operand=token3;
    }
    return inst;
}

// Function to tokenize optable entry from a line (string) and return structure of optab
struct optabcode tokenizeOpcode(char line[])
{
    struct optabcode temp;  // Temporary OPTAB variable

    char *token1=NULL;
    char *token2=NULL;
    char *token3=NULL;

    temp.opcode=strtok(line," ");
    token2=strtok(NULL," ");
    token3=strtok(NULL," ");


    //sscanf(token2,"%X",&temp.objectcode);
    temp.objectcode = (int)strtol(token2,NULL,16);  // Converting operand to hex
    temp.size = (int)strtol(token3,NULL,10);

    return temp;
}

// Function returns number of entries in OPTABLE after reading from file
// and storing opcode and objectcode in arrays
int getOPTAB(FILE *fp4,char opcodearray[][MAXOP],int codearray[],int sizearray[])
{
    char line[30];  // Temporary string to hold a line/instruction
    struct optabcode temp_optab;
    int j=0;

    while(!feof(fp4))
    {
        // Read a character
        fgets(line,30,fp4);


        if(feof(fp4))
        {
            printf("OPTAB file reading finished\n");
            break;
        }


        if(line[0]=='\n')
        {
            printf("Here lies a backslash n  \n");
            continue;
        }

        if(line[strlen(line)-1]=='\n')
            line[strlen(line)-1]='\0';    // fgets() reads '\n' in the string

        temp_optab = tokenizeOpcode(line);

        // Opcode name
        strcpy(opcodearray[j],temp_optab.opcode);
        // Opcode hex code
        codearray[j] = temp_optab.objectcode;
        // Opcode size
        sizearray[j] = temp_optab.size;

        printf("%s %X\n",opcodearray[j],codearray[j]);

        j++;
    }

    // returns number of entries in OPTABLE
    return j;
}




// Returning Address as Hexadecimal integer even though in struct it is a string
void getAddress(struct instruction inst,int *address)
{
    static int read=0;  // read flag is initially 0, after reading address it is set to 1

    if(!strcmp(inst.opcode,"Start")||!strcmp(inst.opcode,"start"))
    {
        if(inst.operand==NULL){
            *address=0;
            read=1;
        }

        if(inst.operand!=NULL)
        {

            //sscanf(inst.operand,"%X",address);
            *address = (int)strtol(inst.operand,NULL,16);  // Converting operand to hex

            read=1;
            start_f=1;
        }
    }
    if(read==0)
    {
        *address=0;
        read=1;
    }
}

// Updates address Based on Pass 1 Algorithm of SIC
void updateAddress(int *address,struct instruction temp_inst,char opcodearray[MAXOP][MAXOP],int codearray[],int sizearray[])
{
    int hex_operand;
    if(!strcmp(temp_inst.opcode,"WORD")||!strcmp(temp_inst.opcode,"Word")||!strcmp(temp_inst.opcode,"word"))
    {
        *address = *address+3;

        return;
    }

    else if(!strcmp(temp_inst.opcode,"RESW")||!strcmp(temp_inst.opcode,"Resw")||!strcmp(temp_inst.opcode,"resw"))
    {
        //sscanf(temp_inst.operand,"%X",&hex_operand);
        hex_operand = (int)strtol(temp_inst.operand,NULL,10);  // Converting operand to hex
        *address = *address+ (3*hex_operand);

        return;
    }

    else if(!strcmp(temp_inst.opcode,"RESB")||!strcmp(temp_inst.opcode,"Resb")||!strcmp(temp_inst.opcode,"resb"))
    {
        //sscanf(temp_inst.operand,"%X",&hex_operand);
        hex_operand = (int)strtol(temp_inst.operand,NULL,10);  // Converting operand to hex
        *address = *address + (1*hex_operand);

        return;
    }

    else if(!strcmp(temp_inst.opcode,"BYTE")||!strcmp(temp_inst.opcode,"Byte")||!strcmp(temp_inst.opcode,"byte"))
    {
        int count=0;
        int i=2;    // characters start at 2nd index inside C'sample' or X'1B0FK3'

        while(temp_inst.operand[i]!=39)
        {
            //printf("\n%c",temp_inst.operand[i]);
            count++;
            i++;
        }

        hex_operand = count;

        if(temp_inst.operand[0]=='X'||temp_inst.operand[0]=='x')
        {
            *address = *address + (0.5*hex_operand);
        }

        else if(temp_inst.operand[0]=='C'||temp_inst.operand[0]=='c')
        {
            *address = *address + (1*hex_operand);
        }

        return;
    }


    // Else search opcode in OPTAB
    int opcode_found = 0;   // set to 1 if opcode found in OPTAB, else 0

    // If opcode has a '+' , remove it to search opcode in OPTAB
    int plus_flag = 0;
    char opcode_noplus[MAXOP];
    opcode_noplus[0]='\0';

    if(temp_inst.opcode[0]=='+')
    {
        plus_flag=1;
        int j=1;
        for(j=1;j<sizeof(temp_inst.opcode);j++)
        {
            //strcat(opcode_noplus,temp_inst.opcode[j]);
            opcode_noplus[j-1] = temp_inst.opcode[j];
        }
        opcode_noplus[++j]='\0';
    }

    int i=0;

    // OPTAB Search
    if(plus_flag==1)
    {
        for(i=0;i<MAXOP;i++)
        {
            if(!strcmp(opcodearray[i],opcode_noplus))
            {
                opcode_found = 1;
                break;
            }
        }
    }
    else if(plus_flag==0)
    {
        for(i=0;i<MAXOP;i++)
        {
            if(!strcmp(opcodearray[i],temp_inst.opcode))
            {
                opcode_found = 1;
                break;
            }
        }
    }




    if(opcode_found == 1)
    {
        if(plus_flag==1)
            *address = *address + 0x04;
        else
            *address = *address + sizearray[i];

        return;
    }

    else
    {
        printf("\nERROR: opcode %s not found in OPTABLE\n\n",temp_inst.opcode);
        //exit(6);
    }


}

// Search symbol in symbol table, if not found then add it in the end with its address
void addSYMTAB(int *address,FILE *fp3, struct instruction inst)
{
    // Check if label already exists

    rewind(fp3);  // Take file pointer to Start of File

    char temp;  // Temporary character variable
    char line[30];  // Temporary string to hold a line/instruction
    int repeat_flag = 0;
    int i=0;

    while(!feof(fp3))
    {
        // Read a character
        fscanf(fp3,"%c",&temp);

        // Put it in a character array
        line[i]=temp;

        // Increment string index
        i++;

        if(temp==' ')
        {
            line[--i]='\0'; // Adding null terminator
            i=0;

            // Check if label names are same !strcmp(*temp_inst.opcode,*inst.label)
            if(!strcmp(inst.label,line))
            {
                printf("\nERROR: Label %s is repeated\n\n",inst.label);
                repeat_flag=1;
                //exit(5);
            }
        }

        if(temp=='\n')
        {
            i=0;
        }
    }

    // If code gets executed till here then
    // 1. The label we want to insert is a new one and does not exist in the SYMTAB
    // 2. FILE pointer fp3 has reached EOF

    // Adding new label in SYMTAB
    if(repeat_flag==0)
    {
        fprintf(fp3,"%s %x\n",inst.label,*address);
    }

    return;
}

// This function is called before ProcessLines
// Reads first line expecting "Start" opcode. Address is updated in LOCCTR.
// linecount is returned.
int ProcessFirstLine(FILE *fp1,FILE *fp2,FILE *fp3,struct instruction temp_inst,int *address,char opcodearray[][MAXOP],int codearray[],int sizearray[])
{
    char temp;
    char line[200];
    int i=0;
    int linecount=0;

    while(!feof(fp1)){

        fgets(line,30,fp1);

        if(line[strlen(line)-1]=='\n')
            line[strlen(line)-1]='\0';    // fgets() reads '\n' in the string

        // Empty Lines
        if(line[0]=='\0')
        {
            continue;
        }

        // Comment Lines
        if(line[0]=='.')
        {
            printf("%s\n",line);
            fprintf(fp2,"%s\n",line);
            continue;
        }

        temp_inst = tokenizeInstruction(line);

        getAddress(temp_inst,address);

        displayInstruction(temp_inst);

        if(start_f==1){
            // Don't call Update Address
            fprintf(fp2," %s %s %s\n",temp_inst.label,temp_inst.opcode,temp_inst.operand);
        }

        else{
            // Symbol Table
            if(temp_inst.label!="")
            {
                addSYMTAB(address,fp3,temp_inst);
            }

            //int hex_operand = (int)strtol(temp_inst.operand,NULL,16);  // Converting operand to hex
            fprintf(fp2,"%x %s %s %s\n",*address,temp_inst.label,temp_inst.opcode,temp_inst.operand);

            // Update Address
            updateAddress(address,temp_inst,opcodearray,codearray,sizearray);
        }


        linecount++;
        return linecount;  // Break from the loop after reading first line
    }
    return 1;
}

// Function to processes every line after first line till EOF. Tokenize every instruction,
// checks and adds symbols in SYMTAB and updates address
int ProcessLines(FILE *fp1,FILE *fp2,FILE *fp3,struct instruction temp_inst,int *address,char opcodearray[][MAXOP],int codearray[],int sizearray[])
{
    char temp;  // Temporary character variable
    char line[30];  // Temporary string to hold a line/instruction
    int i=0;
    int linecount = 0;

    while(!feof(fp1))
    {
        fgets(line,30,fp1);

        if(line[strlen(line)-1]=='\n')
            line[strlen(line)-1]='\0';    // fgets() reads '\n' in the string
        // If the read character is '\n' then we have completed reading a line

        if(line[0]=='\0')
        {
            continue;
        }

        // Comment Lines
        if(line[0]=='.')
        {
            printf("%s\n",line);
            fprintf(fp2,"%s\n",line);
            continue;
        }

        temp_inst = tokenizeInstruction(line);

        displayInstruction(temp_inst);

        if(!strcmp(temp_inst.opcode,"End"))
        {
            //int hex_operand = (int)strtol(temp_inst.operand,NULL,16);  // Converting operand to hex
            fprintf(fp2,"%x %s %s %s\n",*address,temp_inst.label,temp_inst.opcode,temp_inst.operand);
            return linecount++;
        }

        if(!strcmp(temp_inst.opcode,"BASE")||!strcmp(temp_inst.opcode,"Base")||!strcmp(temp_inst.opcode,"base"))
        {
            fprintf(fp2," %s %s\n",temp_inst.opcode,temp_inst.operand);

            continue;
        }

        // Symbol Table
        if(temp_inst.label!="")
        {
            addSYMTAB(address,fp3,temp_inst);
        }

        //int hex_operand = (int)strtol(temp_inst.operand,NULL,16);  // Converting operand to hex
        fprintf(fp2,"%04X %s %s %s\n",*address,temp_inst.label,temp_inst.opcode,temp_inst.operand);

        updateAddress(address,temp_inst,opcodearray,codearray,sizearray);

        linecount++;

    }
    return linecount;
}