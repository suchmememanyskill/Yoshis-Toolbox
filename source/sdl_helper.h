#pragma once
#include <SDL_ttf.h>
#include <switch.h>

#define COLOR_MAKE(r, g, b, a) (SDL_Color){r, g, b, a}

#define COLOR_WHITE (SDL_Color){255, 255, 255, 255}
#define COLOR_LIGHTBLUE (SDL_Color){0, 243, 243, 255}
#define COLOR_BACKGROUND (SDL_Color){0, 50, 0, 255}
#define COLOR_TOP (SDL_Color){0, 75, 0, 255}
#define COLOR_HIGHLIGHT (SDL_Color){0, 100, 0, 255}
#define COLOR_GREEN (SDL_Color){0, 255, 0, 255}
#define COLOR_RED (SDL_Color){255, 0, 0, 255}
#define COLOR_BLACK (SDL_Color){0, 0, 0, 255}

void ClearScreen(SDL_Color color);
void DrawOutline(int x, int y, int x2, int y2, int bordersize, SDL_Color color);
void DrawBox(int x, int y, int w, int h, SDL_Color color);
void SDLDeInit();
void SDLInit();
void RenderFrame();
void GetTextSize(char *text, int *w, int *h);
int GetButtonTextSize(u16 text);
void DrawText(int x, int y, char *text, SDL_Color color, int wrap);
void DrawGlyph(int x, int y, u16 glyph, SDL_Color color);

/*
#define COLOR_BACKGROUND (SDL_Color){23, 24, 25, 255}
#define COLOR_TOP (SDL_Color){46, 48, 50, 255}
#define COLOR_HIGHLIGHT (SDL_Color){69, 72, 75, 255}
*/