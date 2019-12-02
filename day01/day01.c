//
//  day01.c
//  day01
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int calculate_fuel(int mass)
{ 
    //Divide mass by 3, round down, subtract 2
    int fuel = floor(mass / 3) - 2;

    //Account for negative
    return fuel < 0 ? 0 : fuel;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day01 <INPUT>\n");
        exit(0);
    }
    
    //Initialize variables
    int totalPart1 = 0;
    int totalPart2 = 0;
    
    //Parse masses
    FILE *file = fopen(argv[1], "r");
    int mass = 0;
    while(fscanf(file, "%d", &mass) == 1)
    {
        //Calculate initial fuel for mass and add it to total
        int massFuel = calculate_fuel(mass);
        totalPart1 += massFuel;
        totalPart2 += massFuel;

        //Calculate fuel for fuel
        while(massFuel > 0)
        {
            //Calculate fuel and add it to total
            massFuel = calculate_fuel(massFuel);
            totalPart2 += massFuel;
        }
    }
    fclose(file);
    printf("part1: total = %d\n", totalPart1);
    printf("part2: total = %d\n", totalPart2);
    return 0;
}
