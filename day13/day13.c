//
//  day13.c
//  day13
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

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

typedef enum tile tile;
enum tile
{
    tile_empty = 0,
    tile_wall = 1,
    tile_block = 2,
    tile_paddle = 3,
    tile_ball = 4
};

int64_t get_mode_read(mode mode, state *s, int64_t i)
{
    int64_t v = 0;
    switch(mode)
    {
        case mode_position:
            v = s->m[i];
            break;
        case mode_immediate:
            v = i;
            break;
        case mode_relative:
            v = s->m[s->r + i];
            break;
        default:
            break;
    }
    return v;
}

int64_t get_mode_write(mode mode, state *s, int64_t i)
{
    int64_t v = 0;
    switch(mode)
    {
        case mode_position:
        case mode_immediate:
            v = i;
            break;
        case mode_relative:
            v = s->r + i;
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

int64_t execute_state(state *s, int64_t state_in)
{
    //Execute codes
    int64_t *input = &state_in;
    for(;;)
    {
        instruction instruction = parse_instruction(s->m[s->i]);
        switch(instruction.op)
        {
            case 1:
            {
                //Add
                int64_t a = s->m[s->i+1];
                int64_t b = s->m[s->i+2];
                int64_t c = s->m[s->i+3];
                int64_t av = get_mode_read(instruction.m1, s, a);
                int64_t bv = get_mode_read(instruction.m2, s, b);
                int64_t cv = get_mode_write(instruction.m3, s, c);
                s->m[cv] = av + bv;
                s->i += 4;
                break;
            }
            case 2:
            {
                //Multiply
                int64_t a = s->m[s->i+1];
                int64_t b = s->m[s->i+2];
                int64_t c = s->m[s->i+3];
                int64_t av = get_mode_read(instruction.m1, s, a);
                int64_t bv = get_mode_read(instruction.m2, s, b);
                int64_t cv = get_mode_write(instruction.m3, s, c);
                s->m[cv] = av * bv;
                s->i += 4;
                break;
            }
            case 3:
            {
                //Input
                if (input == NULL)
                {
                    s->s = status_need_input;
                    return 0;
                }
                int64_t a = s->m[s->i+1];
                int64_t av = get_mode_write(instruction.m1, s, a);
                s->m[av] = *input;
                input = NULL;
                s->i += 2;
                break;
            }
            case 4:
            {
                //Output
                int64_t a = s->m[s->i+1];
                int64_t av = get_mode_read(instruction.m1, s, a);
                s->i += 2;
                s->s = status_output;
                return av;
                break;
            }
            case 5:
            {
                //Jump if true
                int64_t a = s->m[s->i+1];
                int64_t b = s->m[s->i+2];
                int64_t av = get_mode_read(instruction.m1, s, a);
                int64_t bv = get_mode_read(instruction.m2, s, b);
                s->i = av != 0 ? bv : s->i + 3;
                break;
            }
            case 6:
            {
                //Jump if false
                int64_t a = s->m[s->i+1];
                int64_t b = s->m[s->i+2];
                int64_t av = get_mode_read(instruction.m1, s, a);
                int64_t bv = get_mode_read(instruction.m2, s, b);
                s->i = av == 0 ? bv : s->i + 3;
                break;
            }
            case 7:
            {
                //Less than
                int64_t a = s->m[s->i+1];
                int64_t b = s->m[s->i+2];
                int64_t c = s->m[s->i+3];
                int64_t av = get_mode_read(instruction.m1, s, a);
                int64_t bv = get_mode_read(instruction.m2, s, b);
                int64_t cv = get_mode_write(instruction.m3, s, c);
                s->m[cv] = av < bv;
                s->i += 4;
                break;
            }
            case 8:
            {
                //Equals
                int64_t a = s->m[s->i+1];
                int64_t b = s->m[s->i+2];
                int64_t c = s->m[s->i+3];
                int64_t av = get_mode_read(instruction.m1, s, a);
                int64_t bv = get_mode_read(instruction.m2, s, b);
                int64_t cv = get_mode_write(instruction.m3, s, c);
                s->m[cv] = av == bv;
                s->i += 4;
                break;
            }
            case 9:
            {
                int64_t a = s->m[s->i+1];
                int64_t av = get_mode_read(instruction.m1, s, a);
                s->r += av;
                s->i += 2;
                break;
            }
            case 99:
            {
                //Halt
                s->s = status_halt;
                return s->m[0];
                break;
            }
            default:
                //Error
                printf("error: unknown opcode %lld\n", instruction.op);
                s->s = status_error;
                return instruction.op;
                break;
        }
    }
}

state *alloc_state(int ms)
{
    state *s = malloc(sizeof(state));
    s->ms = ms;
    s->m = malloc(sizeof(int64_t) * ms);
    s->i = 0;
    s->r = 0;
    s->s = status_idle;
    return s;
}

void load_state(state *s, int64_t *c, int cc)
{
    s->i = 0;
    s->r = 0;
    s->s = status_idle;
    memset(s->m, 0, sizeof(int64_t) * s->ms);
    memcpy(s->m, c, sizeof(int64_t) * cc);
}

void free_state(state *s)
{
    free(s->m);
    free(s);
}

void render_tiles(tile **tiles, int width, int height)
{
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            tile t = tiles[y][x];
            char c;
            switch(t)
            {
                case tile_empty:
                    c = '.';
                    break;
                case tile_wall:
                    c = '|';
                    break;
                case tile_block:
                    c = '#';
                    break;
                case tile_paddle:
                    c = '=';
                    break;
                case tile_ball:
                    c = 'o';
                    break;
                default:
                    break;
            }
            mvprintw(y, x, "%c", c);
        }
    }
    refresh();
}

int main(int argc, char *argv[])
{
    //Argument check
    if (argc < 2)
    {
        printf("usage: day13 <INPUT>\n");
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

    //Initialize state
    state *s = alloc_state(3000);

    //Load state and start game to find number of block tiles
    int blockCount = 0;
    load_state(s, codes, codesCount);
    do
    {
        int x = (int)execute_state(s, 0);
        int y = (int)execute_state(s, 0);
        tile t = (tile)execute_state(s, 0);
        if (t == tile_block)
        { blockCount++; }
    }
    while(s->s != status_halt && s->s != status_error);

    //Initialize tiles
    int width = 44;
    int height = 24;
    tile **tiles = malloc(sizeof(tile *) * height);
    for(int y = 0; y < height; y++)
    {
        tiles[y] = malloc(sizeof(tile) * width);
        for(int x = 0; x < width; x++)
        { tiles[y][x] = tile_empty; }
    }

    //Initialize ncurses
    initscr();
    clear();

    //Load state and hack game with 2 fake quarters
    load_state(s, codes, codesCount);
    s->m[0] = 2;

    //Draw initial screen
    for(int i = 0; i < width*height; i++)
    {
        //Get next x,y and tile
        int x = (int)execute_state(s, 0);
        int y = (int)execute_state(s, 0);
        tiles[y][x] = (tile)execute_state(s, 0);
        render_tiles(tiles, width, height);
    }

    //Start main game loop
    int score = 0;
    int lx = 0;
    int input = 0;
    int tick = 0;
    do
    {
        //Get next x and y values
        int x = (int)execute_state(s, input);
        int y = (int)execute_state(s, input);

        //Save score and continue
        if (x == -1 && y == 0)
        { 
            score = (int)execute_state(s, input);
            continue;
        }

        //Update tile and start adjusting input after first hit
        tiles[y][x] = (tile)execute_state(s, input);
        if (tiles[y][x] == tile_ball && tick > 3)
        {
            //Follow ball
            int dx = x - lx;
            if (dx > 0)
            { input = 1; }
            else if (dx < 0)
            { input = -1; }
            else
            { input = 0; }
            lx = x;
        }

        //Render with delay (for fun)
        render_tiles(tiles, width, height);
        tick++;
    }
    while(s->s != status_halt && s->s != status_error);
    clear();
    printw("part1: number of block tiles: %d\n", blockCount);
    printw("part2: final score: %d\n", score);
    refresh();

    //Cleanup
    for(int y = 0; y < height; y++)
    { free(tiles[y]); }
    free(tiles);
    free_state(s);
    free(codes);
    return 0;
}