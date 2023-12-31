#include "SDL.h"
#include <stdio.h>
#include "tglshim.h"

/* From gears.c */
void init(void);
void reshape(int width, int height);
void idle(void);
void draw(void);

int
main(int ac, char** av){
    SDL_Window* wnd;
    SDL_Surface* dest;
    SDL_Surface* src;
    SDL_Event evt;

    void* pixels;
    int x, y;

    if(SDL_Init(SDL_INIT_VIDEO)){
        printf("SDL Init failed.\n");
        return 1;
    }
    if(!(wnd = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                1280, 720, 0))){
        SDL_Quit();
        printf("SDL CreateWindow failed.\n");
        return 1;
    }

    if(!(dest = SDL_GetWindowSurface(wnd))){
        SDL_Quit();
        printf("SDL GetWindowSurface failed.\n");
        return 1;
    }

    /* Init GL app */
    tglshim_alloc(1280, 720);
    init();
    reshape(1280, 720);

    /* Main Loop */
    for(;;){
        /* Consume events */
        while(SDL_PollEvent(&evt)){
            if(evt.type == SDL_QUIT){
                goto quit;
            }
        }

        /* Draw */
        idle();
        draw();
        tglshim_capture(&pixels, &x, &y);

        /* Blit */
        src = SDL_CreateRGBSurfaceFrom(pixels, 1280, 720, 16, 2 * 1280,
                                       31, 63<<5, 31<<11, 0);

        if(SDL_BlitSurface(src, NULL, dest, NULL)){
            printf("Blit error\n");
        }
        if(SDL_UpdateWindowSurface(wnd)){
            printf("Update error\n");
        }

        SDL_FreeSurface(src);
    }
quit:
    return 0;
}
