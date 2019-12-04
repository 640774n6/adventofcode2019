//
//  day02.c
//  day02
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SEARCH_OUTPUT 19690720

int execute_codes(int *codes, int n, int v)
{
    //Adjust values for noun and verb
    codes[1] = n;
    codes[2] = v;

    //Execute code
    int i = 0;
    int halt = 0;
    while(!halt)
    {
        switch(codes[i])
        {
            case 1:
            {
                //Add
                int a = codes[i+1];
                int b = codes[i+2];
                int c = codes[i+3];
                codes[c] = codes[a] + codes[b];
                i += 4;
                break;
            }
            case 2:
            {
                //Multiply
                int a = codes[i+1];
                int b = codes[i+2];
                int c = codes[i+3];
                codes[c] = codes[a] * codes[b];
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
                printf("error: unknown op code: %d\n", codes[i]);
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
        printf("usage: day02 <INPUT>\n");
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
    
    //Load codes into memory and execute part 1
    memcpy(mem, codes, memSize);
    int part1 = execute_codes(mem, 12, 2);
    printf("part1: output = %d\n", part1);

    //Search for correct noun and verb
    for(int i = 0; i <= 99; i++)
    {
        for (int j = 0; j <= 99; j++)
        {
            //Load codes into memory and execute with noun and verb
            memcpy(mem, codes, memSize);
            int output = execute_codes(mem, i, j);

            //If output matches
            if (output == SEARCH_OUTPUT)
            {
                //Calculate part 2
                int part2 = 100 * i + j;
                printf("part2: n = %d, v = %d, answer = %d\n", i, j, part2);
                break;
            }
        }
    }

    //Cleanup
    free(mem);
    free(codes);
    return 0;
}
