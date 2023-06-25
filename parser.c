/*
    Author name: Yuyang Zhang
	skeleton for Dr. Montagne's Systems Software project Summer 2022
	you may alter all of this code if desired, but you must provide clear
	instructions if you alter the compilation process or wish us to
	use different files during grading than the ones provided
	(driver.c, compiler.h, vm.o/.c, or lex.o/.c)
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

lexeme *tokens;
int lIndex = 0;
instruction *code;
int cIndex = 0;
symbol *table;
int tIndex = 0;
int level;
lexeme curr_token;
int error = 0;//determine if error happened

void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);

void pprogram();
void block();
void constdeclare();
int vardeclare();
void proceduredeclare(int addr);
void statement();
void condition();
void expression();
void term();
void factor();
void getNextToken();
int rel_op();

void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

instruction *parser_code_generator(lexeme *list)
{
	int i;
	tokens = list;
	code = malloc(sizeof(instruction) * MAX_ARRAY_SIZE);
	table = malloc(sizeof(symbol) * MAX_ARRAY_SIZE);

	// Your code here.
	// Make sure to remember to free table before returning always
	// and to free code and return NULL if there was an error

    //Recursively parse tokens.
    pprogram();

    if(error == 1)//check error
    {
        free(code);
        code = NULL;
        return NULL;
    }

    printsymboltable();//
    printassemblycode();//
    free(table);//free malloc space
    table = NULL;//set null
    code[cIndex].op = -1;

    return code;//
}

// adds an instruction to the end of the code array
void emit(int opname, int level, int mvalue)
{
	code[cIndex].op = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// adds a symbol to the end of the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].value = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

/*
starts at the end of the table and works backward (ignoring already
marked entries (symbols of subprocedures)) to mark the symbols of
the current procedure. it knows it's finished the current procedure
by looking at level: if level is less than the current level and unmarked
(meaning it belongs to the parent procedure) it stops
*/
void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

// does a linear pass through the symbol table looking for the symbol
// who's name is the passed argument. it's found a match when the names
// match, the entry is unmarked, and the level is equal to the current
// level. it returns the index of the match. returns -1 if there are no
// matches
int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

// returns -1 if a symbol matching the arguments is not in the symbol table
// returns the index of the desired entry if found (maximizing the level value)
int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=, +=, -=, *=, /=, or modassign\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Conflicting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	error = 1;//exit from execution
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].value, table[i].level, table[i].addr, table[i].mark);
	printf("\n");
}

void printassemblycode()
{
	int i;
	printf("Assembly Code:\n");
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].op);
		switch (code[i].op)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("MOD\t");
						break;
					case 7:
						printf("EQL\t");
						break;
					case 8:
						printf("NEQ\t");
						break;
					case 9:
						printf("LSS\t");
						break;
					case 10:
						printf("LEQ\t");
						break;
					case 11:
						printf("GTR\t");
						break;
					case 12:
						printf("GEQ\t");
						break;
					default:
					    printf("current code[i].m: %d", code[i].m);
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				printf("SYS\t");
				break;
			default:
				printf("err\t");
				printf("current code[i].m: %d", code[i].m);
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	printf("\n");
}

void pprogram()
{
    emit(7, 0, 0);//emit JMP
    addToSymbolTable(3, "main", 0, 0, 0, 0);//add main to symbol table
    level = -1;//set initial level
    getNextToken();
    block();
    if(error == 1) return;

    if(curr_token.type != periodsym)//error(1) for last token is not period
        printparseerror(1);
    if (error == 1) return;

    emit(9, 0, 3);//emit Halt
    code[0].m = table[0].addr * 3;//main m * 3
}

void block()
{
    if (error == 1) return;

    level++;//level + 1
    int jmpaddr = cIndex - 1;//set temp variable for jmp address
    int jmpaddr1 = tIndex - 1;

    //declarations
    constdeclare();
    int x = vardeclare();//x is how many variable spaces we’ll need on the stack
    proceduredeclare(jmpaddr);

    table[jmpaddr1].addr = cIndex;

    emit(6, 0, x + 3);//emit INC
    statement();
    if (error == 1) return;

    mark();
    level--;//level - 1
}

void constdeclare()//ident ":=" number {"," ident ":=" number}
{
    char name[12];//set temp variable
    if(curr_token.type == constsym)//determine constant type
    {
        do
        {
            getNextToken();
            if(curr_token.type != identsym)//check identifier
                printparseerror(2);//error(2) for not start with identifier
            if (error == 1) return;

            strcpy(name, curr_token.name);//copy to temp
            getNextToken();

            if(curr_token.type != becomessym)//check :=
                printparseerror(2);//error(2) for ident ":=" number {"," ident ":=" number}
            if (error == 1) return;

            getNextToken();

            if(curr_token.type != numbersym)
                printparseerror(2);//error(2) for ident ":=" number {"," ident ":=" number}
            if (error == 1) return;

            int mulcheck = multipledeclarationcheck(name);//check multiple declaration
            if(mulcheck != -1)
                printparseerror(18);//error(18) for identifier declared already
            if (error == 1) return;

            addToSymbolTable(1, name, curr_token.value, level, 0, 0);//add this constant to symbol table
            getNextToken();

            if(curr_token.type == identsym)
                printparseerror(13);//error(13) for find an identifier instead for the ending semicolon
            if (error == 1) return;

        }while(curr_token.type == commasym);//if comma then continue get tokens for next constant

        if(curr_token.type != semicolonsym)
            printparseerror(14);//error(14) for ending semicolon
        if (error == 1) return;

        getNextToken();
    }
}

int vardeclare()//var-declaration  ::= [ "var "ident {"," ident} “;"].
{
    int address = 3;//set initial address

    if(curr_token.type == varsym)//determine if variable
    {
        do
        {
            getNextToken();
            if(curr_token.type != identsym)
                printparseerror(3);//error(3) for identifier is missing
            if (error == 1) return -1;

            int mulcheck = multipledeclarationcheck(curr_token.name);//check multiple declaration
            if(mulcheck != -1)
                printparseerror(18);//error(18) for multiple variable declaration
            if (error == 1) return -1;

            addToSymbolTable(2, curr_token.name, curr_token.value, level, address, 0);//add this variable to symbol table

            address++;//address + 1
            getNextToken();

            if(curr_token.type == identsym)
                printparseerror(13);//find an identifier instead of ending semicolon
            if (error == 1) return -1;

        }while(curr_token.type == commasym);//continue get next variable

        if(curr_token.type != semicolonsym)
            printparseerror(14);//error(14) for ending semicolon
        if (error == 1) return -1;

        getNextToken();
    }
    return address - 3;//subtract address back to 0
}

void proceduredeclare(int addr)//{ "procedure" ident ";" block ";" }
{
    int address = 3;//set initial address
    int jmpaddr = addr;
    int procex;

    while(curr_token.type == procsym)//determine if procedure
    {
        getNextToken();
        if(curr_token.type != identsym)
            printparseerror(4);//error(4) for identifier is missing before block is entered
        if (error == 1) return;

        int mulcheck = multipledeclarationcheck(curr_token.name);//check multiple declaration
        if(mulcheck != -1)
            printparseerror(18);//error(18) for multiple variable declaration
        if (error == 1) return;

        addToSymbolTable(3, curr_token.name, 0, level, jmpaddr + 1, 0);//add this procedure to symbol table
        procex = tIndex - 1; //Procedure index.
        table[procex].level = level;
        table[procex].addr = jmpaddr + 1;

        getNextToken();

        if(curr_token.type != semicolonsym)
            printparseerror(4);//error(4) for ; is missing before block is entered
        if (error == 1) return;

        getNextToken();
        block();
        if (error == 1) return;

        if(curr_token.type != semicolonsym)
            printparseerror(14);//error(14) for missing ending semicolon
        if (error == 1) return;

        getNextToken();
        emit(2, 0, 0);//emit RTN
    }
}

void statement()
{
    int aa = 0;
    if (error == 1) return;
    int symIdx;//set temp variable

    if(curr_token.type == identsym)//ident ":=" expression
    {
        symIdx = findsymbol(curr_token.name, 2);//find variable in symbol table and return index
        if((findsymbol(curr_token.name, 1) != -1 || findsymbol(curr_token.name, 3) != -1) && (symIdx == -1))
            printparseerror(6);//error(6) for identifier present is not a variable
        if (error == 1) return;

        if(symIdx == -1)
            printparseerror(19);//error(19) for cannot find corresponding identifier
        if (error == 1) return;

        getNextToken();

        //check for each assignsym
        if(curr_token.type != becomessym && curr_token.type != addassignsym && curr_token.type != subassignsym && curr_token.type != timesassignsym && curr_token.type != divassignsym && curr_token.type != modassignsym)
            printparseerror(5);//error(5) for := or addassign or subassign or timesassign or divassign or modassign missing
        if (error == 1) return;

        if(curr_token.type == becomessym)
            aa = 1;
        else
        {
            if(curr_token.type == addassignsym)
                aa = 2;
            else if(curr_token.type == subassignsym)
                aa = 3;
            else if(curr_token.type == timesassignsym)
                aa = 4;
            else if(curr_token.type == divassignsym)
                aa = 5;
            else if(curr_token.type == modassignsym)
                aa = 6;
            emit(3, level - table[symIdx].level, table[symIdx].addr);//emit LOD for assignsym
        }

        getNextToken();
        expression();
        if (error == 1) return;

        if(aa == 2)
            emit(2, 0, 2);//emit +
        else if(aa == 3)
            emit(2, 0, 3);//emit -
        else if(aa == 4)
            emit(2, 0, 4);//emit *
        else if(aa == 5)
            emit(2, 0, 5);//emit /
        else if(aa == 6)
            emit(2, 0, 6);//emit %

        emit(4, level - table[symIdx].level, table[symIdx].addr);//emit STO
    }
    else if(curr_token.type == callsym)//call ident
    {
        getNextToken();
        if(curr_token.type != identsym)
            printparseerror(7);//error(7) for identifier is missing
        if (error == 1) return;

        if((findsymbol(curr_token.name, 2) != -1) || (findsymbol(curr_token.name, 1) != -1))
            printparseerror(7);//error(7) for not a procedure
        if (error == 1) return;

        symIdx = findsymbol(curr_token.name, 3);
        if(symIdx == -1)
            printparseerror(19);//error(19) for cannot find corresponding identifier
        if (error == 1) return;

        if(table[symIdx].kind == 3)
            emit(5, level - table[symIdx].level, table[symIdx].addr * 3);//emit CAL

        getNextToken();
    }
    else if(curr_token.type == beginsym)//"begin" statement { ";" statement } "end"
    {
        getNextToken();
        statement();
        if (error == 1) return;

        do
        {
            if(curr_token.type != semicolonsym)
                printparseerror(15);//error(15) for semicolon is expected
            if (error == 1) return;

            getNextToken();
            statement();
            if (error == 1) return;

            if(curr_token.type == identsym || curr_token.type == readsym || curr_token.type == writesym || curr_token.type == beginsym || curr_token.type == callsym || curr_token.type == ifsym || curr_token.type == whilesym)
                printparseerror(15);//error(15) for end symbol is expected but found: identifier, read, write, begin, call, if, or while
            if (error == 1) return;
        }while(curr_token.type == semicolonsym);

        if((curr_token.type != endsym) && (curr_token.type != identsym) && (curr_token.type != readsym) && (curr_token.type != writesym) && (curr_token.type != beginsym) && (curr_token.type != callsym) && (curr_token.type != ifsym) && (curr_token.type != whilesym))
            printparseerror(16);//error(16) for end symbol is expected and neither end, identifier, read, write, begin, call, if, nor while

        if (error == 1) return;
        getNextToken();
    }
    else if(curr_token.type == ifsym)//"if" condition "then" statement ["else" statement]
    {
        getNextToken();
        condition();
        if (error == 1) return;

        int jpcIdx = cIndex;//set temp
        emit(8, 0, 0);//emit JPC

        if(curr_token.type != thensym)
            printparseerror(8);//error(8) for then not follow if
        if (error == 1) return;

        getNextToken();
        statement();
        if (error == 1) return;

        if(curr_token.type == elsesym)//if has else
        {
            int jmpIdx = cIndex;//set temp2
            emit(7, 0, 0);//emit JMP
            code[jpcIdx].m = cIndex * 3;//jpc address multiplied by 3 for temp

            getNextToken();
            statement();
            if (error == 1) return;

            code[jmpIdx].m = cIndex * 3;//jmp address multiplied by 3 for temp2
        }
        else
            code[jpcIdx].m = cIndex * 3;//only jpc address multiplied by 3
    }
    else if(curr_token.type == whilesym)//"while" condition "do" statement
    {
        int loopIdx = cIndex;//set temp to keep code index for loop

        getNextToken();
        condition();
        if (error == 1) return;

        int jpcIdx1 = cIndex;//set temp to keep code index for jpc
        emit(8, 0, 0);

        if(curr_token.type != dosym)
            printparseerror(9);//error(9) for current symbol is not do
        if (error == 1) return;

        getNextToken();
        statement();
        if (error == 1) return;

        emit(7, 0, loopIdx * 3);//emit JMP
        code[jpcIdx1].m = cIndex * 3;//save current code index in corresponding code address
    }
    else if(curr_token.type == readsym)//"read" ident
    {
        getNextToken();
        if(curr_token.type != identsym)
            printparseerror(6);//error(6) for identifier is missing
        if (error == 1) return;

        if(findsymbol(curr_token.name, 1) != -1 || findsymbol(curr_token.name, 3) != -1)
            printparseerror(6);//error(6) for identifier present is not a variable
        if (error == 1) return;

        int readIdx = findsymbol(curr_token.name, 2);//set temp for returned index
        if(readIdx == -1)
            printparseerror(19);//cannot find corresponding variable
        if (error == 1) return;

        emit(9, 0, 2);//emit READ
        emit(4, level - table[readIdx].level, table[readIdx].addr);//emit STO
        getNextToken();
    }
    else if(curr_token.type == writesym)//"write" expression
    {
        getNextToken();
        expression();
        if (error == 1) return;
        emit(9, 0, 1);//emit WRITE
    }
}

void condition()//expression  rel-op  expression
{

    if (error == 1) return;
    expression();
    if (error == 1) return;

    if((curr_token.type != eqlsym) && (curr_token.type != neqsym) && (curr_token.type != lessym) && (curr_token.type != leqsym) && (curr_token.type != gtrsym) && (curr_token.type != geqsym))
        printparseerror(10);//error(10) for not a relational operator
    if (error == 1) return;



    int relop = rel_op();//set temp to save rel_op return value

    getNextToken();
    expression();
    if (error == 1) return;

    //relop check for each emit execution
    if(relop == 9)
        emit(2, 0, 7);
    else if(relop == 10)
        emit(2, 0 ,8);
    else if(relop == 11)
        emit(2, 0, 9);
    else if(relop == 12)
        emit(2, 0, 10);
    else if(relop == 13)
        emit(2, 0, 11);
    else if(relop == 14)
        emit(2, 0, 12);
}

void expression()//["-"] term { ("+"|"-") term}
{
    if (error == 1) return;
    int addop;//set temp for add or minus operator

    if(curr_token.type == minussym)//neg
    {
        addop = curr_token.type;//save neg
        getNextToken();
        term();
        if (error == 1) return;

        if(addop == minussym)
            emit(2, 0, 1);//emit NEG
    }
    else
    {
        term();
        if (error == 1) return;
    }


    while(curr_token.type == plussym || curr_token.type == minussym)//following expression on add and subtract
    {
        addop = curr_token.type;//get corresponding type
        getNextToken();
        term();
        if (error == 1) return;

        if(addop == plussym)
            emit(2, 0, 2);//emit ADD
		else
			emit(2, 0, 3);//emit SUB
    }
    if(curr_token.type == lparentsym || curr_token.type == identsym || curr_token.type == numbersym)
        printparseerror(17);//error(17) for current token is (, identifier or number
    if (error == 1) return;
}

void term()//factor {("*"|"/"|”%”) factor}
{
    if (error == 1) return;
    int mulop;//set temp for multiply or divide or mod

    factor();
    if (error == 1) return;

    while(curr_token.type == multsym || curr_token.type == slashsym || curr_token.type == modsym)//loop for multiply, divide and mod
    {
        mulop = curr_token.type;//temp save current token
        getNextToken();
        factor();
        if (error == 1) return;

        if(mulop == multsym)
            emit(2, 0, 4);//emit MUL
        else if(mulop == slashsym)
            emit(2, 0, 5);//emit DIV
        else
            emit(2, 0, 6);//emit MOD
    }
}

void factor()//ident | number | "(" expression ")
{
    if (error == 1) return;
    //set temps for returned index
    int symIdx1;
    int symIdx2;
    int symIdx3;

    if(curr_token.type == identsym)
    {
        //get returned index
        symIdx1 = findsymbol(curr_token.name, 1);
        symIdx2 = findsymbol(curr_token.name, 2);
        symIdx3 = findsymbol(curr_token.name, 3);

        if(symIdx1 == -1 && symIdx2 == -1 && symIdx3 == -1)
            printparseerror(19);//cannot find corresponding identifier
        if (error == 1) return;

        if(symIdx3 != -1)//if procedure
            printparseerror(11);//error(11) for find a procedure
        if (error == 1) return;

        if(symIdx1 != -1 && symIdx2 == -1)
            emit(1, 0, table[symIdx1].value);//emit LIT
        else
            emit(3, level - table[symIdx2].level, table[symIdx2].addr);//emit LOD

        getNextToken();
    }
    else if(curr_token.type == numbersym)
    {
        emit(1, 0, curr_token.value);//emit LIT
        getNextToken();
    }
    else if(curr_token.type == lparentsym)
    {
        getNextToken();
        expression();
        if (error == 1) return;

        if(curr_token.type != rparentsym)
            printparseerror(12);//) is not found
        if (error == 1) return;
        getNextToken();
    }
    else
    {
        printparseerror(11);//error(11) for neither a number, an identifier, nor a (
        if (error == 1) return;
    }
}

int rel_op()//"="|“!="|"<"|"<="|">"|">=“
{
    //determine relational operator
    if(curr_token.type == eqlsym)
        return 9;
    else if(curr_token.type == neqsym)
        return 10;
    else if(curr_token.type == lessym)
        return 11;
    else if(curr_token.type == leqsym)
        return 12;
    else if(curr_token.type == gtrsym)
        return 13;
    else if(curr_token.type == geqsym)
        return 14;
    else
        return 0;
}

void getNextToken()//get next token from tokens array
{
    //let curr_token get each of current token's attribute
    curr_token.type = tokens[lIndex].type;
    strcpy(curr_token.name, tokens[lIndex].name);
    curr_token.value = tokens[lIndex].value;
    curr_token.error_type = tokens[lIndex].error_type;

    lIndex++;//increase lIndex to move to next token
}
