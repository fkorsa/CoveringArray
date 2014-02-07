#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <unistd.h>

using namespace std;

#include "CA_Solution.h"
#include "Mouvement.h"


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


CA_Solution* recuitSimule(CA_Solution* configInit, int tempInit, int coeff, ofstream* fichier) {
    int nombreEssais = 2000; // Condition d'arrêt
    Mouvement mouvementActuel;
    ofstream& fichierLocal = *fichier;
    double T = tempInit;
    int sleepCpt = 0, itCpt = 0, statiqueCpt = 0, totalIt = 0;
    //trial mov fmin fmax acc
    int vraisMouvement = 0, fmin, fmax, tauxAcceptation;
	CA_Solution* configTestee = configInit; // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configInit); // Meilleures des configurations testées jusqu'alors

	int coutMeilleure = meilleureConfig->verifierSolution();

    if(fichier)
    {

    }

    while(statiqueCpt < nombreEssais)
    {
		// On génère un voisin aléatoire de S
        mouvementActuel = configTestee->mouvement();
        int coutTest = configTestee->verifierSolution(mouvementActuel);
        int coutActuelle = configTestee->verifierSolution();
		int delta = coutTest - coutActuelle;

		// On teste le critère de Métropolis
		bool metropolis;
        if(delta <= 0)
        {
			metropolis = true;
        }
        else
        {
			double prob = exp(-delta/T);
			int seuilTirage = 100*prob;
			int numTire = rand()%100;
			metropolis = (numTire <= seuilTirage);
		}

		// Application des conséquences
        if(metropolis)
        {
            configTestee->appliquerMouvement(mouvementActuel); // Déplacement entériné
            if(coutTest < coutMeilleure)
            { // Mise à jour de la meilleure configuration (pas forcément S' si on a tiré au sort en faveur de la dégradation)
                delete meilleureConfig;
                meilleureConfig = new CA_Solution(*configTestee);
				coutMeilleure = coutTest;
                statiqueCpt = 0;
            }
        }
        statiqueCpt++;

        if(sleepCpt > 100)
        {
            usleep(100);
            sleepCpt = 0;
        }
        sleepCpt++;
        if(itCpt > 100)
        {
            itCpt = 0;
            // Refroidissement (à élaborer encore plus)
            T*=coeff;
        }
        itCpt++;
        totalIt++;
	}
    meilleureConfig->nbIt = totalIt;
	return(meilleureConfig);
}

void testerParametres()
{

}

int main()
{
    int seed = time(NULL);
    srand(seed);
    CA_Solution* ancienneConfig = configurationAleatoire(3, 20, 23);
    CA_Solution* bestConfig = new CA_Solution(*ancienneConfig);
    CA_Solution* configRecuit = recuitSimule(ancienneConfig, 1);
    delete ancienneConfig;
    while(configRecuit->verifierSolution() == 0)
    {
        delete bestConfig;
        bestConfig = new CA_Solution(*configRecuit);
        configRecuit->enleverLigne();
        ancienneConfig = configRecuit;
        configRecuit = recuitSimule(configRecuit, 1);
        cout << "Nombre d'iterations pour cette simulation : " << configRecuit->nbIt << endl;
        delete ancienneConfig;
    }
    cout << "Nombre de lignes : " << bestConfig->N << endl;
    delete bestConfig;
    delete configRecuit;

    double temp[] = {1.6, 0.8, 0.4, 0.2, 0.1};
    ofstream fichier("resultsParam");

    if(fichier)
    {
        for(indexTemp = 0; indexTemp<sizeof(temp); indexTemp++)
        {
            fichier << "temp seed " << temp[indexTemp] << " " << seed << endl;
            fichier << "trial mov fmin fmax acc" << endl;
            configRecuit = recuitSimule(ancienneConfig, temp[indexTemp], 1, &fichier);
        }
        fichier.close();
    }
    else {
        cerr << "Impossible d'ouvrir le fichier !" << endl; }
}
