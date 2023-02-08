// Contributed by - Yash Deshpande and Shreyas Joshi

// First Build of Pass2
// Requires Intermediate file and Symbol Table file from Pass1
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Structure to hold components of instruction
struct instruction
{
    int location;
    char *label;
    char *opcode;
    char *operand;
};

void WriteFirstLine(FILE *fp1,FILE *fp4,FILE *fp5,int program_length)
{
    char name[100];
    char op[100];
    int num;

    fscanf(fp1," %s %s %x\n",name,op,&num);

    // Writing in Object Program
    fprintf(fp4,"H^%7s^%7x^%7d",name,num,program_length);

    // Writing in output file
    fprintf(fp5,"$ %s %s %x",name,op,num);
}

ProcessTextLines(FILE *fp1,FILE *fp2,FILE *fp3,FILE *fp4,FILE *fp5)
{
    
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
    ProcessTextLines(fp1,fp2,fp3,fp4,fp5);


    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);
    fclose(fp5);

}
