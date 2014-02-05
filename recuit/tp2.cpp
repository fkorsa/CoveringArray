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
	int nombreEssais = 100; // Condition d'arr�t simpliste, � modifier par la suite
	int coeff = 0.5; // A changer �galement par la suite lorsque le sch�ma de refroidissement sera clairement �tabli

	int T = tempInit;

	CA_Solution* configTestee = configInit; // Configuration S' suite � un mouvement
	CA_Solution* configActuelle = configTestee; // Configuration S dans laquelle on se trouve
	CA_Solution* meilleureConfig = configActuelle; // Meilleures des configurations test�es jusqu'alors

	int coutMeilleure = meilleureConfig->verifierSolution();

	for(int ess=0; ess<nombreEssais; ess++) {
		// On g�n�re un voisin al�atoire de S
		configTestee->mouvement();
		int coutTest = configTestee->verifierSolution();
		int coutActuelle = configActuelle->verifierSolution();
		int delta = coutTest - coutActuelle;

		// On teste le crit�re de M�tropolis
		bool metropolis;
		if(delta <= 0) {
			metropolis = true;
		} else {
			double prob = exp(-delta/T);
			int seuilTirage = 100*prob;
			int numTire = rand()%100;
			metropolis = (numTire <= seuilTirage);
		}

		// Application des cons�quences
		if(metropolis) {
			configActuelle = configTestee; // D�placement ent�rin�
			if(coutTest < coutMeilleure) { // Mise � jour de la meilleure configuration (pas forc�ment S' si on a tir� au sort en faveur de la d�gradation)
				meilleureConfig = configTestee;
				coutMeilleure = coutTest;
			}
		}

		// Refroidissement (� �laborer)
		T*=coeff;
	}
	return(meilleureConfig);
}


int main()
{

}