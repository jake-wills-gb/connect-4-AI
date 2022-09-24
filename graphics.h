#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <string.h>
#include "puissance4.h"


int graphics_main();
/* initialisation function */
int graphics_open(const char *s,int mode);
void graphics_close();

/* spécifique kono */
void draw_grid();
void update_grille(damier *d);
int get_click_grille(int *x, int *y);
void dessine_pion(int posy, int posx, int couleur, int couleur2, int select);

/* text input/output */
void graph_gets(char *s);
void graph_puts(char *s);
void graph_printf(char *format, ...);
void graph_scanf(char *format, ...);

/* partie graphique */
void graph_set_color(int r, int g, int b);
void graph_draw_line(int x1, int y1, int x2, int y2);
void graph_get_click(int *x, int *y);
void graph_clear();

#endif // GRAPHICS_H_INCLUDED
