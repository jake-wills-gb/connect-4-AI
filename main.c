#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "graphics.h"
#include "puissance4.h"
#include "AIpuissance4.h"
#include "time.h"

int SIZE_LIGNES;
int SIZE_COLONNES;
int WRAP;
int type_joueur[2];


partie *jeu_humain(partie *p) {
    int col,lig;
    mouvement m;
    m.joueur=p->position.joueur;
    while (1) {
       get_click_grille(&col,&lig);
      //     if (p->position.grille[lig][col]!=0) continue;
   //        m.ligne=lig;
       m.colonne=col;
       if (coup_possible(p,&m)>0) {
          graph_printf("(%d,%d) Humain joue %d.", p->num_coups, p->position.joueur, m.colonne);
          return joue_coup_suivant(p,&m);
       }
    }
}

int partie_complete(partie *p) {
    while (p->termine<=0) {
        mouvement m;
        int player = type_joueur[(1-p->position.joueur)/2];
        update_grille(&(p->position));
        if (player!=0) {
            unsigned int starttemps = SDL_GetTicks();
            int score;
            switch (player) {
               case 1 : score = AIplayer1(p,&m); break;
               case 2 : score = AIplayer2(p,&m); break;
               case 3 : score = AIplayer3(p,&m); break;
               case 4 : score = AIplayer4(p,&m); break;
               case 5 : score = AIplayer5(p,&m); break;
               default : score = AIplayer(p,&m);
            }
            graph_printf("(%d,%d) Ordi%d joue %d (score retourné : %d en %u ms)",p->num_coups,
                         p->position.joueur, player, m.colonne, score,
                         SDL_GetTicks()-starttemps);
            p = joue_coup_suivant(p,&m);
            if (p==NULL) exit(-4);
        } else {
            p = jeu_humain(p); if (p==NULL) exit(-4);
        }
    }
    update_grille(&(p->position));
    return (p->position.score/10000);
}

int debut_partie(int deja_joue) {
    int q1;
    if (deja_joue==0) {
      SIZE_LIGNES=6;
      SIZE_COLONNES=7;
      WRAP=0;
      type_joueur[0] = 0;
      type_joueur[1] = -1;
      graph_printf("Choix (-1: tester l'AI, 0 : partie par défaut, 1 : custom) : ");
    } else {
      graph_printf("Choix (-1: tester l'AI, 0 : même options, 1 : custom) : ");
    }

    graph_scanf("%d", &q1);
    if (q1<=0) {
        return q1;
    } else {
        int sl, sc, w, j1, j2;
        graph_printf("Nb de lignes (max %d) : ", MAX_SIZE_LIGNES);
        graph_scanf("%d", &sl);
        while ((sl<4 || sl>MAX_SIZE_LIGNES)) {
            graph_printf("Nombre incorrect. Nb lignes (max %d) : ", MAX_SIZE_LIGNES);
            graph_scanf("%d",&sl);
        }
        SIZE_LIGNES=sl;
        graph_printf("Nb de colonnes (max %d) : ", MAX_SIZE_COLONNES);
        graph_scanf("%d", &sc);
        while ((sc<4 || sc>MAX_SIZE_COLONNES)) {
            graph_printf("Nombre incorrect. Nb colonnes (max %d) : ", MAX_SIZE_COLONNES);
            graph_scanf("%d",&sc);
        }
        SIZE_COLONNES = sc;
        graph_printf("Wrap (0 : non, 1 : oui) ? ");
        graph_scanf("%d", &w);
        WRAP = (w!=0);
        j1=-2;
        while (j1< -1 || j1>5) {
           graph_printf("Joueur rouge (0 : humain, -1 : AIplayer, 1 à 5 : AIplayeri) ? ");
           graph_scanf("%d",&j1);
        }
        type_joueur[0]=j1;
        j2=-2;
        while (j2< -1 || j2>5) {
           graph_printf("Joueur jaune (0 : humain, 1 : AIplayer, 1 à 5 : AIplayeri) ? ");
           graph_scanf("%d",&j2);
        }
        type_joueur[1]=j2;
        return 0;
    }
}

void test_AI(char *nom_fichier, int numai) {
    FILE *testfile;
    char ligne[80];
    testfile = fopen(nom_fichier,"r");
    while (!feof(testfile)) {
        int numtest,player,nc;
        partie *p;
        char *s;
        fgets(ligne,80,testfile);
        if (sscanf(ligne,"test%d%d%d%d%d",&numtest,&SIZE_LIGNES,&SIZE_COLONNES,&WRAP,&player)<5) {
                printf("Erreur : test incomplet ???"); exit(-1);
        }
        printf("test %d lignes %d cols %d wrap %d joueur %d\n",numtest,SIZE_LIGNES,SIZE_COLONNES,WRAP, player);
        p = cree_partie_initiale();
        p->position.joueur=player;
        for (nc=0;nc<SIZE_COLONNES;nc++) {
            int nl=0;
            fgets(ligne,80,testfile);
//            printf("%s : ",ligne);
            s = ligne;
            while (*s=='R' || *s=='J') {
                int j = *s=='R' ? JBLANC : JNOIR;
                p->position.grille[nl][nc]= j;
                /* int r =  */ calcul_fin_partie(p,nl,nc,j);
                // printf("ajout %d %d %d : %d\n",j,nl,nc,r);
                nl++; s++;
            }
//            printf(" ajoute %d pions.\n",nl);
        }
        fgets(ligne,80,testfile);
        if (p->termine) { printf("erreur : test sur une partie terminee ?\n"); exit(-1); }
        draw_grid();
        update_grille(&(p->position));
        {
            unsigned int starttemps = SDL_GetTicks();
            int score;
            int savefail=-1;
            mouvement m;
            int fail=0;
            int i;
            for (i=0;i<5;i++) {
              switch (numai) {
                 case 1 : score = AIplayer1(p,&m); break;
                 case 2 : score = AIplayer2(p,&m); break;
                 case 3 : score = AIplayer3(p,&m); break;
                 case 4 : score = AIplayer4(p,&m); break;
                 case 5 : score = AIplayer5(p,&m); break;
                 default : score = AIplayer(p,&m);
              }
              printf("essai %d : joueur %d colonne %d score %d\n",i+1,m.joueur,m.colonne,score);
              int u;
              s = ligne;
              while ((u=strtol(s,&s,10))>0 && u-1!=m.colonne);
              if (u==0) { fail++; savefail=m.colonne; }
            }
            if (fail==0) {
                graph_printf("test %d reussi (5 réussites) en %d ms. Cliquer pour continuer.",
                                numtest,SDL_GetTicks()-starttemps);
            } else {
                graph_printf("test %d échoué (%d échecs /5) en %d ms. Cliquer pour continuer.",
                                numtest,fail,SDL_GetTicks()-starttemps);
                graph_printf("Exemple d'echec : colonne %d (au lieu de %d), score %d.",
                                savefail, strtol(ligne,NULL,10)-1, score);
            }
            int d1,d2;
            get_click_grille(&d1,&d2);
        }
        detruit_partie(p);
    }
    graph_printf("Tests terminés.");
}

int graphics_main()
{
    int res;
    int deja_joue = 0;
    srand(time(NULL));
    while (1) {
       graph_clear();
       res = debut_partie(deja_joue);
       if (res>=0) {
          int winner;
          partie *p = cree_partie_initiale();
          draw_grid();
          winner = partie_complete(p);
          switch (winner) {
            case JNOIR : graph_printf("Joueur jaune vainqueur."); break;
            case JBLANC : graph_printf("Joueur rouge vainqueur."); break;
            default : graph_printf("Match nul.");
          }
          deja_joue=1;
          detruit_partie(p);


       } else {
           int i=0;
           while (i!=-1 && (i<1 || i>5)) {
              graph_printf("AI à tester ? (-1 : défault, 1 à 5 : AIplayeri) ");
              graph_scanf("%d",&i);
           }
           test_AI("fichiertest.txt",i);

       }
       char u;
       graph_printf("Reprendre (o/n) ? ");
       graph_scanf("%c",&u);
       if ((u!='o') && (u!='O')) return 0;
    }
    return 0;
}
