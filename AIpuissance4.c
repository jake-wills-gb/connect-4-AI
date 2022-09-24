#include "puissance4.h"
#include "AIpuissance4.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int max(int a, int b){if(a>=b){return a;}return b;}
/* joueur par défaut (-1) : cette fonction fait juste un appel à AIplayer[0-5] */
int AIplayer(partie *p, mouvement *m) {
    return AIplayer0(p,m);
}

/* joueur 0 : sans algorithme minimax, illustre l'usage des fonctions manipulant les parties */
int AIplayer0(partie *p, mouvement *m) {
   liste_mouvements lm;
   int i;
   int bscore = -2*VICTORYSCORE;
   int bmove=-1;
   int nb_bscore=1;
   int nb_moves= remplit_liste_mouvements(p,&lm);
   for (i=0;i<nb_moves;i++) {
       partie *p2;
       int scoretmp;
       p2=joue_coup_suivant(p,&(lm.liste[i]));
       scoretmp = get_score(p2) * get_joueur(p);
       if (scoretmp>bscore) { bscore=scoretmp; bmove=i; nb_bscore=1; }
       else if ((scoretmp==bscore) && (rand()%(++nb_bscore)==0)) {
            bscore=scoretmp; bmove=i;
       }
   }
   memcpy(m,&(lm.liste[bmove]),sizeof(mouvement));
   return bscore;
}


////////////////////////////////////////////////
//////////////// AIplayer1 /////////////////////
////////////////////////////////////////////////
int negamax1(partie *p, int profondeur){
    if(p->termine || profondeur == 0){
        return get_score(p) * get_joueur(p);
    }
    int score = -2*VICTORYSCORE;
    liste_mouvements lm;
	int nb_moves = remplit_liste_mouvements(p, &lm);
    for(int i=0;i<nb_moves;i++){
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = negamax1(p2, profondeur-1);
		score = max(scoretmp,score);
    }
    return score;
}

int AIplayer1(partie *p, mouvement *m) {
	liste_mouvements lm;
	int i;
	int profondeur = 4;
	int bscore = -2*VICTORYSCORE;
	int bmove=-1;
	int nb_moves= remplit_liste_mouvements(p,&lm);
	for (i=0;i<nb_moves;i++) {
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = -negamax1(p2, profondeur);
		if (scoretmp>bscore) { bscore=scoretmp; bmove=i; }
		/*else if ((scoretmp==bscore) && (rand()%(++nb_bscore)==0)){
			bscore=scoretmp; bmove=i;
       }*/
	}
	memcpy(m,&(lm.liste[bmove]),sizeof(mouvement));
	return bscore;

}

////////////////////////////////////////////////
//////////////// AIplayer2 /////////////////////
////////////////////////////////////////////////
int negamax2(partie *p, int profondeur){
    if(p->termine || profondeur == 0){
        return get_score(p) * get_joueur(p);
    }
    int score = -2*VICTORYSCORE;
    liste_mouvements lm;
	int nb_moves = remplit_liste_mouvements(p, &lm);
    for(int i=0;i<nb_moves;i++){
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = negamax2(p2, profondeur/nb_moves);
		score = max(scoretmp,score);
    }
    return score;
}

int AIplayer2(partie *p, mouvement *m) {
    liste_mouvements lm;
	int i;
	int profondeur = 10000;
	int bscore = -2*VICTORYSCORE;
	int bmove=-1;
	int nb_moves= remplit_liste_mouvements(p,&lm);
	for (i=0;i<nb_moves;i++) {
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = -negamax2(p2, (profondeur/nb_moves));
		if (scoretmp>bscore) { bscore=scoretmp; bmove=i; }
	}
	memcpy(m,&(lm.liste[bmove]),sizeof(mouvement));
	return bscore;
}

////////////////////////////////////////////////
//////////////// AIplayer3 /////////////////////
////////////////////////////////////////////////

int negamax3(partie *p, int profondeur){
    if(p->termine || profondeur == 0){
        return (get_score(p)+profondeur) * get_joueur(p);
    }
    int score = -2*VICTORYSCORE;
    liste_mouvements lm;
	int nb_moves = remplit_liste_mouvements(p, &lm);
    for(int i=0;i<nb_moves;i++){
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = negamax3(p2, profondeur/nb_moves);
		score = max(scoretmp,score);
    }
    return score+profondeur;
}

int AIplayer3(partie *p, mouvement *m) {
    liste_mouvements lm;
	int i;
	int profondeur = 10000;
	int bscore = -2*VICTORYSCORE;
	int bmove=-1;
	int nb_moves= remplit_liste_mouvements(p,&lm);
	for (i=0;i<nb_moves;i++) {
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = -negamax3(p2, (profondeur/nb_moves));
		if (scoretmp>bscore) { bscore=scoretmp; bmove=i; }
		/*else if ((scoretmp==bscore) && (rand()%(++nb_bscore)==0)){
			bscore=scoretmp; bmove=i;
       }*/
	}
	memcpy(m,&(lm.liste[bmove]),sizeof(mouvement));
	return bscore;
}

////////////////////////////////////////////////
//////////////// AIplayer4 /////////////////////
////////////////////////////////////////////////
int negamax4(partie *p, int profondeur, int a, int b){
    if(p->termine || profondeur == 0){
        return (get_score(p)+profondeur) * get_joueur(p);
    }
    int score = -2*VICTORYSCORE;
    liste_mouvements lm;
	int nb_moves = remplit_liste_mouvements(p, &lm);
    for(int i=0;i<nb_moves;i++){
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = negamax4(p2, profondeur/nb_moves, -a, -b);
		score = max(scoretmp,score);
		a = max(score, a);
		if(a>=b){
            return score+profondeur;
		}
    }
    return score+profondeur;
}
int AIplayer4(partie *p, mouvement *m) {
    liste_mouvements lm;
	int i;
	int profondeur = 10000;
	int bscore = -2*VICTORYSCORE;
	int bmove=-1;
	int nb_moves= remplit_liste_mouvements(p,&lm);
	for (i=0;i<nb_moves;i++) {
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = -negamax4(p2, (profondeur/nb_moves),-2*VICTORYSCORE,VICTORYSCORE);
		if (scoretmp>bscore) { bscore=scoretmp; bmove=i; }
	}
	memcpy(m,&(lm.liste[bmove]),sizeof(mouvement));
	return bscore;
}

////////////////////////////////////////////////
//////////////// AIplayer5 /////////////////////
////////////////////////////////////////////////


int negamax5(partie *p, int profondeur, int a, int b){
    if(p->termine || profondeur == 0){
        return (get_score(p)+profondeur) * get_joueur(p);
    }
    int score = -2*VICTORYSCORE;
    liste_mouvements lm;
	int nb_moves = remplit_liste_mouvements(p, &lm);
	tri_liste_mouvements(p,&lm);
    for(int i=0;i<nb_moves;i++){
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = negamax5(p2, profondeur/nb_moves, -a, -b);
		score = max(scoretmp,score);
		a = max(score, a);
		if(a>=b){
            return score+profondeur;
		}
    }
    return score+profondeur;
}
int AIplayer5(partie *p, mouvement *m) {
    liste_mouvements lm;
	int i;
	int profondeur = 10000;
	int bscore = -2*VICTORYSCORE;
	int bmove=-1;
	int nb_moves= remplit_liste_mouvements(p,&lm);
	tri_liste_mouvements(p,&lm);
	for (i=0;i<nb_moves;i++) {
		partie *p2;
		int scoretmp;
		p2=joue_coup_suivant(p,&(lm.liste[i]));
		scoretmp = -negamax5(p2, (profondeur/nb_moves),-2*VICTORYSCORE,VICTORYSCORE);
		if (scoretmp>bscore) { bscore=scoretmp; bmove=i; }
	}
	memcpy(m,&(lm.liste[bmove]),sizeof(mouvement));
	return bscore;
}
