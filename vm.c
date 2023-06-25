//Author name: Yuyang Zhang
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "compiler.h"
#define MAX 500

int bp, sp, pc;
int pas[MAX] = {0};

int base(int L);

void virtual_machine(instruction *code)
{
    //set variables
    char op[4] = "OOO";
    int OP, L, M;
    int Halt = 1;
    int IR[MAX];
    int i = 0;
    int codeI = 0;

    while(code[codeI].op != -1)
    {
        pas[3 * codeI] = code[codeI].op;
        pas[3 * codeI + 1] = code[codeI].l;
        pas[3 * codeI + 2] = code[codeI].m;
        codeI++;
    }

	bp = 3 * codeI;
	sp = bp - 1;
	pc = 0;
	int initialbp = bp;


    //output
    printf("\t\t  PC   BP   SP   stack\n");
    printf("Initial values:   %2d   %2d   %2d\n", pc, bp, sp);
    printf("\n");


	while(Halt != 0)//check Halt
    {
        //fetch cycle
        for(int l = 0; l < 3; l++)
            IR[l] = pas[l + pc];
        OP = IR[0];
        L = IR[1];
        M = IR[2];
        pc = pc + 3;

        //execute cycle
        switch(OP)
        {
        //LIT
        case 1:
            sp = sp + 1;
            pas[sp] = M;
            strcpy(op, "LIT");
            break;

        //OPR
        case 2:
            if(M == 0)//RTN
            {
                sp = bp - 1;
                bp = pas[sp + 2];
                pc = pas[sp + 3];
                strcpy(op, "RTN");
            }
            else if(M == 1)//NEG
            {
                pas[sp] = -1 * pas[sp];
                strcpy(op, "NEG");
            }
            else if(M == 2)//ADD
            {
                sp = sp - 1;
                pas[sp] = pas[sp] + pas[sp + 1];
                strcpy(op, "ADD");
            }
            else if(M == 3)//SUB
            {
                sp = sp - 1;
                pas[sp] = pas[sp] - pas[sp + 1];
                strcpy(op, "SUB");
            }
            else if(M == 4)//MUL
            {
                sp = sp - 1;
                pas[sp] = pas[sp] * pas[sp + 1];
                strcpy(op, "MUL");
            }
            else if(M == 5)//DIV
            {
                sp = sp - 1;
                pas[sp] = pas[sp] / pas[sp + 1];
                strcpy(op, "DIV");
            }
            else if(M == 6)//MOD
            {
                sp = sp - 1;
                pas[sp] = pas[sp] % pas[sp + 1];
                strcpy(op, "MOD");
            }
            else if(M == 7)//EQL
            {
                sp = sp - 1;
                pas[sp] = pas[sp] == pas[sp + 1];
                strcpy(op, "EQL");
            }
            else if(M == 8)//NEQ
            {
                sp = sp - 1;
                pas[sp] = pas[sp] != pas[sp + 1];
                strcpy(op, "NEQ");
            }
            else if(M == 9)//LSS
            {
                sp = sp - 1;
                pas[sp] = pas[sp] < pas[sp + 1];
                strcpy(op, "LSS");
            }
            else if(M == 10)//LEQ
            {
                sp = sp - 1;
                pas[sp] = pas[sp] <= pas[sp + 1];
                strcpy(op, "LEQ");
            }
            else if(M == 11)//GTR
            {
                sp = sp - 1;
                pas[sp] = pas[sp] > pas[sp + 1];
                strcpy(op, "GTR");
            }
            else if(M == 12)//GEQ
            {
                sp = sp - 1;
                pas[sp] = pas[sp] >= pas[sp + 1];
                strcpy(op, "GEQ");
            }
            break;

        //LOD
        case 3:
            sp = sp + 1;
            pas[sp] = pas[base(L) + M];
            strcpy(op, "LOD");
            break;

        //STO
        case 4:
            pas[base(L) + M] = pas[sp];
            sp = sp - 1;
            strcpy(op, "STO");
            break;

        //CAL
        case 5:
            pas[sp + 1] = base(L);// static link (SL)
            pas[sp + 2] = bp;// dynamic link (DL)
            pas[sp + 3] = pc;// return address (RA)
            bp = sp + 1;
            pc = M;
            strcpy(op, "CAL");
            break;

        //INC
        case 6:
            sp = sp + M;
            strcpy(op, "INC");
            break;

        //JMP
        case 7:
            pc = M;
            strcpy(op, "JMP");
            break;

        //JPC
        case 8:
            if(pas[sp] == 0)
                pc = M;
            sp = sp - 1;
            strcpy(op, "JPC");
            break;

        //SYS
        case 9:
            strcpy(op, "SYS");

            if(M == 1)
            {
                printf("Output result is: ");
                printf("%d\n", pas[sp]);
                sp = sp - 1;
            }
            else if(M == 2)
            {
                sp = sp + 1;
                printf("Please Enter an Integer: ");
                scanf("%d", &pas[sp]);
            }
            else if(M == 3)
            {
                Halt = 0;
            }
            break;

        default:
            printf("Invalid input value!");
            exit(1);
        }

        printf("   %s  %2d  %2d    %2d   %2d   %-d   ", op, L, M, pc, bp, sp);//output each line

        //output first part of AR
        if(bp == initialbp)
        {
            if(sp > bp)
            {
                for(int j = 0; j <= (sp - bp); j++)
                    printf("%-2d ", pas[initialbp + j]);
            }
        }

        //output second part of AR
        if(bp > initialbp)
        {
            for(int j = 0; j < (bp - initialbp); j++)
                printf("%-2d ", pas[initialbp + j]);
            if(sp > bp)//determine if the AR need |
            {
                printf("| ");
                for(int k = 0; k <= (sp - bp); k++)
                    printf("%-2d ", pas[bp + k]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int base(int L)
{
    int arb = bp;// arb = activation record base
    while (L > 0)//find base L levels down
	{
		arb = pas[arb];
		L--;
	}
	return arb;
}

