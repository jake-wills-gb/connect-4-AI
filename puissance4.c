#include "puissance4.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

partie *cree_partie_initiale () {
   partie *retour= calloc(1,sizeof(partie));
   retour->position.joueur = JBLANC;
   retour->position_prec = retour->position_suiv = NULL;
   return retour;
}

void detruit_partie(partie *p) {
    partie *pn;
    if (p==NULL) return;
    if (p->position_prec) { bloque_avenir(p->position_prec); p->position_prec->position_suiv = NULL; }
    while (p!=NULL) {
       pn = p->position_suiv;
       free(p);
       p = pn;
    }
}

/* -1 : hors borne ou NULL        -2 : mauvais joueur
   -3 : colonne pleine
   1 : ok  */
int coup_possible (partie *p, mouvement *c) {
   if ((p==NULL) || (c==NULL)) return -1;
   if (c->colonne>=SIZE_COLONNES)
   {
        return -1;
   }
   if (p->position.joueur!= c->joueur) return -2;
   if (p->position.grille[SIZE_LIGNES-1][c->colonne]!=0) return -3;
   return 1;
}

int remplit_liste_mouvements(partie *p, liste_mouvements *lm) {
    int c,nbm=0,ret;
    for (c=0;c<SIZE_COLONNES;c++) {
        lm->liste[nbm].colonne=c;
        lm->liste[nbm].joueur=p->position.joueur;
        if ((ret=coup_possible(p,&(lm->liste[nbm])))==1) nbm++;
  //      else printf("Coup pas possible : %d (%d,%d)\n",ret,c,lm->liste[nbm].joueur);
    }
    lm->nb_moves = nbm;
    return nbm;
}

int compare_score_move(const void *m1, const void *m2) {
    const mouvement *mv1 = m1;
    const mouvement *mv2 = m2;
    if (mv1->score>mv2->score) return -1;
    else if (mv1->score==mv2->score) return 0;
    else return 1;
}

void tri_liste_mouvements (partie *p, liste_mouvements *lm) {
    int nbm;
    partie *p2;
    for (nbm=0;nbm<lm->nb_moves;nbm++) {
        p2 = joue_coup_suivant(p,&(lm->liste[nbm]));
        lm->liste[nbm].score = get_score(p2)*get_joueur(p);
    }
    qsort(lm->liste,lm->nb_moves,sizeof(mouvement),compare_score_move);
}

/* valeur grille */
static int get_grille(damier *d, int c, int l) {
   if (c<0) if (WRAP) c+=SIZE_COLONNES; else return -10;
   else if (c>=SIZE_COLONNES) { if (WRAP) c-=SIZE_COLONNES; else return -10; }
   if (l<0) return -10;
   if (l>=SIZE_LIGNES) return -10;
   return d->grille[l][c];
}
/* valeur grille */
static void set_promesse(damier *d, int c, int l, int joueur) {
   if ((c<0) && (WRAP)) c+=SIZE_COLONNES;
   else if ((c>=SIZE_COLONNES) && (WRAP)) c-=SIZE_COLONNES;
   d->promesse[l][c] |= 1+(joueur==JBLANC);
}
/* modifie "promesse" en fonction d'un nouveau coup placé en c,l */
void adapte_damier(damier *d, int c, int l, int joueur) {
   /* vertical */
   if ((l>=2) && (l<SIZE_LIGNES-1) && (d->grille[l-1][c]==joueur) && (d->grille[l-2][c]==joueur))
        d->promesse[l+1][c] |= 1+ (joueur == JBLANC);
   /* horizontal */
   unsigned int a,b; /* a=0 : rien   a=1 : JB
               a=3 : JJB    a=16 : VB    a=18: VJ   a=17 : JVB   */
   int u = get_grille(d,c-1,l);
   if (u==0) {
       if (get_grille(d,c-2,l)!=joueur) a=16;
       else if (get_grille(d,c-3,l)!=joueur) a = 18;
       else { set_promesse(d,c-1,l,joueur); a=0; }
   } else if (u==joueur) {
       u = get_grille(d,c-2,l);
       if (u==0) {
          if (get_grille(d,c-3,l)!=joueur) a=17;
          else { set_promesse(d,c-2,l,joueur); a=1; }
       } else if (u==joueur) {
          if (get_grille(d,c-3,l)==0) { set_promesse(d,c-3,l,joueur); }
          a=3;
       } else a=1;
   } else a=0;
   u = get_grille(d,c+1,l);
   if (u==0) {
       if (get_grille(d,c+2,l)!=joueur) b=16;
       else if (get_grille(d,c+3,l)!=joueur) b = 18;
       else { set_promesse(d,c+1,l,joueur); b=0; }
   } else if (u==joueur) {
       u = get_grille(d,c+2,l);
       if (u==0) {
          if (get_grille(d,c+3,l)!=joueur) b=17;
          else { set_promesse(d,c+2,l,joueur); b=1; }
       } else if (u==joueur) {
          if (get_grille(d,c+3,l)==0) { set_promesse(d,c+3,l,joueur); }
          b=3;
       } else b=1;
   } else b=0;
   if (a>=16) {
       if (b==3)
           set_promesse(d,c-1-(a&1),l,joueur);
       else if ((b & 1) && (a>16))
           set_promesse(d,c-1-(a&1),l,joueur);
   }
   if (b>=16) {
       if (a==3)
           set_promesse(d,c+1+(b&1),l,joueur);
       else if ((a & 1) && (b>16))
           set_promesse(d,c+1+(b&1),l,joueur);
   }
   /* up-right */
   u = get_grille(d,c-1,l-1);
   if (u==0) {
       if (get_grille(d,c-2,l-2)!=joueur) a=16;
       else if (get_grille(d,c-3,l-3)!=joueur) a = 18;
       else { set_promesse(d,c-1,l-1,joueur); a=0; }
   } else if (u==joueur) {
       u = get_grille(d,c-2,l-2);
       if (u==0) {
          if (get_grille(d,c-3,l-3)!=joueur) a=17;
          else { set_promesse(d,c-2,l-2,joueur); a=1; }
       } else if (u==joueur) {
          if (get_grille(d,c-3,l-3)==0) { set_promesse(d,c-3,l-3,joueur); }
          a=3;
       } else a=1;
   } else a=0;
   u = get_grille(d,c+1,l+1);
   if (u==0) {
       if (get_grille(d,c+2,l+2)!=joueur) b=16;
       else if (get_grille(d,c+3,l+3)!=joueur) b = 18;
       else { set_promesse(d,c+1,l+1,joueur); b=0; }
   } else if (u==joueur) {
       u = get_grille(d,c+2,l+2);
       if (u==0) {
          if (get_grille(d,c+3,l+3)!=joueur) b=17;
          else { set_promesse(d,c+2,l+2,joueur); b=1; }
       } else if (u==joueur) {
          if (get_grille(d,c+3,l+3)==0) { set_promesse(d,c+3,l+3,joueur); }
          b=3;
       } else b=1;
   } else b=0;
   if (a>=16) {
       if (b==3)
           set_promesse(d,c-1-(a&1),l-1-(a&1),joueur);
       else if ((b & 1) && (a>16))
           set_promesse(d,c-1-(a&1),l-1-(a&1),joueur);
   }
   if (b>=16) {
       if (a==3)
           set_promesse(d,c+1+(b&1),l+1+(b&1),joueur);
       else if ((a & 1) && (b>16))
           set_promesse(d,c+1+(b&1),l+1+(b&1),joueur);
   }
   /* down-right */
   u = get_grille(d,c-1,l+1);
   if (u==0) {
       if (get_grille(d,c-2,l+2)!=joueur) a=16;
       else if (get_grille(d,c-3,l+3)!=joueur) a = 18;
       else { set_promesse(d,c-1,l+1,joueur); a=0; }
   } else if (u==joueur) {
       u = get_grille(d,c-2,l+2);
       if (u==0) {
          if (get_grille(d,c-3,l+3)!=joueur) a=17;
          else { set_promesse(d,c-2,l+2,joueur); a=1; }
       } else if (u==joueur) {
          if (get_grille(d,c-3,l+3)==0) { set_promesse(d,c-3,l+3,joueur); }
          a=3;
       } else a=1;
   } else a=0;
   u = get_grille(d,c+1,l-1);
   if (u==0) {
       if (get_grille(d,c+2,l-2)!=joueur) b=16;
       else if (get_grille(d,c+3,l-3)!=joueur) b = 18;
       else { set_promesse(d,c+1,l-1,joueur); b=0; }
   } else if (u==joueur) {
       u = get_grille(d,c+2,l-2);
       if (u==0) {
          if (get_grille(d,c+3,l-3)!=joueur) b=17;
          else { set_promesse(d,c+2,l-2,joueur); b=1; }
       } else if (u==joueur) {
          if (get_grille(d,c+3,l-3)==0) { set_promesse(d,c+3,l-3,joueur); }
          b=3;
       } else b=1;
   } else b=0;
   if (a>=16) {
     //   printf("%d %d c=%d l=%d %d\n",a,b,c,l,joueur);
       if (b==3)
           set_promesse(d,c-1-(a&1),l+1+(a&1),joueur);
       else if ((b & 1) && (a>16))
           set_promesse(d,c-1-(a&1),l+1+(a&1),joueur);
   }
   if (b>=16) {
       if (a==3)
           set_promesse(d,c+1+(b&1),l-1-(b&1),joueur);
       else if ((a & 1) && (b>16))
           set_promesse(d,c+1+(b&1),l-1-(b&1),joueur);
   }
}
#if 0
/* check if (c,l) is a start of 4 line : up, up_right, right, down-right */
int ligne_de_4 (damier *d, int c, int l) {
   int u;
   int coul = d->grille[l][c];
   if (coul==0) return 0;
   /* up */
   if (l<SIZE_LIGNES-3) {
      u=1;
      while (u<4 && d->grille[l+u][c]==coul) u++;
      if (u==4) { for (u=0;u<4;u++) d->grille[l+u][c]=2*coul; return coul; }
   }
   if (!WRAP && (c>=SIZE_COLONNES-3)) return 0;
   /* up-right */
   if (l<SIZE_LIGNES-3) {
      u=1;
      while (u<4 && d->grille[l+u][(c+u)%SIZE_COLONNES]==coul) u++;
      if (u==4) { for (u=0;u<4;u++) d->grille[l+u][(c+u)%SIZE_COLONNES]=2*coul; return coul; }
   }
   /* right */
   {
      u=1;
      while (u<4 && d->grille[l][(c+u)%SIZE_COLONNES]==coul) u++;
      if (u==4) { for (u=0;u<4;u++) d->grille[l][(c+u)%SIZE_COLONNES]=2*coul; return coul; }
   }
   /* down-right */
   if (l>=3) {
      u=1;
      while (u<4 && d->grille[l-u][(c+u)%SIZE_COLONNES]==coul) u++;
      if (u==4) { for (u=0;u<4;u++) d->grille[l-u][(c+u)%SIZE_COLONNES]=2*coul; return coul; }
   }
   return 0;
}
#endif
#if 0
int identify_hole(damier *d, int c, int l, int *even_win, int *odd_win) {
    int tminus = ((*even_win & 2) == 0 && (*odd_win & 1)==0);
    int tplus = ((*odd_win & 2) == 0 && (*even_win & 1)==0);
    int wminus = 0;
    int wplus = 0;
    if (tplus==0 && tminus==0) return 0;

    /* horizontal */
    int pp=0, pm=0, k=1, ltm=tminus, ltp=tplus;
    while ((WRAP || c+k<SIZE_COLONNES) && (ltm || ltp)) {
        if (d->grille[l][(c+k)%SIZE_COLONNES]==0) { ltm=ltp=0; break; }
        if (d->grille[l][(c+k)%SIZE_COLONNES]==1) { ltm=0; if (ltp) { pp++; ltp=(pp<4); } }
                                             else { ltp=0; if (ltm) { pm++; ltm=(pm<4); } }
        k++;
    }
    k=1; ltm=tminus; ltp=tplus;
    while ((WRAP || c-k>=0) && (ltm || ltp)) {
        if (d->grille[l][(c-k+SIZE_COLONNES)%SIZE_COLONNES]==0) { ltm=ltp=0; break; }
        if (d->grille[l][(c-k+SIZE_COLONNES)%SIZE_COLONNES]==1) { ltm=0; if (ltp) { pp++; ltp=(pp<4); } }
                                            else { ltp=0; if (ltm) { pm++; ltm=(pm<4); } }
        k++;
    }
    if (tplus && pp>=3) { tplus=0; wplus=1; }
    if (tminus && pm>=3) { tminus=0; wminus=1; }
    /* diagonal 1 */
    pp=0; pm=0; k=1; ltm=tminus; ltp=tplus;
    while ((WRAP || c+k<SIZE_COLONNES) && (l+k<SIZE_LIGNES) && (ltm || ltp)) {
        if (d->grille[l+k][(c+k)%SIZE_COLONNES]==0) { ltm=ltp=0; break; }
        if (d->grille[l+k][(c+k)%SIZE_COLONNES]==1) { ltm=0; if (ltp) { pp++; ltp=(pp<4); } }
                                                else { ltp=0; if (ltm) { pm++; ltm=(pm<4); } }
        k++;
    }
    k=1; ltm=tminus; ltp=tplus;
    while ((WRAP || c-k>=0) && (ltm || ltp) && (l-k>=0)) {
        if (d->grille[l-k][(c-k+SIZE_COLONNES)%SIZE_COLONNES]==0) { ltm=ltp=0; break; }
        if (d->grille[l-k][(c-k+SIZE_COLONNES)%SIZE_COLONNES]==1) { ltm=0; if (ltp) { pp++; ltp=(pp<4); } }
                                                else { ltp=0; if (ltm) { pm++; ltm=(pm<4); } }
        k++;
    }
    if (tplus && pp>=3) { tplus=0; wplus=1; }
    if (tminus && pm>=3) { tminus=0; wminus=1; }
    /* diagonal 2 */
    pp=0; pm=0; k=1; ltm=tminus; ltp=tplus;
    while ((WRAP || c+k<SIZE_COLONNES) && (l-k>=0) && (ltm || ltp)) {
        if (d->grille[l-k][(c+k)%SIZE_COLONNES]==0) { ltm=ltp=0; break; }
        if (d->grille[l-k][(c+k)%SIZE_COLONNES]==1) { ltm=0; if (ltp) { pp++; ltp=(pp<4); } }
                                                else { ltp=0; if (ltm) { pm++; ltm=(pm<4); } }
        k++;
    }
    k=1; ltm=tminus; ltp=tplus;
    while ((WRAP || c-k>=0) && (ltm || ltp) && (l+k<SIZE_LIGNES)) {
        if (d->grille[l+k][(c-k+SIZE_COLONNES)%SIZE_COLONNES]==0) { ltm=ltp=0; break; }
        if (d->grille[l+k][(c-k+SIZE_COLONNES)%SIZE_COLONNES]==1) { ltm=0; if (ltp) { pp++; ltp=(pp<4); } }
                                                else { ltp=0; if (ltm) { pm++; ltm=(pm<4); } }
        k++;
    }
    if (tplus && pp>=3) { tplus=0; wplus=1; }
    if (tminus && pm>=3) { tminus=0; wminus=1; }

    if (wplus) (*even_win) = (*even_win) | 1;
    if (wminus) (*even_win) = (*even_win) | 2;
    return wplus - wminus;
}
#endif
int calcul_fin_partie(partie *p, int l, int c, int joueur) {
    int score=0;
    int i;
    if (p->position.promesse[l][c] & (1+(joueur==JBLANC))) {
        p-> termine=1;
//        p->position.grille[l][c] = 2*p->position.grille[l][c];
        p->position.score = VICTORYSCORE*joueur;  return 1;
    }

    if (p->num_coups==SIZE_COLONNES*SIZE_LIGNES) { p-> termine=1; p->position.score=0; return 1; }
    p->position.promesse[l][c]=0;
    adapte_damier(&(p->position),c,l,joueur);

    /* score : aligné 2 dessous : 1 aligné 3 dessous : 10 */
    /* trou à 3 direct : bonus de 5 ; indirect : 5 ; double parité : 20 */
    for (i=0;i<SIZE_COLONNES;i++) {
        int u=0; while (u<SIZE_LIGNES && p->position.grille[u][i]!=0) { u++; }
        int j;
        int pair_win = 0, impair_win = 0, tmp_win;
        for (j=u;j<SIZE_LIGNES;j++) {
          if (((pair_win&1)==0) && ((impair_win&2)==0) && (p->position.promesse[j][i] & 1)) {
                pair_win |=1;
                if (j==u) score-=2; }
          if (((pair_win&2)==0) && ((impair_win&1)==0) && (p->position.promesse[j][i] & 2)) {
                pair_win |=2;
                if (j==u) score+=2; }
          if (p->position.promesse[j][i] & 1) score--;
          if (p->position.promesse[j][i] & 1) score++;
          tmp_win = pair_win; pair_win = impair_win; impair_win = tmp_win;
        }
        if ((pair_win & impair_win) ==1) score -=20;
        else if ((pair_win & impair_win) ==2) score +=20;
        if (pair_win==1) score-=7; else if (pair_win ==2) score+=7;
        if (impair_win ==1) score-=7; else if (impair_win==2) score+=7;
    }
 #if 0

        u=0; while (u<SIZE_LIGNES && p->position.grille[u][i]!=0) {
            if (p->position.grille[u][i]==cp) k++; else { cp = p->position.grille[u][i]; k=1; }
            u++;
        }
        /* score de 3 dessous */
        if (k>=2) score += cp*(k==3 ? 10 : 1);
        int pair_win = 0, impair_win = 0;
        for (j=0;j<SIZE_LIGNES-u;j++) {
            int a;
            a = identify_hole(&(p->position),i,j+u,(j%2==0 ? &pair_win : &impair_win),
                                                    (j%2==0 ? &impair_win : &pair_win));
            if ((a!=0) && (j==0)) score += cp*a*5;
        }
        if ((pair_win & impair_win)==1) score +=10;
        if ((pair_win & impair_win)==2) score -=10;
        if (pair_win==1) score+=5; if (pair_win ==2) score-=5;
        if (impair_win ==1) score+=5; if (impair_win==2) score-=5;
    }
#endif
    p->termine=0;
    p->position.score=score;
    return 0;
}

/* nouveau mouvement */
/* note : c->ligne==-1 : passe son tour */
partie *joue_coup_suivant(partie *p, mouvement *c) {
    int ok = coup_possible(p,c);
    /* on vérifie, quand même, que le coup est valide */
    if (ok==-1) {
        fprintf(stderr,"out-of-bound move\n");
        exit(-1);
    }
    if (ok<=0) {
        fprintf(stderr,"tentative de coup invalide!\n");
        exit(-10+ok);
    }
    {
        partie *newpart;
        if (p->position_suiv==NULL)
        {
            p->position_suiv = calloc(1,sizeof(partie));
        }
        newpart = p->position_suiv;
        newpart->position_prec=p;
        memset(&(newpart->coup_suivant),0,sizeof(mouvement));
        newpart->num_coups = p->num_coups+1;
        memcpy(&(p->coup_suivant),c,sizeof(mouvement));
        memcpy(&(newpart->position),&(p->position),sizeof(damier));

        int u = SIZE_LIGNES-1;
        while (u>=0 && newpart->position.grille[u][c->colonne]==0) u--;
        assert(u<SIZE_LIGNES-1);
        newpart->position.grille[u+1][c->colonne]=newpart->position.joueur;
        newpart->position.joueur = -p->position.joueur;
        calcul_fin_partie(newpart,u+1,c->colonne,p->position.joueur);
        return newpart;
    }
}

partie *parcours_arriere(partie *p) {
    partie *pprec = p->position_prec;
    if (pprec==NULL || pprec->coup_suivant.joueur==0) return NULL;
    return pprec;
}
partie *parcours_avant(partie *p) {
    if (p->coup_suivant.joueur==0) return NULL;
    return p->position_suiv;
}

void bloque_avenir(partie *p) {
    memset(&(p->coup_suivant),0,sizeof(mouvement));
}

partie *duplique_position_actuelle(partie *p) {
    partie * newpart = calloc(1,sizeof(partie));
    memcpy(newpart,p,sizeof(partie));
    bloque_avenir(newpart);
    newpart->position_suiv=NULL;
    newpart->position_prec=NULL;
    return newpart;
}
