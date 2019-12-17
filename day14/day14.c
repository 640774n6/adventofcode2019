//
//  day14.c
//  day14
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

#define FUEL_ID     12052106
#define ORE_ID      51815
#define ORE_STORE   1000000000000

typedef struct term term;
struct term
{
    int64_t rid;
    int coefficient;
};

typedef struct reaction reaction;
struct reaction
{
    int64_t id;
    int64_t yield;
    int64_t available;
    int64_t createdCount;
    term **terms;
    int termsCount;
};

int64_t generate_id(char *s)
{
    int c = strlen(s);
    int64_t r = 0;
    for(int i = 0; i < c; i++)
    {
        int v = s[i] - 64;
        r += v * pow(10, i*2);
    }
    return r;
}

void get_pretty_id(int64_t id, char *pid)
{
    int i = 0;
    do
    {
        int64_t a = id % 10;
        id /= 10;
        int64_t b = (id % 10) * 10;
        id /= 10;
        char c = a + b + 64;
        pid[i++] = c;
    }
    while(id > 0);
    pid[i]='\0';
}

typedef struct node node;
struct node
{
    int64_t key;
    reaction *val;
    node *next;
};

typedef struct table table;
struct table
{
    int size;
    node **list;
};

table *createTable(int size)
{
    table *t = (table*)malloc(sizeof(table));
    t->size = size;
    t->list = (node**)malloc(sizeof(node*) * size);
    
    for(int i = 0; i < size; i++)
    { t->list[i] = NULL; }
    
    return t;
}

int hashCode(table *t, int64_t key)
{
    if(key < 0)
    { return -(key % t->size); }
    return key % t->size;
}

void tableInsert(table *t, int64_t key, reaction *val)
{
    int pos = hashCode(t, key);
    node *list = t->list[pos];
    node *newNode = (node*)malloc(sizeof(node));
    node *temp = list;
    while(temp)
    {
        if(temp->key == key)
        {
            temp->val = val;
            return;
        }
        temp = temp->next;
    }
    newNode->key = key;
    newNode->val = val;
    newNode->next = list;
    t->list[pos] = newNode;
}

reaction *tableLookup(struct table *t, int64_t key)
{
    int pos = hashCode(t, key);
    node *list = t->list[pos];
    node *temp = list;
    while(temp)
    {
        if(temp->key==key)
        { return temp->val; }
        temp = temp->next;
    }
    return NULL;
}

int getNextMultiple(double a, double b)
{ return ceil(a / b) * b; }

void create(table *rt, reaction *r, int c)
{
    int64_t yc = getNextMultiple(c, r->yield);
    int64_t rc = yc / r->yield;

    for(int i = 0; i < r->termsCount; i++)
    {
        term *t = r->terms[i];
        if(t->rid != ORE_ID)
        {
            reaction *tr = tableLookup(rt, t->rid);
            int64_t tc = t->coefficient * rc;
            if (tr->available < tc)
            { 
                int64_t d = tc - tr->available;
                create(rt, tr, d);
            }
            tr->available -= tc;
        }
    }
    r->available += yc;
    r->createdCount += rc;
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day14 <INPUT>\n");
        exit(0);
    }

    //Initialize reactions table
    table *reactionsTable = createTable(100);
    reaction **reactions = malloc(sizeof(reaction *));
    int reactionsCount = 0;

    //Parse reactions
    FILE *file = fopen(argv[1], "r");
    char line[256];
    while(fgets(line, sizeof(line), file))
    {
        //Parse reaction
        char expression[256];
        int yield = 0;
        char id[10];
        sscanf(line, "%[^=]=> %d %[^\n]", expression, &yield, id);

        //Create reaction
        reaction *r = malloc(sizeof(reaction));
        r->id = generate_id(id);
        r->yield = yield;
        r->available = 0;
        r->createdCount = 0;
        r->terms = malloc(sizeof(term *));
        r->termsCount = 0;

        //Add reaction
        tableInsert(reactionsTable, r->id, r);
        reactionsCount++;
        reactions = realloc(reactions, sizeof(reaction *) * reactionsCount);
        reactions[reactionsCount - 1] = r;

        //Split expression into terms
        char *rt = strtok(expression, ",");
        while (rt != NULL)
        {
            //Parse term
            int coefficient = 0;
            char trid[10];
            sscanf(rt, "%d %s", &coefficient, trid);

            //Create term
            term *t = malloc(sizeof(term));
            t->rid = generate_id(trid);
            t->coefficient = coefficient;

            //Add term to reaction
            r->termsCount++;
            r->terms = realloc(r->terms, sizeof(term *) * r->termsCount);
            r->terms[r->termsCount - 1] = t;

            //Next term
            rt = strtok(NULL, ",");
        }
    }
    fclose(file);

    int64_t oreCount = 0;
    int64_t fuelCount = -1;
    int64_t singleFuelCost = 0;
    do
    {
        oreCount = 0;
        reaction *fuel = tableLookup(reactionsTable, FUEL_ID);
        create(reactionsTable, fuel, 1);

        for(int i = 0; i < reactionsCount; i++)
        {
            reaction *r = reactions[i];
            term *t = r->termsCount == 1 ? r->terms[0] : NULL;
            if (t && t->rid == ORE_ID)
            { oreCount += r->createdCount * t->coefficient; }
        }
        
        fuelCount++;
        if (fuelCount == 0)
        { singleFuelCost = oreCount; }
    }
    while(oreCount <= ORE_STORE);
    printf("part1: Need %lld ORE required to make 1 FUEL\n", singleFuelCost);
    printf("part2: Can make %lld FUEL\n", fuelCount);

    return 0;
}
