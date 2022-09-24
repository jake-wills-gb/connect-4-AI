#ifndef PUISSANCE4_H_INCLUDED
#define PUISSANCE4_H_INCLUDED

extern int SIZE_LIGNES;
extern int SIZE_COLONNES;
extern int WRAP;

#define MAX_SIZE_LIGNES 8
#define MAX_SIZE_COLONNES 10
#define JNOIR  (-1)
#define JBLANC  (1)
#define VICTORYSCORE    10000

/* description d'un jeu */
typedef struct st_damier {
    int grille[MAX_SIZE_LIGNES][MAX_SIZE_COLONNES];
      /* JNOIR : pion jaune, JBLANC : pion rouge, *2 : pion gagnant */
    int promesse[MAX_SIZE_LIGNES][MAX_SIZE_COLONNES];
      /* victoire potentielle : +1 : JNOIR, +2 : JBLANC, +3 : both */
    int joueur;
    int score;
} damier;

/* mouvement */
typedef struct st_mouvement {
    int joueur;
    int colonne;
    int score;
} mouvement;

typedef struct st_lmouvements {
    mouvement liste[MAX_SIZE_COLONNES];
    int nb_moves;
} liste_mouvements;

/* partie */
typedef struct st_partie {
    damier position;
    int num_coups;
    int termine; /* 0 : rien de spécial ; 1 : terminé ; -1 : ne peut pas jouer */
    mouvement coup_suivant; /* coup_suivant.joueur = 0 => position_suiv pointe sur une "fausse" partie */
    struct st_partie *position_prec, *position_suiv;
} partie;

partie *cree_partie_initiale ();  /* crée un jeu initial */
void detruit_partie(partie *p); /* libère un jeu et tous ses successeurs */
int coup_possible (partie *p, mouvement *c);
int remplit_liste_mouvements(partie *p, liste_mouvements *lm);
static inline int get_joueur(partie *p) { return p->position.joueur; }
static inline int get_score(partie *p) { return p->position.score; }
partie *joue_coup_suivant(partie *p, mouvement *c); /* joue un coup sur le jeu en cours */
partie *parcours_arriere(partie *p); /* retourne la position precedence */
partie *parcours_avant(partie *p); /* retourne la position suivante, si elle correspond à quelque chose */
partie *parcours_present(partie *p); /* retourne la position "maximale" en avant */
void bloque_avenir(partie *p); /* reinitialise la partie au coup actuel, détruisant les coups suivants */
partie *duplique_position_actuelle(partie *p); /* duplique la position actuelle, isolée */
int calcul_fin_partie(partie *p, int l, int c, int joueur); /* calcule le score, et retourne 1 si partie terminee */
void tri_liste_mouvements(partie *p,liste_mouvements *lm);

#endif // PUISSANCE4_H_INCLUDED
