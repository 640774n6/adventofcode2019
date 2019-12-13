//
//  day12.c
//  day12
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct delta delta;
struct delta
{
    int c;
    int d;
    int f;
    int l;
};

typedef struct axis axis;
struct axis
{
    int p;
    int v;
    delta d;
};

typedef struct moon moon;
struct moon
{
    axis x;
    axis y;
    axis z;
};

int64_t lcm(int64_t a, int64_t b)
{
    int64_t gcd = 0;
    for (int i = 1; i <= a && i <= b; i++) 
    {
        if (a % i == 0 && b % i == 0)
        { gcd = i; }
    }
    return (a * b) / gcd;
}

void axis_apply_gravity(axis *a, axis *b)
{
    if (a->p < b->p)
    {
        a->v++;
        b->v--;
    }
    else if (a->p > b->p)
    {
        a->v--;
        b->v++;
    }
}

void moons_apply_gravity(moon **moons, int moonsCount)
{
    for(int i = 0; i < moonsCount - 1; i++)
    {
        moon *a = moons[i];
        for(int j = i + 1; j < moonsCount; j++)
        {
            moon *b = moons[j];
            axis_apply_gravity(&a->x, &b->x);
            axis_apply_gravity(&a->y, &b->y);
            axis_apply_gravity(&a->z, &b->z);
        }
    }
}

void moons_apply_velocity(moon **moons, int moonsCount)
{
    for(int i = 0; i < moonsCount; i++)
    {
        moon *m = moons[i];
        m->x.p += m->x.v;
        m->y.p += m->y.v;
        m->z.p += m->z.v;
    }
}

int moons_total_energy(moon **moons, int moonsCount)
{
    int total = 0;
    for(int i = 0; i < moonsCount; i++)
    {
        moon *m = moons[i];
        int p = llabs(m->x.p) + llabs(m->y.p) + llabs(m->z.p);
        int k = llabs(m->x.v) + llabs(m->y.v) + llabs(m->z.v);
        total += k * p;
    }
    return total;
}

int axis_compare(axis a, axis b)
{ return a.p == b.p && a.v == b.v; }

void delta_update(delta *delta, int s)
{
    int d = s - delta->l;
    delta->c += delta->c == d ? 0 : d;
    if (delta->f == 0)
    { delta->f = d; }
    else if (delta->f == d)
    { delta->d = delta->c; }
    delta->l = s;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day12 <INPUT>\n");
        exit(0);
    }

    //Initialize moons
    moon **moons = malloc(sizeof(moon *));
    int moonsCount = 0;

    //Parse moons
    FILE *file = fopen(argv[1], "r");
    int x = 0;
    int y = 0;
    int z = 0;
    while(fscanf(file, "<x=%d, y=%d, z=%d>\n", &x, &y, &z) == 3)
    {
        //Create moon
        moon *m = malloc(sizeof(moon));
        memset(m, 0, sizeof(moon));
        m->x.p = x;
        m->y.p = y;
        m->z.p = z;

        //Add moon
        moonsCount++;
        moons = realloc(moons, sizeof(moon *) * moonsCount);
        moons[moonsCount - 1] = m;
    }
    fclose(file);

    //Make copy for simulations
    moon **moonsSim = malloc(sizeof(moon *) * moonsCount);
    for(int i = 0; i < moonsCount; i++)
    {
        moonsSim[i] = malloc(sizeof(moon)); 
        memcpy(moonsSim[i], moons[i], sizeof(moon));
    }

    //Find total energy after 1000 steps
    for(int i = 0; i < 1000; i++)
    {
        //Apply gravity to velocity and velocity to position
        moons_apply_gravity(moonsSim, moonsCount);
        moons_apply_velocity(moonsSim, moonsCount);
    }

    //Calculate total energy
    int energy = moons_total_energy(moonsSim, moonsCount);
    printf("part1: total energy = %d\n", energy);

    //Reset simulation copy
    for(int i = 0; i < moonsCount; i++)
    { memcpy(moonsSim[i], moons[i], sizeof(moon)); }

    //Find number of steps until repeat
    int s = 0;
    int xd = 0;
    int yd = 0;
    int zd = 0;
    for(;;)
    {
        //Apply gravity to velocity and velocity to position and increment the step
        moons_apply_gravity(moonsSim, moonsCount);
        moons_apply_velocity(moonsSim, moonsCount);
        s++;

        //Update deltas on a per axis per moon basis
        int all = 1;
        for(int i = 0; i < moonsCount; i++)
        {
            //Get simulation moon and initial moon for comparison
            moon *a = moonsSim[i];
            moon *b = moons[i];

            //If moon has not found x delta and x matches initial; Update x delta
            if (a->x.d.d == 0 && axis_compare(a->x, b->x))
            { delta_update(&a->x.d, s); }

            //If moon has not found y delta and y matches initial; Update y delta
            if (a->y.d.d == 0 && axis_compare(a->y, b->y))
            { delta_update(&a->y.d, s); }

            //If moon has not found z delta and z matches initial; Update z delta
            if (a->z.d.d == 0 && axis_compare(a->z, b->z))
            { delta_update(&a->z.d, s); }

            //Update flag indicating all axes deltas have been found for moon
            all = all && a->x.d.d != 0 && a->y.d.d != 0 && a->z.d.d != 0;
        }

        //If all axes deltas have been found for all moons; Break
        if (all)
        { break; }
    }

    //Find max delta for each axis
    for(int i = 0; i < moonsCount; i++)
    {
        moon *m = moonsSim[i];
        xd = MAX(xd, m->x.d.d);
        yd = MAX(yd, m->y.d.d);
        zd = MAX(zd, m->z.d.d);
    }

    //Calculate steps until repeat by finding lowest common multiple of the 3 max axis deltas
    int64_t steps = lcm(xd, lcm(yd, zd));
    printf("part2: %lld steps until repeat\n", steps);

    //Cleanup
    for (int i = 0; i < moonsCount; i++)
    {
        free(moonsSim[i]);
        free(moons[i]);
    }
    free(moonsSim);
    free(moons);
    return 0;
}
