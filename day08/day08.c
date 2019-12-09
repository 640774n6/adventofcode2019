//
//  day08.c
//  day08
//
//  Created by Brandon Holland on 2019-12-01.
//  Copyright © 2019 Brandon Holland. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define LAYER_WIDTH     25
#define LAYER_HEIGHT    6

void render_image(int **image, int w, int h)
{
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            int px = image[y][x];
            char c = px ? '#' : '.';
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
        printf("usage: day08 <INPUT>\n");
        exit(0);
    }

    //Open input file
    FILE *file = fopen(argv[1], "r");

    //Get total size
    fseek(file, 0, SEEK_END);
    size_t totalSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    //Initialize layers
    int layerSize = (LAYER_WIDTH * LAYER_HEIGHT);
    int layersCount = totalSize / layerSize;
    int **layers = malloc(sizeof(int *) * layersCount);
    for(int i = 0; i < layersCount; i++)
    { layers[i] = malloc(sizeof(int) * layerSize); }

    //Parse layers
    int layer = 0;
    int layerPx = 0;
    int px = 0;
    while(fscanf(file, "%1d", &px) == 1)
    {
        layers[layer][layerPx] = px;
        layerPx++;
        if (layerPx == layerSize)
        {
            layerPx = 0;
            layer++;
        }
    }
    fclose(file);

    //Find checksum (1 count * 2 count) of layer with min zeros
    int minZeroLayer = -1;
    int minZero = INT_MAX;
    int *layerSums = malloc(sizeof(int) * layersCount);
    memset(layerSums, 0, sizeof(int) * layersCount);

    for(int i = 0; i < layersCount; i++)
    {
        int counts[3] = { 0, 0, 0 };
        for(int p = 0; p < layerSize; p++)
        {
            int px = layers[i][p];
            if (px < 3)
            { counts[px]++; }
        }
        
        layerSums[i] = counts[1] * counts[2];
        if (counts[0] < minZero)
        {
            minZero = counts[0];
            minZeroLayer = i;
        }
    }
    printf("part1: image checksum = %d\n", layerSums[minZeroLayer]);
    free(layerSums);

    //Render layers
    int **image = malloc(sizeof(int *) * LAYER_HEIGHT);
    for(int i = 0; i < LAYER_HEIGHT; i++)
    { 
        image[i] = malloc(sizeof(int) * LAYER_WIDTH); 
        for(int j = 0; j < LAYER_WIDTH; j++)
        { image[i][j] = 2; }
    }

    for(int i = 0; i < layersCount; i++)
    {
        for(int y = 0; y < LAYER_HEIGHT; y++)
        {
            for(int x = 0; x < LAYER_WIDTH; x++)
            {
                int offset = (y * LAYER_WIDTH) + x;
                int px = layers[i][offset];
                if (image[y][x] == 2)
                { image[y][x] = px; }
            }
        }
    }
    render_image(image, LAYER_WIDTH, LAYER_HEIGHT);
    free(image);

    //Cleanup
    for(int i = 0; i < layersCount; i++)
    { free(layers[i]); }
    free(layers);
    return 0;
}
