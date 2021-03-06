#ifndef CA_SOLUTION_H
#define CA_SOLUTION_H

using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "Mouvement.h"

class CA_Solution
{
public:
	CA_Solution(int val, int col, vector<int> resultat);
    CA_Solution(int val, int col, int lignes);
    CA_Solution(const CA_Solution& sol);
    ~CA_Solution();

    void ecrireFichier(string chemin);
    int verifierSolution();
    int verifierSolution(Mouvement mv);
    void enleverLigne();
    Mouvement mouvementSuivant();
    void reinitialiserMouvement();
    void mouvementCritiqueSuivant();
    void reinitialiserMouvementCritique();
    Mouvement mouvementCourant(){return mvtCourant;}
    Mouvement* mouvementCritique(){return &mvtCourant;}
    Mouvement mouvementAleatoire();
    void appliquerMouvement(Mouvement mv);

	int v;
	int k;
	int N;
	vector<int> solution;
    int nbIt, nbMvt;
    int erreurs, erreursDernierMv;

private:
    void trouverMouvementSuivant();
    void allouerMemoire();
    int calculerDelta(Mouvement mv);
    Mouvement mvtCourant;
    int ****occurencesCouples, *sousContraintesColonnes, **sousContraintesSymboles, ***deltas;
    int nbContraintes;
};

#endif //CA_SOLUTION_H
