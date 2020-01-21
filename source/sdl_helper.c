#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <switch.h>
#include "sdl_helper.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
TTF_Font *textfont, *buttonfont;

void SDLInit(){
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow("Menu", 0, 0, 1920, 1080, 4);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    PlFontData font, button_data;
    plGetSharedFontByType(&font, PlSharedFontType_Standard);
    plGetSharedFontByType(&button_data, PlSharedFontType_NintendoExt);

    textfont = TTF_OpenFontRW(SDL_RWFromMem(font.address, font.size), 1, 36);
    buttonfont = TTF_OpenFontRW(SDL_RWFromMem(button_data.address, button_data.size), 1, 50);
}

void SDLDeInit(){
    TTF_CloseFont(textfont);
    TTF_CloseFont(buttonfont);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void DrawBox(int x, int y, int w, int h, SDL_Color color){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect box = {x, y, w, h};
    SDL_RenderFillRect(renderer, &box);
}

void DrawOutline(int x, int y, int x2, int y2, int bordersize, SDL_Color color){
    DrawBox(x, y, bordersize, y2 - y, color);
    DrawBox(x2 - bordersize, y, bordersize, y2 - y, color);
    DrawBox(x, y, x2 - x, bordersize, color);
    DrawBox(x, y2 - bordersize, x2 - x, bordersize, color);
}

void ClearScreen(SDL_Color color){
    int w, h;

    SDL_RenderClear(renderer);
    SDL_GetWindowSize(window, &w, &h);

    DrawBox(0, 0, w, h, color);
}

void RenderFrame(){
    SDL_RenderPresent(renderer);
}

void GetTextSize(char *text, int *w, int *h){
    TTF_SizeText(textfont, text, w, h);
}

int GetButtonTextSize(u16 text){
    /*
    int w, h;
    TTF_SizeUNICODE(buttonfont, &text, &w, &h);
    return w;
    */
   return 51;
}

void DrawSurface(int x, int y, SDL_Surface *surface, SDL_Color color){
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect pos = {x, y, surface->w, surface->h};

    SDL_RenderCopy(renderer, t, NULL, &pos);
    SDL_DestroyTexture(t);
    SDL_FreeSurface(surface);
}

void DrawGlyph(int x, int y, u16 glyph, SDL_Color color){
    SDL_Surface *surface = TTF_RenderGlyph_Blended(buttonfont, glyph, color);
    DrawSurface(x, y, surface, color);
}

void DrawText(int x, int y, char *text, SDL_Color color, int wrap){
    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(textfont, text, color, wrap);
    DrawSurface(x, y, surface, color);
}