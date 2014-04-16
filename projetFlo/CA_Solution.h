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
#include "utils.h"

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
    void mouvementSuivant();
    void reinitialiserMouvement();
    void mouvementCritiqueSuivant();
    void reinitialiserMouvementCritique();
    Mouvement* mouvementCourant(){return &mvtCourant;}
    Mouvement* mouvementCritique(){return &mvtCourant;}
    Mouvement mouvementAleatoire();
    void appliquerMouvement(Mouvement mv);
    void allouerMemoire();

	int v;
	int k;
	int N;
    vector<int> solution;
    int nbIt, nbMvt;
    int erreurs, erreursDernierMv;

private:
    void trouverMouvementSuivant();

    int calculerDelta(Mouvement mv);
    Mouvement mvtCourant;
    int ****occurencesCouples, *sousContraintesColonnes, **sousContraintesSymboles, ***deltas;
    int nbContraintes;
};

inline bool operator< (const CA_Solution& lhs, const CA_Solution& rhs)
{
    unsigned int ind = 0;
    while(lhs.solution[ind] == rhs.solution[ind])
    {
        ind++;
        if(ind >= lhs.solution.size())
        {
            return false;
        }
    }
    return lhs.solution[ind] < rhs.solution[ind];
}

#endif //CA_SOLUTION_H
