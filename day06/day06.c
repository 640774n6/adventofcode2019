//
//  day06.c
//  day06
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct satellite satellite;
struct satellite
{
    int id;
    satellite *body;
};

int generate_id(char *s)
{
    int id = 0;
    for(int i = 0; i < 3; i++)
    {
        int v = (int)s[i];
        id += v * pow(10, i*2);
    }
    return id;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day06 <INPUT>\n");
        exit(0);
    }

    //Initialize variables
    satellite **lookup = malloc(sizeof(satellite *) * 1000000);
    memset(lookup, 0, sizeof(satellite *) * 1000000);
    
    satellite **sats = malloc(sizeof(satellite *));
    int satsCount = 0;
    
    //Parse orbits
    FILE *file = fopen(argv[1], "r");
    char body_sid[4];
    char sat_sid[4];
    while(fscanf(file, "%3s)%3s", body_sid, sat_sid) == 2)
    {
        //Add body if it doesn't exist
        int body_id = generate_id(body_sid);
        satellite *body = lookup[body_id];
        if (body == NULL)
        {
            body = malloc(sizeof(satellite)); 
            body->id = body_id;
            body->body = NULL;

            satsCount++;
            sats = realloc(sats, sizeof(satellite *) * satsCount);
            sats[satsCount - 1] = body;

            lookup[body_id] = body;
        }

        //Add satellite if it doesn't exist
        int sat_id = generate_id(sat_sid);
        satellite *sat = lookup[sat_id];
        if (sat == NULL)
        {
            sat = malloc(sizeof(satellite));
            sat->id = sat_id;
            sat->body = NULL;

            satsCount++;
            sats = realloc(sats, sizeof(satellite *) * satsCount);
            sats[satsCount - 1] = sat;

            lookup[sat_id] = sat;
        }

        //Add satellite to body
        sat->body = body;
    }
    fclose(file);

    //Count orbits
    int orbits = 0;
    for(int i = 0; i < satsCount; i++)
    {  
        int count = 0;
        satellite *sat = sats[i];
        while(sat->body != NULL)
        {
            count++;
            sat = sat->body;
        }
        orbits += count;
    }
    printf("part1: %d orbits\n", orbits);

    //Count distance from YOU to SAN
    int you_id = generate_id("YOU");
    int san_id = generate_id("SAN");
    satellite *you = lookup[you_id];
    int you_jumps = 0;
    int jumps = 0;
    while(you->body != NULL)
    {
        satellite *san = lookup[san_id];
        int san_jumps = 0;
        while(san->body != NULL)
        {
            //Found common orbit
            if (you->body->id == san->body->id)
            { 
                jumps = you_jumps + san_jumps;
                break;
            }
            san = san->body;
            san_jumps++;
        }

        //Jump match found
        if (jumps > 0)
        { break; }

        you = you->body;
        you_jumps++;
    }
    printf("part2: %d orbital jumps\n", jumps);

    //Cleanup
    for(int i = 0; i < satsCount; i++)
    { free(sats[i]); }
    free(sats);
    free(lookup);
    return 0;
}
