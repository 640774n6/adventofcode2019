//
//  day03.c
//  day03
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct point point;
struct point
{
    int x;
    int y;
};

void updatePosition(point *point, char direction, int offset)
{
    switch(direction)
    {
        case 'U':
            point->y += offset;
            break;
        case 'D':
            point->y -= offset;
            break;
        case 'L':
            point->x -= offset;
            break;
        case 'R':
            point->x += offset;
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day03 <INPUT>\n");
        exit(0);
    }
    
    int xMin = INT_MAX;
    int xMax = 0;
    int yMin = INT_MAX;
    int yMax = 0;

    //Determine grid size and origin
    FILE *file = fopen(argv[1], "r");
    do
    {
        point position;
        position.x = 0;
        position.y = 0;
        char direction = 0;
        int offset = 0;
        while (fscanf(file, "%c%d,", &direction, &offset) == 2)
        {
            updatePosition(&position, direction, offset);
            xMin = MIN(xMin, position.x);
            xMax = MAX(xMax, position.x);
            yMin = MIN(yMin, position.y);
            yMax = MAX(yMax, position.y);
        }
    }
    while(!feof(file));
    fseek(file, 0, SEEK_SET);

    int width = abs(xMax - xMin) + 1;
    int height = abs(yMax - yMin) + 1;
    int originX = abs(xMin);
    int originY = abs(yMin);

    int **grid = malloc(sizeof(int *) * width);
    for(int x = 0; x < width; x++)
    {
        grid[x] = malloc(sizeof(int) * height);
        memset(grid[x], 0, sizeof(int) * height);
    }
    grid[originX][originY] = -1;

    int distanceMin = INT_MAX;
    int stepsMin = INT_MAX;
    int pass = 1;
    do
    {
        point position;
        position.x = originX;
        position.y = originY;
        char direction = 0;
        int offset = 0;
        int steps = 0;
        while (fscanf(file, "%c%d,", &direction, &offset) == 2)
        {
            for (int i = 0; i < offset; i++)
            {
                switch (direction)
                {
                    case 'U':
                        position.y++;
                        break;
                    case 'D':
                        position.y--;
                        break;
                    case 'L':
                        position.x--;
                        break;
                    case 'R':
                        position.x++;
                        break;
                    default:
                        break;
                }
                steps++;

                if (pass == 1 && grid[position.x][position.y] == 0)
                { 
                    grid[position.x][position.y] = steps; 
                }
                else if (pass == 2 && grid[position.x][position.y] != 0)
                { 
                    int distance = abs(originX - position.x) + abs(originY - position.y);
                    distanceMin = MIN(distanceMin, distance);
                    int stepsWire1 = grid[position.x][position.y];
                    stepsMin = MIN(stepsMin, stepsWire1 + steps);
                    grid[position.x][position.y] = 2;
                }
            }
        }
        pass++;
    }
    while(!feof(file));
    printf("origin: %d, %d @ %dx%d\n", originX, originY, width, height);
    printf("part1: closest intersection distance = %d\n", distanceMin);
    printf("part2: shortest steps = %d\n", stepsMin);

    return 0;
}
