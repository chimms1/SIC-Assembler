// Contributed by - Yash Deshpande and Shreyas Joshi

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static int start_f = 0; 

// Structure to hold components of instruction
struct instruction
{
    char *label;
    char *opcode;
    char *operand;
};

// Function to display an instruction
void displayInstruction(struct instruction inst)
{
    printf("%s %s %s\n",inst.label,inst.opcode,inst.operand);
}

// Function to tokenize instruction from a line (string) and return structure of instructure
struct instruction token(char line[])
{
    int count=0;
    struct instruction inst;
    char *token1=NULL;
    char *token2=NULL;
    char *token3=NULL;

    token1=strtok(line," ");
    token2=strtok(NULL," ");
    token3=strtok(NULL," ");

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
        // if(!strcmp(inst.label,"(null)"))
        // {
        //     printf("\nScheme\n");
        // }
        inst.opcode=token2;
        inst.operand=token3;
    }
    return inst;
}


// Returning Address as Hexadecimal integer even though in struct it is a string?
void getAddress(struct instruction inst,int *address)
{
    static int read=0;  // read flag is initially 0, after reading address it is set to 1

    if(!strcmp(inst.opcode,"Start"))
    {
        if(inst.operand==NULL){
            *address=0;
            read=1;
            //printf("address = %d\n",address);
        }

        if(inst.operand!=NULL)
        {
            //*address=atoi(inst.operand);
            sscanf(inst.operand,"%X",address);
            read=1;
            start_f=1;
            //printf("address = %d\n",address);
        }
    }
    if(read==0)
    {
        *address=0;
        read=1;
    }
}

// This function is called first
/* Reads first line expecting "Start" opcode. Address is updated in LOCCTR.
Line counter is incremented and Character counter i is used and reset.
*/
int ProcessFirstLine(FILE *fp1,FILE *fp2,struct instruction inst,int *address)
{
    char temp;
    char line[30];
    int i=0;
    int linecount=0;

    while(!feof(fp1)){
        
        // Read a character
        fscanf(fp1,"%c",&temp);
    
        // Put it in a character array
        line[i]=temp;

        // Increment string index
        i++;

        if(temp=='\n')
        {
            // string has been read till '\n' so string is "example\n"
            // But we need one line in one string variable - "example\0"
            line[--i]='\0'; // Adding null terminator
            i=0;
            
            inst = token(line);

            getAddress(inst,address);

            displayInstruction(inst);
            if(start_f==1){
                fprintf(fp2," %s %s %s\n",inst.label,inst.opcode,inst.operand);
            }
            else{
                fprintf(fp2,"%x %s %s %s\n",*address,inst.label,inst.opcode,inst.operand);
            }
            
            
            linecount++;
            return linecount;  // Break from the loop after reading first line
        }
    }
    return 1;
}

int ProcessLines(FILE *fp1,FILE *fp2,struct instruction inst,int *address)
{
    char temp;  // Temporary character variable
    char line[30];  // Temporary string to hold a line/instruction
    int i=0;
    int linecount = 0;

    while(!feof(fp1))
    {
        // Read a character
        fscanf(fp1,"%c",&temp);
        
        // Put it in a character array
        line[i]=temp;

        // Increment string index
        i++;

        // If the read character is '\n' then we have completed reading a line
        if(temp=='\n')
        {
            // string has been read till '\n' so string is "example\n"
            // But we need one line in one string variable - "example\0"
            line[--i]='\0'; // Adding null terminator
            i=0;
            
            inst = token(line);

            if(!strcmp(inst.opcode,"End"))
            {
                break;
            }

            displayInstruction(inst);
            fprintf(fp2,"%x %s %s %s\n",*address,inst.label,inst.opcode,inst.operand);

            linecount++;
            //printf("\n---- Line  = %d\n\n",*linecount);
        }
    }
    
    // Last Line
    line[--i]='\0';
    inst = token(line);
    displayInstruction(inst);
    fprintf(fp2,"%x %s %s %s\n",*address,inst.label,inst.opcode,inst.operand);
    linecount++;

    return linecount;
}

/*
getInstArray(FILE *fp1,struct instruction A[],int n)
{
    char temp;
    char line[30];
    int i=0;
    int j=0;

    // If "Copy Start x" exists
    if(start_f=1)
    {
        j=1;
    }

    while(!feof(fp1)){
        
        // Read a character
        fscanf(fp1,"%c",&temp);
    
        // Put it in a character array
        line[i]=temp;

        // Increment string index
        i++;

        if(temp=='\n')
        {
            // string has been read till '\n' so string is "example\n"
            // But we need one line in one string variable - "example\0"
            line[--i]='\0'; // Adding null terminator
            i=0;
            
            A[j] = token(line);
            j++;
        }
    }
}
*/

int main()
{
    FILE *fp1;  // Read file pointer
    FILE *fp2;  // Write file pointer
    fp1=fopen("input.txt","r");
    fp2=fopen("output.txt","w");

    // File opening Exceptions
    if(fp1==NULL){
        printf("  => Error opening the read file\n\n");
        return 1;
    }

    if(fp2==NULL){
        printf("  => Error opening the write file\n\n");
        return 2;
    }

    printf("  -Files opened- \n\n");
    

    printf("Contents of Read file is: \n");
    printf("--------------------------\n\n");
    
    struct instruction inst;
    int address;
    int linecount;

    //printf("\nLabel Opcode Operand\n\n");

    // Read First Line to check for Start address
    linecount = ProcessFirstLine(fp1,fp2,inst,&address);

    // Read Remaining Lines
    linecount += ProcessLines(fp1,fp2,inst,&address);

    printf("\nAddress = Hex: %x, Decimal: %d\n",address,address);

    printf("\n\n--------------------------\n");
    printf("\nNumber of lines is %d\n",linecount);
    printf("\n\n==> Read and Write Complete \n");

    fclose(fp1);
    fclose(fp2);
    
    printf("\n  -Files closed- \n\n");

    //printf("Creating structure Array\n");
    //struct instruction *A = (struct instruction*)malloc(linecount*sizeof(struct instruction));

    
    /*
    
    */
    return 0; // Execution Completed Successfully
}
