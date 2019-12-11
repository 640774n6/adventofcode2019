//
//  day11.c
//  day11
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum status status;
enum status
{
    status_idle = 0,
    status_need_input = 1,
    status_output = 2,
    status_halt = 3,
    status_error = -1
};

typedef struct state state;
struct state
{
    int64_t ms;
    int64_t *m;
    int64_t i;
    int64_t r;
    status s;
};

typedef enum mode mode;
enum mode
{
    mode_position = 0,
    mode_immediate = 1,
    mode_relative = 2
};

typedef struct instruction instruction;
struct instruction
{
    int64_t op;
    mode m1;
    mode m2;
    mode m3;
};

int64_t get_mode_read(mode mode, state *state, int64_t i)
{
    int64_t v = 0;
    switch(mode)
    {
        case mode_position:
            v = state->m[i];
            break;
        case mode_immediate:
            v = i;
            break;
        case mode_relative:
            v = state->m[state->r + i];
            break;
        default:
            break;
    }
    return v;
}

int64_t get_mode_write(mode mode, state *state, int64_t i)
{
    int64_t v = 0;
    switch(mode)
    {
        case mode_position:
        case mode_immediate:
            v = i;
            break;
        case mode_relative:
            v = state->r + i;
            break;
        default:
            break;
    }
    return v;
}

instruction parse_instruction(int64_t code)
{
    instruction result;
    result.op = -1;
    result.m1 = mode_position;
    result.m2 = mode_position;
    result.m3 = mode_position;
    int64_t i = 0; 
    do
    {
        int64_t d = code % 10;
        switch(i)
        {
            case 0:
                result.op = d;
                break;
            case 1:
                result.op += d * 10;
                break;
            case 2:
                result.m1 = d;
                break;
            case 3:
                result.m2 = d;
                break;
            case 4:
                result.m3 = d;
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

int64_t execute_state(state *state, int64_t state_in)
{
    //Execute codes
    int64_t *input = &state_in;
    for(;;)
    {
        instruction instruction = parse_instruction(state->m[state->i]);
        switch(instruction.op)
        {
            case 1:
            {
                //Add
                int64_t a = state->m[state->i+1];
                int64_t b = state->m[state->i+2];
                int64_t c = state->m[state->i+3];
                int64_t av = get_mode_read(instruction.m1, state, a);
                int64_t bv = get_mode_read(instruction.m2, state, b);
                int64_t cv = get_mode_write(instruction.m3, state, c);
                state->m[cv] = av + bv;
                state->i += 4;
                break;
            }
            case 2:
            {
                //Multiply
                int64_t a = state->m[state->i+1];
                int64_t b = state->m[state->i+2];
                int64_t c = state->m[state->i+3];
                int64_t av = get_mode_read(instruction.m1, state, a);
                int64_t bv = get_mode_read(instruction.m2, state, b);
                int64_t cv = get_mode_write(instruction.m3, state, c);
                state->m[cv] = av * bv;
                state->i += 4;
                break;
            }
            case 3:
            {
                //Input
                if (input == NULL)
                {
                    state->s = status_need_input;
                    return 0;
                }
                int64_t a = state->m[state->i+1];
                int64_t av = get_mode_write(instruction.m1, state, a);
                state->m[av] = *input;
                input = NULL;
                state->i += 2;
                break;
            }
            case 4:
            {
                //Output
                int64_t a = state->m[state->i+1];
                int64_t av = get_mode_read(instruction.m1, state, a);
                state->i += 2;
                state->s = status_output;
                return av;
                break;
            }
            case 5:
            {
                //Jump if true
                int64_t a = state->m[state->i+1];
                int64_t b = state->m[state->i+2];
                int64_t av = get_mode_read(instruction.m1, state, a);
                int64_t bv = get_mode_read(instruction.m2, state, b);
                state->i = av != 0 ? bv : state->i + 3;
                break;
            }
            case 6:
            {
                //Jump if false
                int64_t a = state->m[state->i+1];
                int64_t b = state->m[state->i+2];
                int64_t av = get_mode_read(instruction.m1, state, a);
                int64_t bv = get_mode_read(instruction.m2, state, b);
                state->i = av == 0 ? bv : state->i + 3;
                break;
            }
            case 7:
            {
                //Less than
                int64_t a = state->m[state->i+1];
                int64_t b = state->m[state->i+2];
                int64_t c = state->m[state->i+3];
                int64_t av = get_mode_read(instruction.m1, state, a);
                int64_t bv = get_mode_read(instruction.m2, state, b);
                int64_t cv = get_mode_write(instruction.m3, state, c);
                state->m[cv] = av < bv;
                state->i += 4;
                break;
            }
            case 8:
            {
                //Equals
                int64_t a = state->m[state->i+1];
                int64_t b = state->m[state->i+2];
                int64_t c = state->m[state->i+3];
                int64_t av = get_mode_read(instruction.m1, state, a);
                int64_t bv = get_mode_read(instruction.m2, state, b);
                int64_t cv = get_mode_write(instruction.m3, state, c);
                state->m[cv] = av == bv;
                state->i += 4;
                break;
            }
            case 9:
            {
                int64_t a = state->m[state->i+1];
                int64_t av = get_mode_read(instruction.m1, state, a);
                state->r += av;
                state->i += 2;
                break;
            }
            case 99:
            {
                //Halt
                state->s = status_halt;
                return state->m[0];
                break;
            }
            default:
                //Error
                printf("error: unknown opcode %lld\n", instruction.op);
                state->s = status_error;
                return instruction.op;
                break;
        }
    }
}

state *alloc_state(int ms)
{
    state *state = malloc(sizeof(state));
    state->ms = ms;
    state->m = malloc(sizeof(int64_t) * ms);
    state->i = 0;
    state->r = 0;
    state->s = status_idle;
    return state;
}

void load_state(state *state, int64_t *c, int s)
{
    state->i = 0;
    state->r = 0;
    state->s = status_idle;
    memset(state->m, 0, sizeof(int64_t) * state->ms);
    memcpy(state->m, c, sizeof(int64_t) * s);
}

void free_state(state *state)
{
    free(state->m);
    free(state);
}

typedef struct bot bot;
struct bot
{
    int x;
    int y;
    int b;
};

void bot_turn(bot *bot, int turn)
{
    bot->b += turn == 0 ? -90 : 90;
    if (bot->b < 0)
    { bot->b += 360; }
    else if (bot->b >= 360)
    { bot->b -= 360; }
}

void bot_move(bot *bot, int delta)
{
    switch(bot->b)
    {
        case 0:
            bot->y -= delta;
            break;
        case 90:
            bot->x += delta;
            break;
        case 180:
            bot->y += delta;
            break;
        case 270:
            bot->x -= delta;
            break;
        default:
            break;
    }
}

void bot_paint(bot *bot, int **map, int c)
{ map[bot->y][bot->x] = c; }

int bot_look(bot *bot, int **map)
{ return map[bot->y][bot->x]; }

void print_map(int **map, int w, int h)
{
    for(int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            char c = map[y][x] == 0 ? '.' : '#';
            printf("%c", c);
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day11 <INPUT>\n");
        exit(0);
    }

    //Initialize codes
    int64_t *codes = malloc(sizeof(int64_t));
    int codesCount = 0;
    
    //Parse codes
    FILE *file = fopen(argv[1], "r");
    int64_t code = 0;
    while(fscanf(file, "%lld,", &code) == 1)
    {
        //Add code
        codesCount++;
        codes = realloc(codes, sizeof(int64_t) * codesCount);
        codes[codesCount - 1] = code;
    }
    fclose(file);

    //Initialize map
    int width = 100;
    int height = 100;
    int **map = malloc(sizeof(int *) * height);
    for(int i = 0; i < height; i++)
    { 
        map[i] = malloc(sizeof(int) * width); 
        for(int j = 0; j < width; j++)
        { map[i][j] = -1; }
    }

    //Initialize bot
    bot *bot = malloc(sizeof(bot));
    bot->x = (width / 2) - 1;
    bot->y = (height / 2) - 1;
    bot->b = 0;

    //Initialize state
    state *state = alloc_state(1000);

    //Load state and run bot to find number of panels visited
    int count = 0;
    load_state(state, codes, codesCount);
    do
    {
        int sc = bot_look(bot, map);
        if (sc == -1)
        { 
            sc = 0;
            count++; 
        }

        int pc = (int)execute_state(state, sc);
        bot_paint(bot, map, pc);

        int t = (int)execute_state(state, 0);
        bot_turn(bot, t);
        bot_move(bot, 1);
    }
    while(state->s != status_halt && state->s != status_error);
    printf("part1: panel visit count = %d\n", count);

    //Reset bot
    bot->x = (width / 2) - 1;
    bot->y = (height / 2) - 1;
    bot->b = 0;

    //Reset map
    for(int i = 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        { map[i][j] = 0; }
    }

    //Paint starting panel white
    bot_paint(bot, map, 1);

    //Load state and run bot to paint image
    load_state(state, codes, codesCount);
    do
    {
        int sc = bot_look(bot, map);
        int pc = (int)execute_state(state, sc);
        bot_paint(bot, map, pc);

        int t = (int)execute_state(state, 0);
        bot_turn(bot, t);
        bot_move(bot, 1);
    }
    while(state->s != status_halt && state->s != status_error);
    printf("part2:\n");
    print_map(map, width, height);    

    //Cleanup
    for(int i = 0; i < width; i++)
    { free(map[i]); }
    free(map);
    free(bot);
    free_state(state);
    free(codes);
    return 0;
}