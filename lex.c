//Author name: Yuyang Zhang
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "compiler.h"

#define norw 13
#define imax 5
#define cmax 11

char *word[ ] = {"else", "begin", "end", "if", "then", "while", "do", "call", "const", "var", "procedure", "write", "read"};//reserved words

lexeme *lexical_analyzer(char *input)
{
    //variables
    lexeme *array = malloc(sizeof(lexeme) * MAX_ARRAY_SIZE);//store lexical units and token types
    char ch;
    char tempch1 = 0;
    char lex[cmax + 2], a[cmax + 2];//store lexeme
    int i, k, d, n;
    int j = 0, g = 0;
    long m;
    int sym;
    int inputI = 0;
    int error = 0;

	while(input[inputI] != '\0')
	{
	    //determine multiple characters
	    if(tempch1 != 0)
        {
            ch = tempch1;
            tempch1 = 0;
        }
        else
        {
            ch = input[inputI];
            inputI++;
        }

        if(ch == '\0')//end
            break;

        if(ch == '/' && input[inputI] == '*')//check comments and skip
        {
            inputI++;
            do
            {
                ch = input[inputI];
                inputI++;;
                if(ch == '\0')
                {
                    g = 1;
                    break;
                }
            }
            while(!(ch == '*' && input[inputI] == '/'));//determine comments end

            inputI++;
            ch = input[inputI];
            inputI++;
        }

        if(ch == '\0')//end of input
            break;

        if(ch == ' ' || ch == 9 || ch == 10 || ch == '\r')//skip space, tab and newline
            continue;
        else
        {
            if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')  || (ch >= '0' && ch <= '9'))//check variable names or reserved words
            {
                k = 0;
                d = 0;
                n = 0;
                do
                {
                    if(k < cmax)//get name
                        a[k] = ch;
                    k++;
                    ch = input[inputI];
                    inputI++;
                }
                while((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z'));//determine if char or num

                tempch1 = ch;
                if(input[inputI] == '\0')//check for end and back to previous position
                {
                    tempch1 = 0;
                    inputI--;
                }

                a[k] = '\0';

                if((a[0] >= 'a' && a[0] <= 'z') || (a[0] >= 'A' && a[0] <= 'Z'))//check name
                {
                    d = 1;
                    if(k > cmax)//variable name cannot have more than 11 characters
                    {
                        a[0] = '$';
                        a[1] = '\0';
                        sym = 0;
                        array[j].error_type = 1;
                    }
                    else
                    {
                        strcpy(lex, a);
                        for(i = 0; i < norw; i++)
                        {
                            if(strcmp(lex, word[i]) == 0)//set reserved words tokens
                            {
                                switch(i)//find corresponding sym
                                {
                                    case 0: sym = elsesym; break;
                                    case 1: sym = beginsym; break;
                                    case 2: sym = endsym; break;
                                    case 3: sym = ifsym; break;
                                    case 4: sym = thensym; break;
                                    case 5: sym = whilesym; break;
                                    case 6: sym = dosym; break;
                                    case 7: sym = callsym; break;
                                    case 8: sym = constsym; break;
                                    case 9: sym = varsym; break;
                                    case 10: sym = procsym; break;
                                    case 11: sym = writesym; break;
                                    case 12: sym = readsym; break;
                                }
                                n = 1;
                            }
                        }
                        if(n == 0)
                            sym = identsym;
                    }
                }

                if(a[0] >= '0' && a[0] <= '9')//check for num
                {
                    for(i = 1; i < k; i++)
                    {
                        if((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= 'A' && a[i] <= 'Z'))//variable cannot start with number
                        {
                            a[0] = '$';
                            a[1] = '\0';
                            sym = 0;
                            array[j].error_type = 2;
                            d = 1;
                        }
                    }
                    if(d == 0)
                    {
                        if(k > imax)//number length cannot more than 5 digits
                        {
                            a[0] = '$';
                            a[1] = '\0';
                            array[j].error_type = 3;
                            sym = 0;
                        }
                        m = 0;
                        sym = numbersym;
                        for(i = 0; i < k; i++)//string change to number
                            m = 10 * m + a[i] - '0';
                        array[j].value = m;
                    }
                }
            }
            else//get ch from input
            {
                a[0] = ch;

                if(ch == ':')//check if :=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = becomessym;
                    }
                    else
                    {
                        a[0] = '$';
                        a[1] = '\0';
                        sym = 0;
                        array[j].error_type = 4;
                    }
                }
                else if(ch == '!')//check for !=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = neqsym;
                    }
                    else
                    {
                        a[0] = '$';
                        a[1] = '\0';
                        array[j].error_type = 4;
                        sym = 0;
                    }
                }
                else if(ch == '<')//check for < and <=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = leqsym;
                    }
                    else
                    {
                        a[1] = '\0';
                        sym = lessym;
                        tempch1 = ch;
                    }
                }
                else if(ch == '>')//check for >= and >
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = geqsym;
                    }
                    else
                    {
                        a[1] = '\0';
                        sym = gtrsym;
                        tempch1 = ch;
                    }
                }
                else if(ch == '+')//check for + and +=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = addassignsym;
                    }
                    else
                    {
                        a[1] = '\0';
                        sym = plussym;
                        tempch1 = ch;
                    }
                }
                else if(ch == '-')//check for - and -=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = subassignsym;
                    }
                    else
                    {
                        a[1] = '\0';
                        sym = minussym;
                        tempch1 = ch;
                    }
                }
                else if(ch == '*')//check for * and *=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = timesassignsym;
                    }
                    else
                    {
                        a[1] = '\0';
                        sym = multsym;
                        tempch1 = ch;
                    }
                }
                else if(ch == '/')//check for / and /=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = divassignsym;
                    }
                    else
                    {
                        a[1] = '\0';
                        sym = slashsym;
                        tempch1 = ch;
                    }
                }
                else if(ch == '%')//check for % and %=
                {
                    ch = input[inputI];
                    inputI++;
                    if(ch == '=')
                    {
                        a[1] = ch;
                        a[2] = '\0';
                        sym = modassignsym;
                    }
                    else
                    {
                        a[1] = '\0';
                        sym = modsym;
                        tempch1 = ch;
                    }
                }
                else//single digit character
                {
                    a[1] = '\0';

                    if(ch == '=')
                       sym = eqlsym;
                    else if(ch == '(')
                       sym = lparentsym;
                    else if(ch == ')')
                       sym = rparentsym;
                    else if(ch == ',')
                       sym = commasym;
                    else if(ch == ';')
                       sym = semicolonsym;
                    else if(ch == '.')
                       sym = periodsym;
                    else
                    {
                        a[0] = '$';
                        a[1] = '\0';
                        sym = 0;
                        array[j].error_type = 4;//check for invalid symbol
                    }
                }
            }

            //copy to array
            strcpy(array[j].name, a);
            array[j].type = sym;
            j++;
        }
    }

    //output
    int count = j;
    printf("%s\n", "Lexeme Table");
    printf("%s\t%s\n", "lexeme", "token type");
    for(i = 0; i < count; i++)
    {
        if(strcmp(array[i].name, "$") == 0)//check error
        {
            printf("Lexical Analyzer Error: ");
            switch(array[i].error_type)//print each error
            {
                case 1: printf("Identifier length exceeds 11 characters!\n"); break;
                case 2: printf("Variable identifier does not start with letter!\n"); break;
                case 3: printf("Number length exceeds 5 digits!\n"); break;
                case 4: printf("Invalid symbol!\n"); break;
           }
           error = 1;
        }
        else
            printf("%11s %d\n", array[i].name, array[i].type);
    }
   if(g == 1)//never ending comment
   {
       error = 1;
       printf("Lexical Analyzer Error: Neverending comment!\n");
   }

    printf("\n");
    printf("%s\n", "Token List");
    for(i = 0; i < count; i++)//output
    {
        if(strcmp(array[i].name, "$") == 0)
           continue;
        else
        {
            printf("%d  ", array[i].type);
            if(array[i].type == 2 || array[i].type == 3)
                printf("%s  ", array[i].name);
        }
    }
    printf("\n\n");

    if(error == 1)//if error free array
    {
        free(array);
        array = NULL;
    }
    return array;
}
