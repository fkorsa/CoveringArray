#ifndef GENERATEUR_H
#define GENERATEUR_H

#include <vector>
#include <chrono>
#include <list>
#include <math.h>
#include <set>

#include "resultats.h"
#include "CA_Solution.h"

using namespace std;

class Generateur
{
public:
    Generateur(int nbExec, float dfmax);
    void TesterTout();
private:
    enum ALGO_TYPE
    {
        GLOUTON,
        DESCENTE,
        TABOU,
        TABOU_DIVERSIFIE,
        TABOU_MATRICE,
        TABOU_DIVERSIFIE_MATRICE,
        RECUIT_SIMULE,
        EVOLUTION
    };
    enum TYPE_CROISEMENT
    {
        CROISEMENT_SYMBOLE,
        CROISEMENT_LIGNE
    };
    void GenererResultats(ALGO_TYPE type);

    // Tabou configs entieres
    Resultats TesterTabouMatrice(CA_Solution* configInit, bool diversification);
    void TabouChoixDiversificationMatrice(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, set<CA_Solution> *listeTaboue,
                              bool*** presence, int ***dernierePresence, int ***frequence);
    void TabouChoixMouvementMatrice(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, set<CA_Solution> *listeTaboue,
                             int coutMeilleure);

    // Tabou attribut
    Resultats TesterTabou(CA_Solution* configInit, int longueurListe, bool diversification);
    void TabouChoixDiversification(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration,
                              bool*** presence, int ***dernierePresence, int ***frequence, int coutMeilleure);
    void TabouChoixMouvement(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration, int coutMeilleure);

    CA_Solution* ConfigurationAleatoire(int v, int k, int N);
    int m_tempsMax, m_nbExec;
    float m_dfmax;
};

#endif // GENERATEUR_H
