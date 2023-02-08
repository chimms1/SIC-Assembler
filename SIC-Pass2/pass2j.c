// Contributed by Yash Deshpande and Shreyas Joshi
// Calculating object codes and writing into output.txt

// Requires Intermediate file and Symbol Table file from Pass1

// Note: char arrays can be used instead of char pointers to avoid unwanted in-place modification of variables

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXOP 100

static int optab_size;  // Holds OPTAB size
static int symtab_size; // Holds SYMTAB size
static int symbol_found = 0;

// Structure to hold components of instruction
struct instruction
{
    int location;
    char *label;
    char *opcode;
    char *operand;
};

// Structure to hold one entry of OPTAB
struct optabcode
{
    char *opcode;
    int objectcode;
};

// Function Declarations--------

// Returns program length from the temp file created in Pass 1
int getProgram_Length();

// Reads First Line from intermediate-file and writes Header Record in Object Program and puts entry in output file
int WriteFirstLine(FILE *fp1,FILE *fp4,FILE *fp5,int program_length);

// Function returns structure of OPTAB after tokenizing one entry present in line string
struct optabcode tokenizeOpcode(char line[]);

/* Function returns number of entries in OPTABLE after reading from file
and storing object code in Arrays*/
int getOPTAB(FILE *fp3,char opcodearray[][MAXOP],int codearray[]);

/* Function returns number of entries in SYMTAB after reading from file
and storing object code in Arrays*/
int getSYMTAB(FILE *fp2,char opcodearray[][MAXOP],int codearray[]);

// Function to tokenize instruction from a line (string) and return structure of instructure
struct instruction tokenizeInstructionLine(char line[]);

// Returns address of a given symbol from the SYMTAB
int getSYMADD(char symbol[],char symbolarray[][MAXOP],int addarray[]);

// Function assembles the object code and returns size of the current instruction's hex object code
int getObjectCode(struct instruction temp_inst,char opcodearray[][MAXOP],int codearray[],char symbolarray[][MAXOP],int addarray[],char hexop[]);

// Reads all lines from intermediate file, Get Object Code for every Instruction and write Text Records in Object Program
void ProcessTextLines(FILE *fp1,FILE *fp2,FILE *fp3,FILE *fp4,FILE *fp5,char opcodearray[][MAXOP],int codearray[],char symbolarray[][MAXOP],int addarray[]);

int main()
{
    FILE *fp1;  // Intermediate file pointer
    FILE *fp2;  // SYMTAB file pointer
    FILE *fp3;  // OPTAB file pointer
    FILE *fp4;  // Object Program pointer
    FILE *fp5;  // Output file pointer

    fp1=fopen("intermediate-file.txt","r");
    fp2=fopen("SYMTAB.txt","r");
    fp3=fopen("OPTAB.txt","r");
    fp4=fopen("Object-Program.txt","w");
    fp5=fopen("output.txt","w");


    // File opening Exceptions
    if(fp1==NULL){
        printf("  => Error opening the read file\n\n");
        return 1;
    }
    if(fp2==NULL){
        printf("  => Error opening the intermediate file\n\n");
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
        printf("  => Error opening the Object Program file\n\n");
        return 5;
    }

    printf("  -Files opened- \n\n");

    // Get Program Length
    int program_length = getProgram_Length();

    // Header Record
    int start_address = WriteFirstLine(fp1,fp4,fp5,program_length);

    /* Reading OPTAB and SYMTAB------------------------*/

    char opcodearray[MAXOP][MAXOP]; // Array of strings to hold operation names
    int codearray[MAXOP];   // Array of int to hold corresponding object codes

    printf("\n\n");
    optab_size = getOPTAB(fp3,opcodearray,codearray);

    char symbolarray[MAXOP][MAXOP]; // Array of strings to hold operation names
    int addarray[MAXOP];   // Array of int to hold corresponding object codes

    printf("\n\n");
    symtab_size = getSYMTAB(fp2,symbolarray,addarray);
    printf("\n\n");

    /*--------------------------------------------------*/


    // Text Records
    ProcessTextLines(fp1,fp2,fp3,fp4,fp5,opcodearray,codearray,symbolarray,addarray);

    // End Record
    fprintf(fp4,"E^%06x\n",start_address);


    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);

    printf("  -Files closed- \n\n");
    return 0;   // Program execution successful
}
// End of Main function ==============================================================

// Function Definitions -------

// Returns program length from the temp file created in Pass 1
int getProgram_Length()
{
    FILE *fp = fopen("temp.txt","r");
    int length;

    fscanf(fp,"%x",&length);

    fclose(fp);

    return length;
}

// Reads First Line from intermediate-file and writes Header Record in Object Program and puts entry in output file
int WriteFirstLine(FILE *fp1,FILE *fp4,FILE *fp5,int program_length)
{
    char name[100];
    char op[100];
    int num;

    fscanf(fp1," %s %s %x\n",name,op,&num);

    // Writing in Object Program
    fprintf(fp4,"H^%7s^%7x^%7x\n",name,num,program_length);

    // Writing in output file
    fprintf(fp5,"$ %s %s %X\n",name,op,num);

    return num;
}

// Function returns structure of OPTAB after tokinizing one entry present in line string
struct optabcode tokenizeOpcode(char line[])
{
    struct optabcode temp;

    char *token1=NULL;
    char *token2=NULL;

    temp.opcode=strtok(line," ");
    token2=strtok(NULL," ");

    sscanf(token2,"%X",&temp.objectcode);

    return temp;
}


/* Function returns number of entries in OPTABLE after reading from file
and storing objectcode in Structure Array*/
int getOPTAB(FILE *fp3,char opcodearray[][MAXOP],int codearray[])
{
    char line[30];  // Temporary string to hold a line/instruction
    struct optabcode temp_optab;
    int j=0;

    while(!feof(fp3))
    {
        // Read a character
        fgets(line,30,fp3);


        if(feof(fp3))
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

        strcpy(opcodearray[j],temp_optab.opcode);

        codearray[j] = temp_optab.objectcode;

        printf("%s %X\n",opcodearray[j],codearray[j]);

        j++;
    }

    // returns number of entries in OPTABLE
    return j;
}


/* Function returns number of entries in SYMTAB after reading from file
and storing objectcode in Arrays*/
int getSYMTAB(FILE *fp2,char opcodearray[][MAXOP],int codearray[])
{
    char line[30];  // Temporary string to hold a line/instruction
    struct optabcode temp_optab;
    int j=0;

    while(!feof(fp2))
    {
        // Read a character
        fgets(line,30,fp2);


        if(feof(fp2))
        {
            printf("SYMTAB file reading finished\n");
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

        strcpy(opcodearray[j],temp_optab.opcode);

        codearray[j] = temp_optab.objectcode;

        printf("%s %X\n",opcodearray[j],codearray[j]);

        j++;
    }

    // returns number of entries in SYMTABLE
    return j;
}

// Function to tokenize instruction from a line (string) and return structure of instructure
struct instruction tokenizeInstructionLine(char line[])
{
    int count=0;
    struct instruction inst;
    char *token1=NULL;
    char *token2=NULL;
    char *token3=NULL;
    char *token4=NULL;

    token1=strtok(line," ");
    token2=strtok(NULL," ");
    token3=strtok(NULL," ");
    token4=strtok(NULL,"");

    if(token3==NULL && token4==NULL)
        count=2;

    if(token3!=NULL && token4==NULL)
        count=3;

    if(token4!=NULL)
        count=4;

    sscanf(token1,"%X",&inst.location);

    if(count==2)
    {
        inst.label="";
        inst.opcode=token2;
        inst.operand="";
    }

    if(count==3)
    {
        inst.label="";
        inst.opcode=token2;
        inst.operand=token3;
    }

    if(count==4)
    {
        inst.label=token2;
        inst.opcode=token3;
        inst.operand=token4;
    }

    return inst;
}

// Returns address of a given symbol from the SYMTAB
int getSYMADD(char symbol[],char symbolarray[][MAXOP],int addarray[])
{
    symbol_found=0;
    int i=0;

    // a && (b||c)
    // If instruction operand has indexed addressing
    if(symbol[strlen(symbol)-2]==',' && (symbol[strlen(symbol)-1]=='X'||symbol[strlen(symbol)-1]=='x'))
    {
        symbol[strlen(symbol)-2] = '\0';

    }
    // --------Now Operand contains only the symbol---------

    // Search for Symbol in SYMTAB
    for(i=0;i<symtab_size;i++)
    {
        if(!strcmp(symbol,symbolarray[i]))
        {
            symbol_found = 1;
            break;
        }
    }
    // If found
    if(symbol_found==1)
        return addarray[i];

    // If not found return 0
    else
    {
        printf("\nERROR: symbol %s not found in SYMBOL Table\n\n",symbol);
        return 0;
    }

}

// Function assembles the object code and returns size of the current instruction's hex object code
int getObjectCode(struct instruction temp_inst,char opcodearray[][MAXOP],int codearray[],char symbolarray[][MAXOP],int addarray[],char hexop[])
{

    long long int objc; /* Holds integer Hex value of the operand (read as string)*/
    int symadd; // Holds symbol address
    char dollar[2]="$"; // string with $
    int opcode_found=0; // flag set to 1 if opcode is found
    char *stringobj;
    int i=0;

    //Search OPTAB for opcode and if found return objectcode
    for(i=0;i<MAXOP;i++)
    {

        if(!strcmp(temp_inst.opcode,opcodearray[i]))
        {
            opcode_found = 1;
            break;
        }
    }

    if(opcode_found==1)
    {
        char *tempchar;
        itoa(codearray[i],tempchar,16);
        strcpy(hexop,tempchar);

        symadd = getSYMADD(temp_inst.operand,symbolarray,addarray);
        itoa(symadd,tempchar,16);
        strcat(hexop,tempchar);

        // If instruction operand has indexed addressing------------------------------------------------
        /*if(symbol_found==1)
        {
            char symbol[MAXOP];


            strcpy(symbol,temp_inst.operand);

            // a && (b||c)
            if(symbol[strlen(symbol)-2]==',' && (symbol[strlen(symbol)-1]=='X'||symbol[strlen(symbol)-1]=='x'))
            {
                char tempstring[MAXOP];

                int hexop_int = (int)strtol(hexop,NULL,16);

                hexop_int += 8000;


            }
            // --------Now Operand contains only the symbol---------

        }*/
        //-----------------------------------------------------------------------------------------------


        return 6;

    }

    //If opcode is not found in OPTAB, it may be WORD, BYTE, RESB, RESW
    if(!strcmp(temp_inst.opcode,"WORD")||!strcmp(temp_inst.opcode,"Word")||!strcmp(temp_inst.opcode,"word"))
    {
        objc = (int)strtol(temp_inst.operand,NULL,16);
        char hex_word[MAXOP];
        itoa(objc,hex_word,16);

        /*
        char temphexop[MAXOP];
        //temphexop[0]='\0';
        //---------------------------------
        // Your output should always be 6 digit string

        // 1. Count no. digits in hex_word
        // 2. Put (6 minus count_digits) number of zeros in temphexop
        // 3. Concat hex_string with temphexop

        int digit_count = strlen(hex_word);

        for(int j = 0;j<(6-digit_count);j++)
        {
            strcat(temphexop,"0");
        }

        strcat(temphexop,hex_word);
        //---------------------------------
        */

        strcpy(hexop,hex_word);

        return 6;
    }

    else if(!strcmp(temp_inst.opcode,"RESW")||!strcmp(temp_inst.opcode,"Resw")||!strcmp(temp_inst.opcode,"resw"))
    {
        // return dollar;
        strcpy(hexop,"$");
        objc = (int)strtol(temp_inst.operand,NULL,16);  // Converting operand to hex

        return 0;
    }

    else if(!strcmp(temp_inst.opcode,"RESB")||!strcmp(temp_inst.opcode,"Resb")||!strcmp(temp_inst.opcode,"resb"))
    {
        // return dollar;
        strcpy(hexop,"$");

        objc = (int)strtol(temp_inst.operand,NULL,16);  // Converting operand to hex

        return 0;
    }

    else if(!strcmp(temp_inst.opcode,"BYTE")||!strcmp(temp_inst.opcode,"Byte")||!strcmp(temp_inst.opcode,"byte"))
    {
        int count=0;

        // If BYTE operand contains hex characters
        if(temp_inst.operand[0]=='X'||temp_inst.operand[0]=='x')
        {

            int i=2;    // characters start at 2nd index inside C'sample' or X'sample'
            char hex_string[100];   // Variable to hold operand except c/x'----'
            int k=0;
            while(temp_inst.operand[i]!=39)
            {
                hex_string[k]=temp_inst.operand[i];

                k++;
                i++;
                count++;
            }
            hex_string[k]='\0';
            strcpy(hexop,hex_string);

            return strlen(hexop);
        }

        // If BYTE operand contains constants
        else if(temp_inst.operand[0]=='C'||temp_inst.operand[0]=='c')
        {
            char hex_string[1000];
            char *temp_string;
            int tempChar;
            int i=2;    // characters start at 2nd index inside C'sample' or X'sample'


            hex_string[0]='\0';
            while(temp_inst.operand[i]!=39)
            {
                // 1. Take char into decimal ascii
                    tempChar=temp_inst.operand[i];

                    /*printf("\n%d tempchar",i);*/

                // 2. Convert decimal ascii to hex ascii (which is actually just decimal int to hexadecimal int)
                // Convert using itoa(), that hex ascii to string in a temp string (char pointer) variable
                itoa(tempChar,temp_string,16);

                /*printf("\n%d itoa",i);*/

                // 3. Concatenate temp with main string
                strcat(hexop,temp_string);

                /*printf("\n%d strcat",i);*/

                /* int_array[k]=text[i];*/
                i++;
                count++;
            }
            /*printf("\nObjcode: %s\n",hexop);*/

            return strlen(hexop);
        }
    }
}

void ProcessTextLines(FILE *fp1,FILE *fp2,FILE *fp3,FILE *fp4,FILE *fp5,char opcodearray[][MAXOP],int codearray[],char symbolarray[][MAXOP],int addarray[])
{
    //char temp;  // Temporary character variable
    char line[30];  // Temporary string to hold a line/instruction

    struct instruction temp_inst;

    char strarray[MAXOP];
    strarray[0]='\0';
    int start;
    int count_w_b = 0;

    char hexop[1000];
    int inthexop;
    char operandhere[100];
    char opcodehere[100];
    int sizecounter = 0;

    char symbols[MAXOP][MAXOP];

    while(!feof(fp1))
    {
        // Read a character
        fgets(line,30,fp1);

        if(feof(fp1))
        {
            printf("Intermediate file reading finished\n");
            break;
        }

        if(line[0]=='\n')
        {
            printf("Here lies a backslash n  \n");
            continue;
        }

        if(line[strlen(line)-1]=='\n')
            line[strlen(line)-1]='\0';    // fgets() reads '\n' in the string

        printf("%s\n",line);

        temp_inst = tokenizeInstructionLine(line);

        strcpy(operandhere,temp_inst.operand);
        strcpy(opcodehere,temp_inst.opcode);

        //******* Getting the OBJECT CODE of the instruction in hexop *******
        hexop[0]='\0';
        int cursize=getObjectCode(temp_inst,opcodearray,codearray,symbolarray,addarray,hexop);

        // For first entry in text record
        if(sizecounter==0 && (strcmp(opcodehere,"RESW") && strcmp(opcodehere,"RESB")))
        {
            start=temp_inst.location;
            count_w_b = 0;
        }

        // Text record entry is full or stopping condition is reached, now write into Object Program
        if(((!strcmp(opcodehere,"RESW") || !strcmp(opcodehere,"RESB")) && count_w_b==0 )|| sizecounter+cursize >60||!strcmp(opcodehere,"End"))
        {
            fprintf(fp4,"T^%06x^%02x^%s\n",start,sizecounter/2,strarray);
            sizecounter=0;
            strarray[0]='\0';

            if((!strcmp(opcodehere,"RESW") || !strcmp(opcodehere,"RESB")))
            {
                count_w_b++;
            }
            else
            {
                count_w_b = 0;
            }
        }

        // If instruction is BYTE and not RESW,RESB
        if(!strcmp(opcodehere,"BYTE") && strcmp(opcodehere,"RESW") && strcmp(opcodehere,"RESB"))
        {

            strcat(strarray,hexop);
            strcat(strarray,"^");

            if(sizecounter==0)
            {
                start=temp_inst.location;
            }
            sizecounter+=strlen(hexop);
        }

        // If instruction other than BYTE and not RESW,RESB
        else
        {
            if(strcmp(opcodehere,"RESW") && strcmp(opcodehere,"RESB"))
            {
            strcat(strarray,hexop);
            strcat(strarray,"^");
            if(sizecounter==0)
            {
                start=temp_inst.location;

            }
            sizecounter+=6;
            }

        }
        // Writing in output.txt

        // If Instruction is BYTE or RESW or RESB
        if(!strcmp(opcodehere,"BYTE")||!strcmp(opcodehere,"RESW")||!strcmp(opcodehere,"RESB"))
        {
            fprintf(fp5,"%X %s %s %s %s\n",temp_inst.location,temp_inst.label,opcodehere,operandhere,hexop);
        }

        else
        {
            inthexop = (int)strtol(hexop,NULL,16);
            fprintf(fp5,"%X %s %s %s %06X\n",temp_inst.location,temp_inst.label,opcodehere,operandhere,inthexop);
        }
    }
}
//=======================================================================================
// End of Pass-2
