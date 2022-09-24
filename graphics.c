#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "graphics.h"
#include <stdarg.h>

static SDL_Renderer *renderer;
static SDL_Texture *pBlanc, *pNoir, *pVide , *pBlancS, *pNoirS;

static SDL_Rect my_rect;
static TTF_Font *police = NULL;
#define MAX_NB_LINES    6
#define MAX_MEM_LINES  80
static SDL_Surface *texts[MAX_MEM_LINES];
static int nb_lines=0;
static int mem_lines=0;
static int bottom_lines=0;
static int col_r, col_g, col_b;

static int start_grid;

/* utility functions : manage text display */
void display_lines() {
    int i,j;
    SDL_Rect bigRect;
    SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,SDL_ALPHA_OPAQUE);
    bigRect.x=20;
    bigRect.y=2;
    bigRect.w = 700;
    bigRect.h = MAX_NB_LINES*30;
    SDL_RenderFillRect(renderer,&bigRect);
    my_rect.x=20;
    for (i=0;i<nb_lines;i++) {
        j = bottom_lines-nb_lines+i;
        if (texts[j]==NULL) continue;
        SDL_Texture *Rtext = SDL_CreateTextureFromSurface(renderer,texts[j]);
        my_rect.h = texts[j]->h;
        my_rect.w = texts[j]->w;
        my_rect.y = 2+my_rect.h*i;
        SDL_RenderCopy(renderer,Rtext,NULL,&my_rect);
        SDL_DestroyTexture(Rtext);
    }
    SDL_RenderPresent(renderer);
}

void add_line(char *s, Uint8 bg_r, Uint8 bg_g, Uint8 bg_b, Uint32 delay) {
    SDL_Color color = {bg_r,bg_g,bg_b};
    if (mem_lines==MAX_MEM_LINES) {
        int i;
        SDL_FreeSurface(texts[0]);
        for (i=0;i<MAX_MEM_LINES-1;i++) {
            texts[i] = texts[i+1];
        }
    } else mem_lines++;
    if (nb_lines<MAX_NB_LINES) nb_lines++;
    printf("%s\n",s);
    texts[mem_lines-1] = TTF_RenderText_Solid(police, s ,color);
    bottom_lines = mem_lines;
    display_lines();
    SDL_Delay(delay);
}

void draw_String(char *s, Uint8 bg_r, Uint8 bg_g, Uint8 bg_b, SDL_Rect *result_rect) {
    SDL_Color color = {0,0,0};
    SDL_Surface *text = TTF_RenderText_Solid(police, s ,color);
    result_rect -> w = text->w;
    result_rect -> h = text->h;
    SDL_Texture *Rtext = SDL_CreateTextureFromSurface(renderer,text);
    SDL_SetRenderDrawColor(renderer,bg_r,bg_g,bg_b,SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer,result_rect);
    SDL_RenderCopy(renderer,Rtext,NULL,result_rect);
}

/* utility functions : read a
text */
void get_text(char *text, int sizemax, SDL_Rect *written_rect) {
    SDL_bool done = SDL_FALSE;
    int modified = 0, defil=0;
    SDL_StartTextInput();
    written_rect->h=0;
    written_rect->w=0;
    text[0]='\0';
    while (!done) {
        SDL_Event event;
        if (modified) {
                if (defil==1) {
                    bottom_lines = mem_lines;
                    display_lines();
                    defil=0;
                }
                SDL_RenderFillRect(renderer,written_rect);
                SDL_Color color = {0,0,0};
                SDL_Surface *surf;
                surf = TTF_RenderText_Solid(police, (text[0]!='\0' ? text : " ") ,color);
                SDL_Texture *Rtext = SDL_CreateTextureFromSurface(renderer,surf);
                written_rect->h = surf->h;
                written_rect->w = surf->w;
                SDL_RenderCopy(renderer,Rtext,NULL,written_rect);
                SDL_DestroyTexture(Rtext);
                SDL_FreeSurface(surf);
                SDL_RenderPresent(renderer);
                modified=0;
        }
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    TTF_Quit();
                    SDL_Quit();
                    exit(-9);
                    break;
                case SDL_KEYDOWN :
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                        done = SDL_TRUE;
                    } else if (event.key.keysym.sym == SDLK_BACKSPACE) {
                        if (strlen(text)>0) {
                          text[strlen(text)-1]='\0';
                          modified = 1;
                        }
                    } else if (event.key.keysym.sym == SDLK_UP) {
                        if (bottom_lines>nb_lines) {
                            bottom_lines--;
                            display_lines();
                            defil=1;
                        }
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        if (bottom_lines<mem_lines) {
                            bottom_lines++;
                            display_lines();
                            if (bottom_lines==mem_lines) { defil=0; modified=1; }
                        }
                    }
                    break;
                case SDL_TEXTINPUT:
                    if (event.text.text[0]=='\n') {
                        done = SDL_TRUE;
                    } else if (strlen(text)+strlen(event.text.text)<sizemax) {
                    /* Add new text onto the end of our text */
                        strcat(text, event.text.text);
                        modified = 1;
                    }
                    break;
            }

        }
    }
}

/* read a line after a text */
void add_line_with_text(char *s, char *result, int sizemax, Uint8 bg_r, Uint8 bg_g, Uint8 bg_b) {
    SDL_Rect written_rect;
    SDL_Color color = {bg_r,bg_g,bg_b};
    if (mem_lines==MAX_MEM_LINES) {
        int i;
        SDL_FreeSurface(texts[0]);
        for (i=0;i<MAX_MEM_LINES-1;i++) {
            texts[i] = texts[i+1];
        }
    } else mem_lines++;
    if (nb_lines<MAX_NB_LINES) nb_lines++;
    bottom_lines = mem_lines;
    if (s[0]=='\0') {
        texts[mem_lines-1] = TTF_RenderText_Solid(police, " " ,color);
    } else {
        texts[mem_lines-1] = TTF_RenderText_Solid(police, s ,color);
    }
    written_rect.x = 20+texts[mem_lines-1]->w;
    written_rect.y = 2+texts[mem_lines-1]->h*(nb_lines-1);
    if (s[0]=='\0') { SDL_FreeSurface(texts[mem_lines-1]); texts[mem_lines-1]=NULL; written_rect.x = 20; }
    display_lines();
    get_text(result,sizemax,&written_rect);
    {
        char u[300];
        strcpy(u,s);
        strcat(u, result);
        printf("%s\n",u);
        texts[mem_lines-1] = TTF_RenderText_Solid(police, u ,color);
    }
    display_lines();
}






/* initialisation function */
int graphics_open(const char *s,int mode) {
    SDL_Window *win;
    win = SDL_CreateWindow(s,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,1000,675,0);
    renderer=SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
    if (TTF_Init()) {
        fprintf(stderr,"Erreur d'ouverture de police.\n");
        SDL_Quit();
        exit(1);
    }
    police = TTF_OpenFont("c:\\windows\\fonts\\comic.ttf",20);
    if (police==NULL) {
        fprintf(stderr,"Erreur d'ouverture de police.\n");
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    SDL_Surface *surf = SDL_LoadBMP("pionRouge.bmp");
    pBlanc = SDL_CreateTextureFromSurface(renderer,surf);
    SDL_FreeSurface(surf);
    surf = SDL_LoadBMP("pionJaune.bmp");
    pNoir = SDL_CreateTextureFromSurface(renderer,surf);
    SDL_FreeSurface(surf);
    surf = SDL_LoadBMP("pionVide.bmp");
    pVide = SDL_CreateTextureFromSurface(renderer,surf);
    SDL_FreeSurface(surf);
    surf = SDL_LoadBMP("pionRougeS.bmp");
    pBlancS = SDL_CreateTextureFromSurface(renderer,surf);
    SDL_FreeSurface(surf);
    surf = SDL_LoadBMP("pionJauneS.bmp");
    pNoirS = SDL_CreateTextureFromSurface(renderer,surf);
    SDL_FreeSurface(surf);

    col_r = col_g  = col_b = 0;

    return 0;
}


void graphics_close() {
    SDL_Quit();
}

/* text input/output */
void graph_gets(char *s) {
   add_line_with_text("",s,100,0x00,0x00,0x00);
}

void graph_puts(char *s) {
    add_line(s,0x00,0x00,0x00,0);
}

void graph_printf(char *format, ...) {
    va_list args;
    char s[100];

    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);

    add_line(s,0x00,0x00,0x00,0);
}

void graph_scanf(char *format, ...) {
    va_list args;
    char s[80];
    graph_gets(s);

    va_start(args, format);
    vsscanf(s, format, args);
    va_end(args);
}

void efface_pion(int posx, int posy) {
    SDL_Rect tete_rect;
    tete_rect.x = 55+posx*60;
    tete_rect.y = 167+posy*60;
    tete_rect.h = 50;
    tete_rect.w = 50;
    SDL_RenderFillRect(renderer,&tete_rect);
}


void dessine_pion(int posy, int posx, int couleur, int couleur2, int select) {
    SDL_Rect tete_rect;
    SDL_Texture **pion;
    tete_rect.x = 55+posx*60;
    tete_rect.y = start_grid+5+(SIZE_LIGNES-posy-1)*60;
    tete_rect.h = 50;
    tete_rect.w = 50;
    SDL_SetRenderDrawColor(renderer,73,72,204,SDL_ALPHA_OPAQUE);
    switch (couleur) {
       case JNOIR : pion = &pNoir; break;
       case JBLANC : pion = &pBlanc; break;
       case 2*JNOIR : pion = &pNoirS; break;
       case 2*JBLANC : pion = &pBlancS; break;
       default : pion = &pVide;
    }

  /*  switch (couleur2) {
       case 1 : pion = &pNoirS; break;
       case 2 : pion = &pBlancS; break;
       case 3 : pion = &pVide; break;
    } */
        SDL_RenderCopy(renderer,*pion,NULL,&tete_rect);
}

void update_grille(damier *d) {
   int i,j;

    for (i=0;i<SIZE_LIGNES;i++)
        for (j=0;j<SIZE_COLONNES;j++)
        {
            dessine_pion(i,j,d->grille[i][j],d->promesse[i][j],0);
        }
    SDL_RenderPresent(renderer);
}

/* dessine grille de kono et le jeu initial */
void draw_grid() {
    int i;
    /* effacement de la grille */
    graph_clear();
    SDL_Rect graph_rect;
    SDL_SetRenderDrawColor(renderer,73,72,204,SDL_ALPHA_OPAQUE);
    start_grid = 662 - SIZE_LIGNES*60;
    graph_rect.x=40; graph_rect.y=start_grid;
    graph_rect.w=SIZE_COLONNES*60+20; graph_rect.h=SIZE_LIGNES*60+10;
    SDL_RenderFillRect(renderer,&graph_rect);
    /* dessin de la grille */
    SDL_SetRenderDrawColor(renderer,0xFF, 0xFF, 0x7F,SDL_ALPHA_OPAQUE);
    for (i=0;i<=SIZE_LIGNES;i++) {
        SDL_RenderDrawLine(renderer,50,start_grid+i*60,SIZE_COLONNES*60+50,start_grid+i*60);
    }
     for (i=0;i<=SIZE_COLONNES;i++) {
        SDL_RenderDrawLine(renderer,50+i*60,start_grid,50+i*60,SIZE_LIGNES*60+start_grid);
    }

    SDL_RenderPresent(renderer);
}

/* 0 : click normal. -1 : autre */
int get_click_grille(int *x, int *y) {
    SDL_Event event;
    SDL_RenderPresent(renderer);
    while (1)
    {
        SDL_Delay(20);
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_KEYDOWN: {
                    if (event.key.keysym.sym == SDLK_UP) {
                        if (bottom_lines>nb_lines) {
                            bottom_lines--;
                            display_lines();
                        }
                    } else if (event.key.keysym.sym == SDLK_DOWN) {
                        if (bottom_lines<mem_lines) {
                            bottom_lines++;
                            display_lines();
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: {
                    *x= (event.button.x-50)/60;
                    *y= SIZE_LIGNES - 1 - (event.button.y-start_grid)/60;
                    if ((*x<0) || (*x>=SIZE_COLONNES)) continue;
                    return 0;
                }
                case SDL_QUIT:
                    TTF_Quit();
                    SDL_Quit();
                    exit(-1);
            }
        }
    }
}



void graph_set_color(int r, int g, int b) {
    col_r = r;
    col_g = g;
    col_b = b;
}

void graph_draw_line(int x1, int y1, int x2, int y2) {
    SDL_SetRenderDrawColor(renderer,col_r, col_g, col_b,SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer,x1,y1+250,x2,y2+250);
    SDL_RenderPresent(renderer);
}

void graph_clear() {
    SDL_Rect graph_rect;
    SDL_SetRenderDrawColor(renderer,0xFF,0xFF,0xFF,SDL_ALPHA_OPAQUE);
    graph_rect.x=0; graph_rect.y=180;
    graph_rect.w=1000; graph_rect.h=501;
    SDL_RenderFillRect(renderer,&graph_rect);
    SDL_RenderPresent(renderer);
}

void graph_get_click(int *x, int *y) {
    SDL_Event event;
    SDL_RenderPresent(renderer);
    while (1)
    {
        SDL_Delay(20);
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    *x = event.button.x;
                    *y = event.button.y-130;
                    if (*y<0) continue;
                    return;
                case SDL_QUIT:
                    TTF_Quit();
                    SDL_Quit();
                    exit(-1);
            }
        }
    }
}


/* principal function */
int main(int argc, char*argv[]) {
    int ret;
    graphics_open("Puissance 4",0);
    ret=graphics_main();
    graphics_close();
    return ret;
}
