//
//  day10.c
//  day10
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

typedef struct asteroid asteroid;
struct asteroid
{
    int id;
    int x;
    int y;
    int alive;
    int distance;
    double angle;
};

double get_angle(asteroid *from, asteroid *to)
{
    double angle = ((180 / M_PI) * atan2(to->y - from->y, to->x - from->x)) + 90.0;
    return angle < 0 ? angle + 360.0 : angle;
}

int asteroid_compare(const void *a, const void *b)
{
    int result;
    asteroid *a1 = *(asteroid **)a;
    asteroid *a2 = *(asteroid **)b;
    if (a1->angle < a2->angle)
    { result = -1; }
    else if (a1->angle > a2->angle)
    { result = 1; }
    else
    {
        if (a1->distance < a2->distance)
        { result = -1; }
        else if (a1->distance > a2->distance)
        { result = 1; }
        else
        { result = 0; }
    }
    return result;
}

void asteroid_acquire_targets(asteroid *from, asteroid **asteroids, int asteroidsCount)
{
    //Iterate through asteroids
    for(int i = 0; i < asteroidsCount; i++)
    {
        //Get asteroid to check and skip if it matches from
        asteroid *ta = asteroids[i];
        if (from->id == ta->id)
        { continue; }

        //Calculate angle and distance to target asteroid
        ta->distance = abs(from->x - ta->x) + abs(from->y - ta->y);
        ta->angle = get_angle(from, ta);
    }

    //Sort asteroids by angle then distance
    qsort(asteroids, asteroidsCount, sizeof(asteroid *), asteroid_compare);
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day10 <INPUT>\n");
        exit(0);
    }

    //Initialize asteroids
    asteroid **asteroids = malloc(sizeof(asteroid *));
    int asteroidsCount = 0;

    //Parse asteroids
    FILE *file = fopen(argv[1], "r");
    int yCount = 0;
    char line[256];
    while(fgets(line, sizeof(line), file))
    {
        int xCount = 0;
        for(char *c = line; *c != '\n'; c++)
        {
            if (*c == '#')
            {
                //Create asteroid
                asteroid *a = malloc(sizeof(asteroid));
                a->id = asteroidsCount;
                a->x = xCount;
                a->y = yCount;
                a->alive = 1;
                a->distance = 0;
                a->angle = 0;

                //Add asteroid
                asteroidsCount++;
                asteroids = realloc(asteroids, sizeof(asteroid *) * asteroidsCount);
                asteroids[asteroidsCount - 1] = a;
            }
            xCount++;
        }
        yCount++;
    }
    fclose(file);

    //Initialize copy of asteroids for targets
    asteroid **targets = malloc(sizeof(asteroid *) * asteroidsCount);
    memcpy(targets, asteroids, sizeof(asteroid *) * asteroidsCount);
    
    //Find best asteroid for monitoring
    asteroid *best = NULL;
    int bestCount = 0;
    for(int i = 0; i < asteroidsCount; i++)
    {
        //Get test asteroid and acquire targets
        asteroid *a = asteroids[i];
        asteroid_acquire_targets(a, targets, asteroidsCount);

        //Count visible asteroids
        int count = 0;
        double lastAngle = -1;
        for(int j = 0; j < asteroidsCount; j++)
        {
            //Get asteroid to check and skip if it matches test
            asteroid *ta = targets[j];
            if (a->id == ta->id)
            { continue; }

            //If this is a different angle than last
            if (lastAngle != ta->angle)
            {
                //Increment count and set last angle
                count++;
                lastAngle = ta->angle;
            }
        }

        //Update best asteroid
        if (!best || count > bestCount)
        { 
            best = a; 
            bestCount = count;
        }
    }
    printf("part1: best location for monitoring (%d,%d) can see %d asteroids\n", best->x, best->y, bestCount);

    //Acquire targets for best asteroid
    asteroid_acquire_targets(best, targets, asteroidsCount);

    //Kill all asteroids until lucky is found
    int killedAsteroids = 0;
    double lastKilledAngle = -1.0;
    asteroid *lucky = NULL;
    while(!lucky)
    {
        for (int i = 0; i < asteroidsCount; i++)
        {
            //Get asteroid to check and skip if it matches best
            asteroid *ta = targets[i];
            if (best->id == ta->id || !ta->alive)
            { continue; }
            
            //If this is a different angle than last
            if (lastKilledAngle != ta->angle)
            {
                //Kill target and set last killed angle
                ta->alive = 0;
                killedAsteroids++;
                lastKilledAngle = ta->angle;

                //Ding! ding! We have a winner!
                if (killedAsteroids == 200)
                {
                    lucky = ta;
                    break;
                }
            }
        }
    }
    printf("part2: lucky = %d\n", (lucky->x * 100) + lucky->y);
    
    //Cleanup
    for(int i = 0; i < asteroidsCount; i++)
    { free(asteroids[i]); }
    free(asteroids);
    free(targets);
    return 0;
}
