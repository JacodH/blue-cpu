#ifndef SCREEN_H
#define SCREEN_H


// Include SDL
#include <SDL2/SDL.h>
#include "cpu.h"

struct Screen {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *font;

    int width;   // pixel width
    int height;  // pixel height
    int scale;
};

struct Screen create_screen(int width, int height, int scale);
void check_close(struct Screen *screen);
void clear_screen(struct Screen *screen);
void screen_cleanup(struct Screen *screen);
void draw_vram(struct Screen *screen, struct CPU *cpu_ptr);

#endif