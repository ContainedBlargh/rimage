/**
 * @file main.c
 * @author Jon Voigt Tøttrup (jon@zendata.dk)
 * @brief A C program that generates random images. Just for fun.
 * @version 0.1
 * @date 2020-10-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "./libattopng.h"
#define RGBA(r, g, b, a) ((r) | ((g) << 8) | ((b) << 16) | ((a) << 24))
#define RGB(r, g, b) RGBA(r, g, b, 255)

#define randf() ((float)rand())/((float)RAND_MAX)

#define min(a, b) (a < b) ? a : b;
#define max(a, b) (a < b) ? b : a;

libattopng_t* png;
int w;
int h;
float d;
int seed = 1;
char* out_path;

float distance(int p, int q) {
    float a1, r1, g1, b1;
    a1 = ((p >> 24) & 0xff) / 255.0f;
    r1 = ((p >> 16) & 0xff) / 255.0f;
    g1 = ((p >> 8) & 0xff) / 255.0f;
    b1 = (p & 0xff) / 255.0f;
    // printf("rgb1(%f, %f, %f)\n", r1, g1, b1);
    float a2, r2, g2, b2;
    a2 = ((q >> 24) & 0xff) / 255.0f;
    r2 = ((q >> 16) & 0xff) / 255.0f;
    g2 = ((q >> 8) & 0xff) / 255.0f;
    b2 = (q & 0xff) / 255.0f;
    // printf("rgb2(%f, %f, %f)\n", r2, g2, b2);
    float d = sqrtf(
        (a1 - a2) * (a1 - a2) +
            (r1 - r2) * (r1 - r2) +
                (g1 - g2) * (g1 - g2) +
                    (b1 - b2) * (b1 - b2)
    );
    // printf("d = %f\n", d);
    return d;
}

int nb[4];
int nbx[4];
int nby[4];
void coerce_surrounding(int x, int y, float max_diff) {
    nb[0] = (y > 0) ? libattopng_get_pixel(png, x, y - 1) : -1;
    nbx[0] = x;
    nby[0] = y - 1;
    nb[1] = (x < w - 1) ? libattopng_get_pixel(png, x + 1, y) : -1;
    nbx[1] = x + 1;
    nby[1] = y;
    nb[2] = (y < h - 1) ? libattopng_get_pixel(png, x, y + 1) : -1;
    nbx[2] = x;
    nby[2] = y + 1;
    nb[3] = (x > 0) ? libattopng_get_pixel(png, x - 1, y) : -1;
    nbx[3] = x - 1;
    nbx[3] = y;
    int p = libattopng_get_pixel(png, x, y);
    int i;
    float d;
    // printf("\n");
    for (i = 0; i < 4; i++) {
        d = distance(p, nb[i]);
        if (nb[i] != -1 && d < max_diff)
        {
            // printf("d: %f\n", d);
            libattopng_set_pixel(png, nbx[i], nby[i], p);
        } else {
            int nbp = libattopng_get_pixel(png, nbx[i], nby[i]);
            int r, g, b;
            r = min(((nbp >> 16) & 0xff) + 1, 255);
            g = min(((nbp >> 8) & 0xff) + 1, 255);
            b = min((nbp & 0xff) + 1, 255);
            libattopng_set_pixel(png, nbx[i], nby[i], RGB(r, g, b));
        }
    }
}


int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        printf("Usage of this program:\nrimage <seed> <w> <h> <d> <out.png>\n");
        return 0;
    } else {
        sscanf(argv[1], "%d", &seed);
        srand(seed);
        sscanf(argv[2], "%d", &w);
        sscanf(argv[3], "%d", &h);
        sscanf(argv[4], "%f", &d);
        out_path = argv[5];
    }
    png = libattopng_new((size_t)w, (size_t)h, PNG_RGBA);
    int x, y;
    int r, g, b;
    for (x = 0; x < w; x++) {
        for (y = 0; y < h; y++) {
            r = 255 * randf();
            g = 255 * randf();
            b = 255 * randf();
            libattopng_set_pixel(png, x, y, RGB(r, g, b));
        }
    }
    int i;
    for (i = 0; i < 33; i++) {
        for (x = 0; x < w; x++) {
            for (y = 0; y< h; y++) {
                coerce_surrounding(x, y, d);
            }
        }
    }
    libattopng_save(png, out_path);
    libattopng_destroy(png);    
    return 0;
}

