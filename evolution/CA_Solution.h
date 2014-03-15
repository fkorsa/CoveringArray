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
    void reinitialiserMouvementCourant();
    Mouvement mouvementCourant(){return mvtCourant;}
    void appliquerMouvement(Mouvement mv);

	int v;
	int k;
	int N;
	vector<int> solution;
    int nbIt, nbMvt;
    int nbContraintes;
    int erreurs, erreursDernierMv;
    bool ****contraintes;
    bool **copieContraintesAncien, **copieContraintesNouveau;
    Mouvement mvtCourant;
};

#endif //CA_SOLUTION_H
