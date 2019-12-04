//
//  day04.c
//  day04
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int meetsPart1Criteria(int p)
{
    int increasing = 1;
    int twin = 0;
    do
    {
        int d = p % 10;
        p = p / 10;
        if(d < p % 10)
        {
            increasing = 0;
            break;
        }
        else if (d == p % 10)
        { twin = 1; }
    }
    while (p > 0);
    return increasing && twin;
}

int meetsPart2Criteria(int p)
{
    int increasing = 1;
    int twin = 0;
    int dup = -1;
    int dupCount = 0;
    do
    {
        int d = p % 10;
        p = p / 10;
        if(d < p % 10)
        {
            increasing = 0;
            break;
        }
        else if (d == p % 10)
        { 
            if (dup != d)
            { 
                twin = dupCount == 2;
                dup = d;
                dupCount = 2;
            }
            else
            { dupCount++; }
        }
    }
    while (p > 0);
    return increasing && (twin || dupCount == 2);
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day04 <INPUT>\n");
        exit(0);
    }
    
    //Initialize variables
    int rangeLow = 0;
    int rangeHigh = 0;
    
    //Parse range
    FILE *file = fopen(argv[1], "r");
    fscanf(file, "%d-%d", &rangeLow, &rangeHigh);
    fclose(file);

    //Count valid passwords in range
    int validPart1 = 0;
    int validPart2 = 0;
    for (int p = rangeLow; p <= rangeHigh; p++)
    { 
        validPart1 += meetsPart1Criteria(p); 
        validPart2 += meetsPart2Criteria(p);
    }
    printf("part1: valid passwords = %d\n", validPart1);
    printf("part2: valid passwords = %d\n", validPart2);

    //Done
    return 0;
}
