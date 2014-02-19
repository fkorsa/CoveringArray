#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <chrono>

using namespace std;

#include "CA_Solution.h"
#include "Mouvement.h"

// Lire une matrice a partir d'un fichier
CA_Solution* lireFichierSolution(const char* chemin)
{
	ifstream infile(chemin);
	int v = 0;
	int k = 0;
	int N = 0;
	int erreurs = 0;

	infile >> v >> k >> N >> erreurs;

	vector<int> solution(N*k);
	int index = 0;
	int val = 0;

	while (infile >> val)
	{
		solution[index] = val;
		index++;
	}
	CA_Solution* ca_sol = new CA_Solution(v,k,solution);
	return ca_sol;
}

// Generer une matrice aleatoirement
CA_Solution* configurationAleatoire(int v, int k, int N) {
	vector<int> matrice(N*k);

    for(int i=0; i<N*k; i++)
    {
		int symb = rand()%v;
        matrice[i] = symb;
	}

	CA_Solution* configuration = new CA_Solution(v, k, matrice);
	return configuration;
}

// Algorithme tabou
CA_Solution* tabou(CA_Solution* configInit, int nombreEssais, ofstream *fichier, int longueurListe)
{
    Mouvement mouvementActuel, meilleurMouvement;
    ofstream& fichierLocal = *fichier;
    int itSansMvtCpt = 1, totalIt = 1;
    int vraisMouvementsTotal = 0, fmin, fmax, coutTest, coutActuelle, coutMin;
    int k = configInit->k, v = configInit->v, N = configInit->N;
	CA_Solution* configTestee = configInit; // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configInit); // Meilleures des configurations testées jusqu'alors
    double delta, deltaMin;
    bool minDefini, premiereIteration;

    // Initialisation de la liste taboue : grande matrice contenant le numero de l'iteration jusqu'a
    // laquelle l'attribut est tabou
    // Un attribut est de la forme (ligne, colonne, symbole)
    int ***listeTaboue;
    listeTaboue = new int**[N];
    for(int i1=0; i1<N; i1++)
    {
        listeTaboue[i1] = new int*[k];
        for(int i2=0; i2<k; i2++)
        {
            listeTaboue[i1][i2] = new int[v];
            for(int i3=0; i3<v; i3++)
            {
                listeTaboue[i1][i2][i3] = 0;
            }
        }
    }

    int coutMeilleure = configInit->verifierSolution();
    meilleureConfig->erreurs = coutMeilleure;
    fmin = fmax = coutMeilleure;

    while(itSansMvtCpt < nombreEssais && (coutMeilleure > 0 || fichier))
    {
        coutActuelle = configTestee->erreurs;
        minDefini = false;
        premiereIteration = true;
        configTestee->reinitialiserMouvementCourant();
        mouvementActuel = configTestee->mouvementCourant();
        while(!mouvementActuel.estFinal)
        {
            // La methode mouvement() parcourt tous les mouvements possibles : a chaque appel,
            // elle renvoie le mouvement courant puis prend le mouvement suivant dans l'espace
            // des mouvements possibles.
            if(!premiereIteration)
            {
                mouvementActuel = configTestee->mouvementSuivant();
            }
            premiereIteration = false;
            coutTest = configTestee->verifierSolution(mouvementActuel);
            delta = coutTest - coutActuelle;
            if((listeTaboue[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mSymbole] < totalIt
                // Critere d'aspiration
                || coutTest < coutMeilleure)
                && (!minDefini || delta < deltaMin))
            {
                deltaMin = delta;
                coutMin = coutTest;
                meilleurMouvement = mouvementActuel;
                minDefini = true;
            }
        }

		// Application des conséquences
        if(minDefini)
        {
            configTestee->appliquerMouvement(meilleurMouvement); // Déplacement entériné
            listeTaboue[meilleurMouvement.mLigne][meilleurMouvement.mCol][meilleurMouvement.mAncienSymbole] = totalIt + longueurListe;
            if(coutMin < coutMeilleure)
            { // Mise à jour de la meilleure configuration (pas forcément S' si on a tiré au sort en faveur de la dégradation)
                delete meilleureConfig;
                meilleureConfig = new CA_Solution(*configTestee);
                coutMeilleure = coutMin;
                itSansMvtCpt = 0;
            }
            vraisMouvementsTotal++;
        }
        itSansMvtCpt++;

        totalIt++;
        // Enregistrement des parametres de la simulation
        if(fichier && totalIt%50 == 0)
        {
            fichierLocal << totalIt << " " << coutActuelle << endl;
        }
	}
    meilleureConfig->nbIt = totalIt;
    meilleureConfig->nbMvt = vraisMouvementsTotal;
	return(meilleureConfig);
}

int main()
{
    int seed = time(NULL);
    //srand(seed);
    CA_Solution* configInit = configurationAleatoire(3, 60, 21);
    CA_Solution* configTabou = tabou(configInit, 50000, NULL, 40);
    cout << "Erreurs : " << configTabou->erreurs << " iterations : " << configTabou->nbIt << endl;
}
