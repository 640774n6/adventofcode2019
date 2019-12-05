//
//  day05.c
//  day05
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum mode mode;
enum mode
{
    mode_position = 0,
    mode_immediate = 1
};

typedef struct instruction instruction;
struct instruction
{
    int op;
    mode mode_1;
    mode mode_2;
    mode mode_3;
};

instruction parse_instruction(int code)
{
    instruction result;
    result.op = -1;
    result.mode_1 = mode_position;
    result.mode_2 = mode_position;
    result.mode_3 = mode_position;
    int i = 0; 
    do
    {
        int d = code % 10;
        switch(i)
        {
            case 0:
                result.op = d;
                break;
            case 1:
                result.op += d * 10;
                break;
            case 2:
                result.mode_1 = d;
                break;
            case 3:
                result.mode_2 = d;
                break;
            case 4:
                result.mode_3 = d;
                break;
            default:
                break;
        }
        code = code / 10;
        i++;
    }
    while (code > 0);
    return result;
}

int execute_codes(int *codes)
{
    //Execute code
    int i = 0;
    int halt = 0;
    while(!halt)
    {
        instruction instruction = parse_instruction(codes[i]);
        switch(instruction.op)
        {
            case 1:
            {
                //Add
                int a = codes[i+1];
                int b = codes[i+2];
                int c = codes[i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av + bv;
                i += 4;
                break;
            }
            case 2:
            {
                //Multiply
                int a = codes[i+1];
                int b = codes[i+2];
                int c = codes[i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av * bv;
                i += 4;
                break;
            }
            case 3:
            {
                //Input
                int a = codes[i+1];
                printf("> ");
                int input;
                if (scanf("%d", &input) != 1)
                { 
                    printf("error: failed to read integer input\n");
                    exit(-1);
                }
                codes[a] = input;
                i += 2;
                break;
            }
            case 4:
            {
                //Output
                int a = codes[i+1];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                printf("%d\n", av);
                i += 2;
                break;
            }
            case 5:
            {
                //Jump if true
                int a = codes[i+1];
                int b = codes[i+2];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                i = av != 0 ? bv : i + 3;
                break;
            }
            case 6:
            {
                //Jump if false
                int a = codes[i+1];
                int b = codes[i+2];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                i = av == 0 ? bv : i + 3;
                break;
            }
            case 7:
            {
                //Less than
                int a = codes[i+1];
                int b = codes[i+2];
                int c = codes[i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av < bv;
                i += 4;
                break;
            }
            case 8:
            {
                //Equals
                int a = codes[i+1];
                int b = codes[i+2];
                int c = codes[i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av == bv;
                i += 4;
                break;
            }
            case 99:
            {
                //Halt
                halt = 1;
                break;
            }
            default:
                //Unknown Operation
                printf("error: unknown op code: %d\n", instruction.op);
                exit(-1);
                break;
        }
    }
    return codes[0];
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day05 <INPUT>\n");
        exit(0);
    }

    //Initialize variables
    int *codes = malloc(sizeof(int));
    int codesCount = 0;
    
    //Parse codes
    FILE *file = fopen(argv[1], "r");
    int code = 0;
    int offset = 0;
    while(fscanf(file, "%d,", &code) == 1)
    {
        //Add code
        codesCount++;
        codes = realloc(codes, sizeof(int) * codesCount);
        codes[codesCount - 1] = code;
    }
    fclose(file);

    //Allocate working memory
    int memSize = sizeof(int) * codesCount;
    int *mem = malloc(memSize);
    
    //Load codes into memory and execute
    memcpy(mem, codes, memSize);
    execute_codes(mem);

    //Cleanup
    free(mem);
    free(codes);
    return 0;
}
