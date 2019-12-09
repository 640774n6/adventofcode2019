//
//  day07.c
//  day07
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

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

int execute_codes(int *codes, int *i, int input)
{
    //Execute code
    int halt = 0;
    int *in = &input;
    while(!halt)
    {
        instruction instruction = parse_instruction(codes[*i]);
        switch(instruction.op)
        {
            case 1:
            {
                //Add
                int a = codes[*i+1];
                int b = codes[*i+2];
                int c = codes[*i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av + bv;
                *i += 4;
                break;
            }
            case 2:
            {
                //Multiply
                int a = codes[*i+1];
                int b = codes[*i+2];
                int c = codes[*i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av * bv;
                *i += 4;
                break;
            }
            case 3:
            {
                //Input
                if (in == NULL)
                { return -3; }

                int a = codes[*i+1];
                codes[a] = *in;
                in = NULL;
                *i += 2;
                break;
            }
            case 4:
            {
                //Output
                int a = codes[*i+1];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                *i += 2;
                return av;
                break;
            }
            case 5:
            {
                //Jump if true
                int a = codes[*i+1];
                int b = codes[*i+2];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                *i = av != 0 ? bv : *i + 3;
                break;
            }
            case 6:
            {
                //Jump if false
                int a = codes[*i+1];
                int b = codes[*i+2];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                *i = av == 0 ? bv : *i + 3;
                break;
            }
            case 7:
            {
                //Less than
                int a = codes[*i+1];
                int b = codes[*i+2];
                int c = codes[*i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av < bv;
                *i += 4;
                break;
            }
            case 8:
            {
                //Equals
                int a = codes[*i+1];
                int b = codes[*i+2];
                int c = codes[*i+3];
                int av = instruction.mode_1 == mode_immediate ? a : codes[a];
                int bv = instruction.mode_2 == mode_immediate ? b : codes[b];
                codes[c] = av == bv;
                *i += 4;
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
    return -99;
}

void permutate(int *a, int n, int s, int **p, int *c)
{
    if (n == 1)
    {
        memcpy(p[*c], a, sizeof(int) * s);
        *c += 1;
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            permutate(a, n - 1, s, p, c);
            int m = (n % 2 == 1) ? 0 : i;
            int c = a[m];
            a[m] = a[n - 1];
            a[n - 1] = c;
        }
    }
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day07 <INPUT>\n");
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
    int *mem = malloc(sizeof(int) * codesCount);

    //Generate permutations of phases for part 1
    int part1Phases[5] = { 0, 1, 2, 3, 4 };
    int permutationsCount = 0;
    int **permutations = malloc(sizeof(int *) * 120);
    for(int i = 0; i < 120; i++)
    { permutations[i] = malloc(sizeof(int) * 5); }
    permutate(part1Phases, 5, 5, permutations, &permutationsCount);

    //Brute force max signal for part 1
    int maxSignalPart1 = 0;
    for (int i = 0; i < 120; i++)
    {
        int output = 0;
        for(int j = 0; j < 5; j++)
        {
            //Load codes into memory and execute
            int ip = 0;
            memcpy(mem, codes, sizeof(int) * codesCount);
            execute_codes(mem, &ip, permutations[i][j]);
            output = execute_codes(mem, &ip, output);
        }
        maxSignalPart1 = MAX(maxSignalPart1, output);
    }
    printf("part1: max signal = %d\n", maxSignalPart1);

    //Generate permutations of phases for part 2
    int part2Phases[5] = { 5, 6, 7, 8, 9 };
    permutationsCount = 0;
    permutate(part2Phases, 5, 5, permutations, &permutationsCount);

    //Setup state for each amp
    int amps_ip[5] = { 0, 0, 0, 0, 0 };
    int **amps_mem = malloc(sizeof(int *) * 5);
    for(int i = 0; i < 5; i++)
    { 
        amps_mem[i] = malloc(sizeof(int) * codesCount); 
        memcpy(amps_mem[i], codes, sizeof(int) * codesCount);
    }

    //Brute force max signal for part 2
    int maxSignalPart2 = 0;
    for (int i = 0; i < 120; i++)
    {
        //Reset amp states
        for(int j = 0; j < 5; j++)
        {
            amps_ip[j] = 0;
            memcpy(amps_mem[j], codes, sizeof(int) * codesCount);
            execute_codes(amps_mem[j], &amps_ip[j], permutations[i][j]);
        }

        //Run amps in feedback loop until one of them halts
        int output = 0;
        int halt = 0;
        while(!halt)
        {
            for(int j = 0; j < 5; j++)
            {
                int result = execute_codes(amps_mem[j], &amps_ip[j], output);
                if (result == -99)
                {
                    halt = 1;
                    break;
                }
                output = result;
            }
        }
        maxSignalPart2 = MAX(maxSignalPart2, output);
    }
    printf("part2: max signal = %d\n", maxSignalPart2);

    //Cleanup
    free(mem);
    free(codes);
    return 0;
}


