#ifndef GENERATEUR_H
#define GENERATEUR_H

#include <vector>
#include <chrono>
#include <list>
#include <math.h>

#include "resultats.h"
#include "CA_Solution.h"

using namespace std;

class Generateur
{
public:
    Generateur(int nbExec);
    void TesterTout();
private:
    enum ALGO_TYPE
    {
        GLOUTON,
        DESCENTE,
        TABOU,
        TABOU_DIVERSIFIE,
        RECUIT_SIMULE,
        EVOLUTION
    };
    enum TYPE_CROISEMENT
    {
        CROISEMENT_SYMBOLE,
        CROISEMENT_LIGNE
    };
    void GenererResultats(ALGO_TYPE type);

    // Glouton
    Resultats TesterGlouton(int v, int k);
    vector<int> couvertureGlouton(int v, int k, float tolerance);

    // Descente
    int DescenteChoixMouvement(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs);
    Resultats TesterDescente(int v, int k, int N);

    // Evolution
    Resultats TesterEvolution(int v, int k, int N, int tailleParents, int tailleEnfants, float pourcentMutation, TYPE_CROISEMENT type);
    void EvolutionCalculerCouts(CA_Solution **population, int *couts, int taille);
    void EvolutionCroisementSymbole(CA_Solution **population, int tailleParents, int tailleEnfants, int v, int k, int N);
    void EvolutionCroisementLigne(CA_Solution **population, int tailleParents, int tailleEnfants, int v, int k, int N);
    void EvolutionMutation(CA_Solution **populationEnfants, int tailleEnfants, int v, int k, int N, float pourcent);

    // Tabou
    Resultats TesterTabou(CA_Solution* configInit, int longueurListe, bool diversification);
    void TabouChoixDiversification(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration,
                              bool*** presence, int ***dernierePresence, int ***frequence, int coutMeilleure);
    void TabouChoixMouvement(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration, int coutMeilleure);


    Resultats TesterRecuitSimule(CA_Solution* configInit, double tempInit, double coeff);
    CA_Solution* ConfigurationAleatoire(int v, int k, int N);
    int m_tempsMax, m_nbExec;
};

#endif // GENERATEUR_H
