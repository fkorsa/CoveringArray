#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <ctime>

#include <windows.h>
using namespace std;

#include "CA_Solution.h"


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

	for(int i=0; i<N*k; i++){
		int symb = rand()%v;
		matrice[i] = v;
	}

	CA_Solution* configuration = new CA_Solution(v, k, matrice);
	return configuration;
}


CA_Solution* recuitSimule(CA_Solution* configInit, int tempInit) {
	int nombreEssais = 100; // Condition d'arrêt simpliste, à modifier par la suite
	int coeff = 0.5; // A changer également par la suite lorsque le schéma de refroidissement sera clairement établi

	int T = tempInit;

	CA_Solution* configTestee = configInit; // Configuration S' suite à un mouvement
	CA_Solution* configActuelle = configTestee; // Configuration S dans laquelle on se trouve
	CA_Solution* meilleureConfig = configActuelle; // Meilleures des configurations testées jusqu'alors

	int coutMeilleure = meilleureConfig->verifierSolution();

	for(int ess=0; ess<nombreEssais; ess++) {
		// On génère un voisin aléatoire de S
		configTestee->mouvement();
		int coutTest = configTestee->verifierSolution();
		int coutActuelle = configActuelle->verifierSolution();
		int delta = coutTest - coutActuelle;

		// On teste le critère de Métropolis
		bool metropolis;
		if(delta <= 0) {
			metropolis = true;
		} else {
			double prob = exp(-delta/T);
			int seuilTirage = 100*prob;
			int numTire = rand()%100;
			metropolis = (numTire <= seuilTirage);
		}

		// Application des conséquences
		if(metropolis) {
			configActuelle = configTestee; // Déplacement entériné
			if(coutTest < coutMeilleure) { // Mise à jour de la meilleure configuration (pas forcément S' si on a tiré au sort en faveur de la dégradation)
				meilleureConfig = configTestee;
				coutMeilleure = coutTest;
			}
		}

		// Refroidissement (à élaborer)
		T*=coeff;
	}
	return(meilleureConfig);
}


int main()
{

}