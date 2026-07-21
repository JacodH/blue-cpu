#include "cpu.h"
#include "screen.h"

struct Screen create_screen(int width, int height, int scale) {

    // Init screen struct (defined in screen.h)
    struct Screen screen = {
        .window = NULL,
        .renderer = NULL,
        .font = NULL,
        .width = width, 
        .height = height,
        .scale = scale,
    };
    
    
    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) == 1) {printf("SDL ERROR"); exit(1); };
    
    // Create window and pass it to screen struct
    screen.window = SDL_CreateWindow("Blue", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen.width*screen.scale, screen.height*screen.scale, 0);
    if (screen.window == NULL) { printf("SDL ERROR on loading window"); exit(1);}
    
    // Create renderer and pass it to screen struct
    screen.renderer = SDL_CreateRenderer(screen.window, -1, 0);
    if (screen.renderer == NULL) { printf("SDL ERROR on creating renderer"); exit(1);}
    
    // load up font
    SDL_Surface *surface = SDL_LoadBMP("emulation/font.bmp");
    if (surface == NULL) {
        printf("Failed to load font: %s\n", SDL_GetError());
        exit(1);
    }
    screen.font = SDL_CreateTextureFromSurface(screen.renderer, surface);
    if (screen.font == NULL) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        exit(1);
    }
    // SDL_SetTextureColorMod(screen.font, 7, 209, 238);
    // SDL_SetTextureColorMod(screen.font, 25, 150, 255);
    SDL_SetTextureColorMod(screen.font, 45, 69, 250);
    SDL_FreeSurface(surface);

    return screen;
}

void check_close(struct Screen *screen) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                screen_cleanup(screen);
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_ESCAPE:
                        // screen_cleanup(screen);
                        break;
                    default:
                        break;
                }
                

            default:  
                break;
        }

    }
}

void draw_vram(struct Screen *screen, struct CPU *cpu_ptr) {
    for (int i = 0; i < 625; i++) {
        // once i get a vram tester program id need to add i + VRAM_START
        uint8_t vram_byte = cpu_ptr->RAM[i + VRAM_START];

        int adjusted = vram_byte - 32;
        int font_col = adjusted % 32;
        int font_row = adjusted / 32;

        int col = i % 25;  // 25 chars per row
        int row = i / 25;  // 25 rows

        SDL_Rect src = { font_col * 8, font_row * 8, 8, 8 };
        SDL_Rect dst = { col * 8 * screen->scale, row * 8 * screen->scale, 8*screen->scale, 8*screen->scale };
        // idk what this does
        SDL_RenderCopy(screen->renderer, screen->font, &src, &dst);
    }
}

void clear_screen(struct Screen *screen) {
    // Set bg to black
    SDL_SetRenderDrawColor(screen->renderer, 0, 0, 0, 255);
    SDL_RenderClear(screen->renderer);
}

void screen_cleanup(struct Screen *screen) {
    SDL_DestroyRenderer(screen->renderer);
    SDL_DestroyWindow(screen->window);
    SDL_Quit();
    exit(0);
}