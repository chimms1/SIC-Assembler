// Contributed by - Yash Deshpande and Shreyas Joshi

// Calculating object codes and writing into output.txt
// Requires Intermediate file and Symbol Table file from Pass1

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXOP 10

// Structure to hold components of instruction
struct instruction
{
    int location;
    char *label;
    char *opcode;
    char *operand;
};

struct optabcode
{
    char *opcode;
    int objectcode;
};

void WriteFirstLine(FILE *fp1,FILE *fp4,FILE *fp5,int program_length)
{
    char name[100];
    char op[100];
    int num;

    fscanf(fp1," %s %s %x\n",name,op,&num);

    // Writing in Object Program
    fprintf(fp4,"H^%7s^%7x^%7d\n",name,num,program_length);

    // Writing in output file
    fprintf(fp5,"$ %s %s %X\n",name,op,num);
}


void getObjectCode(struct instruction temp_inst,char opcodearray[][MAXOP],int codearray[],char hexop[])
{

    long long int objc;
    char dollar[2]="$";
    int opcode_found=0;
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
        // return;

    }

    //If opcode is not found in OPTAB, it may be WORD, BYTE, RESB, RESW
    if(!strcmp(temp_inst.opcode,"WORD")||!strcmp(temp_inst.opcode,"Word")||!strcmp(temp_inst.opcode,"word"))
    {
        objc = (int)strtol(temp_inst.operand,NULL,16);
        itoa(objc,hexop,16);
        // return stringobj;
        // strcpy(hexop,stringobj);
    }

    else if(!strcmp(temp_inst.opcode,"RESW")||!strcmp(temp_inst.opcode,"Resw")||!strcmp(temp_inst.opcode,"resw"))
    {
        // return dollar;
        strcpy(hexop,"$");
    }

    else if(!strcmp(temp_inst.opcode,"RESB")||!strcmp(temp_inst.opcode,"Resb")||!strcmp(temp_inst.opcode,"resb"))
    {
        // return dollar;
        strcpy(hexop,"$");
    }

    else if(!strcmp(temp_inst.opcode,"BYTE")||!strcmp(temp_inst.opcode,"Byte")||!strcmp(temp_inst.opcode,"byte"))
    {

        if(temp_inst.operand[0]=='X'||temp_inst.operand[0]=='x') //x'78'
        {

            int i=2;    // characters start at 2nd index inside C'sample' or X'sample'
            char hex_string[100];
            int k=0;
            while(temp_inst.operand[i]!=39)
            {
                hex_string[k]=temp_inst.operand[i];

                k++;
                i++;
            }
            hex_string[k]='\0';

            strcpy(hexop,hex_string);

            // return hex_string;
        }

        else if(temp_inst.operand[0]=='C'||temp_inst.operand[0]=='c')
        {
            char hex_string[1000];
            char *temp_string;
            int tempChar;
            int i=2;    // characters start at 2nd index inside C'sample' or X'sample'

            hex_string[0]='\0';
            while(temp_inst.operand[i]!=39)
            {
                // Take char into decimal ascii
                    tempChar=temp_inst.operand[i];
                    //printf("\n%d tempchar",i);

                // Convert decimal ascii to hex ascii (which is actually just decimal int to hexadecimal int)
                // Convert using itoa(), that hex ascii to string in a temp variable
                itoa(tempChar,temp_string,16);

                //printf("\n%d itoa",i);

                // Concatenate temp with main string
                strcat(hexop,temp_string);
                //printf("\n%d strcat",i);

                // int_array[k]=text[i];
                i++;

            }
            printf("\nObjcode: %s",hexop);
        }
    }
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
    token4=strtok(NULL," ");

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

void ProcessTextLines(FILE *fp1,FILE *fp2,FILE *fp3,FILE *fp4,FILE *fp5,char opcodearray[][MAXOP],int codearray[])
{
    //char temp;  // Temporary character variable
    char line[30];  // Temporary string to hold a line/instruction
    //int linecount = 0;
    struct instruction temp_inst;
    char hexop[1000];
    char operandhere[100];

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

        // Getting the opcode from OPTAB
        hexop[0]='\0';
        getObjectCode(temp_inst,opcodearray,codearray,hexop) ;

        fprintf(fp5,"%X %s %s %s %s\n",temp_inst.location,temp_inst.label,temp_inst.opcode,operandhere,hexop);
        //printf("%X %s %s %s\n",temp_inst.location,temp_inst.label,temp_inst.opcode,temp_inst.operand);
    }
}

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



int main()
{
    FILE *fp1;  // Intermediate file pointer
    FILE *fp2;  // SYMTAB file pointer
    FILE *fp3;  // OPTAB file pointer
    FILE *fp4;  // Object Program pointer
    FILE *fp5;  // Output file pointer

    // fp1=fopen("input.txt","r");
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

    int program_length = 23;
    // char temp;
    // char line[200];
    // int i=0;
    struct instruction tempinstruction;

    WriteFirstLine(fp1,fp4,fp5,program_length);

    char opcodearray[5][MAXOP]; // Array of strings to hold operation names
    int codearray[MAXOP];   // Array of int to hold corresponding object codes

    int optable_size = getOPTAB(fp3,opcodearray,codearray);


    ProcessTextLines(fp1,fp2,fp3,fp4,fp5,opcodearray,codearray);


    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);

    printf("  -Files closed- \n\n");
    return 0;   // Program execution successful
}
