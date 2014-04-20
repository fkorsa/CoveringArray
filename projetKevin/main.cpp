#include <iostream>
#include <stdio.h>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <chrono>
#include <list>

using namespace std;
#include "CA_Solution.h"
#include "Population.h"

// Lire une matrice a partir d'un fichier
CA_Solution* lireFichierSolution(const char* chemin)
{
	ifstream infile(chemin);
	int v = 0;
	int k = 0;
	int N = 0;
	int erreurs = 0;

	infile >> v >> k >> N >> erreurs;

	int** solution = new int*[N];
	for(int i=0; i<N; i++)
	{
		solution[i] = new int[k];
	}

	int index = 0;
	int val = 0;

	while (infile >> val)
	{
		int ligne = index/N;
		int col = index % N;
		solution[ligne][col] = val;
		index++;
	}
	CA_Solution* ca_sol = new CA_Solution(v,k,solution);
	return ca_sol;
}

enum TYPE_CROISEMENT
{
	CROISEMENT_GLOUTON,
	CROISEMENT_RAND,
	CROISEMENT_ECHANGE,
	CROISEMENT_AUCUN
};

enum TYPE_DEUX_ETAPE
{
	DEUX_ETAPE_DESCENTE,
	DEUX_ETAPE_MUTATION
};


// Application de l'algorithme génétique
CA_Solution* evolution(int v, int k, int N, int tailleParents, int tailleEnfants, ofstream *fichier, float pourcentMutation, int essaisDesc, TYPE_CROISEMENT type1, TYPE_DEUX_ETAPE type2, int maxTemps)
{
	// Variables pour l'algorithme de base
	ofstream& fichierLocal = *fichier;
	int iteration = 1;
	int i;
	Population* pop = new Population(tailleParents,tailleEnfants);
	CA_Solution* meilleureConfig = nullptr; // Meilleures des configurations testées jusqu'alors
	bool croisement;

	// Initialisation de la population
	pop->remplirGeneration(v, k, N);
	// Calcul et tri de leurs couts
	pop->calculerCouts(tailleParents);

	// Variables pour la prise en compte du temps d'execution
	chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now();   
	chrono::duration<double> duree;
	double dureeMillisecondes;
	const double tempsMax = maxTemps*12.4/8.6;

	// Initialisation de la meilleure config
	int coutMeilleure = pop->couts[0];
	meilleureConfig = new CA_Solution(*(pop->generation[0]));

	duree = chrono::system_clock::now()-dateDebut;
	dureeMillisecondes = 1000*duree.count();

	// Critere d'arret : une minute de temps d'execution, ou bien une solution trouvee
	while((coutMeilleure > 0 || fichier) && dureeMillisecondes < tempsMax)
	{
		// Croisement
		if(type1 == CROISEMENT_GLOUTON) {
			pop->croisementGlouton();
			croisement = true;
		}
		else if(type1 == CROISEMENT_RAND) {
			pop->croisementRand();
			croisement = true;
		}
		else if(type1 == CROISEMENT_ECHANGE) {
			pop->croisementEchange();
			croisement = true;
		}
		else {
			pop->taille = tailleParents;
			pop->tailleEnfants = tailleParents;
			tailleEnfants = 0;
			croisement = false;
		}

		// Mutation
		if(type2 == DEUX_ETAPE_DESCENTE) {
			pop->descente(essaisDesc, croisement);
		}
		else if(type2 == DEUX_ETAPE_MUTATION) {
			pop->mutation(pourcentMutation, croisement);
		} else {
			// On ne fait rien, mais ce cas n'est pas prévu
		}

		// Enregistrement de la progression du cout dans un fichier
		if(fichier && iteration%50 == 0)
		{
			fichierLocal << iteration << " " << coutMeilleure << endl;
			cout << iteration << " " << coutMeilleure << endl;
		}

		// Calcul et des couts, tri dans l'ordre croissant du tableau des couts et tri de la population dans le meme ordre
		pop->calculerCouts(pop->taille);

		// Selection
		for(i = tailleParents; i < tailleParents+tailleEnfants; i++)
		{
			pop->enleverIndividu(i);
		}

		// Mise à jour de la meilleure configuration
		if(pop->couts[0] < coutMeilleure)
		{
			delete meilleureConfig;
			meilleureConfig = new CA_Solution(*(pop->generation[0]));
			coutMeilleure = pop->couts[0];
			// cout << "Meilleure solution trouvee a l'iteration : " << iteration << " de cout : " << coutMeilleure << endl;
		}

		iteration++;

		// Actualisation du temps passe
		duree = chrono::system_clock::now()-dateDebut;
		dureeMillisecondes = 1000*duree.count();
	}
	// Enregistrement pour les statistiques
	meilleureConfig->nbIt = iteration;

	return(meilleureConfig);
}

// Génère des résultats selon le croisement et la seconde étape sélectionnés
void genererResultats(int nbExec, TYPE_CROISEMENT type1, TYPE_DEUX_ETAPE type2, int nbParents, int nbEnfants, float pourcMut, int essaisDesc, string output)
{
	ifstream infile("inputData");
	ofstream outfile(output);
	int nbSymboles, nbColonnes, nbLignes, maxTps;
	double coutMoyen, itMoyen, tempsMoyen;
	double coutMin, itMin, tempsMin;
	double coutMax, itMax, tempsMax, temps;
	chrono::time_point<chrono::system_clock> start;
	chrono::duration<double> realTime;
	CA_Solution* solution;

	if(outfile.is_open())
	{
		outfile << "NombreSymboles NombreColonnes NombreLignes CoutMin CoutMoyen CoutMax ItMin ItMoyen ItMax TempsMin TempsMoyen TempsMax" << endl;
		// Pour chaque configuration dans le fichier "inputData"
		while (infile >> nbSymboles >> nbColonnes >> nbLignes >> maxTps)
		{
			coutMoyen = 0;
			itMoyen = 0;
			tempsMoyen = 0;
			coutMin = 0;
			itMin = 0;
			tempsMin = 0;
			coutMax = 0;
			itMax = 0;
			tempsMax = 0;
			cout << nbSymboles << " "
				<< nbColonnes << " "
				<< nbLignes << " " << endl;
			// On execute l'algorithme plusieurs fois.
			int nbTentatives = 0; // nombre de tentatives pour un même N
			for(int i = 0; i < nbExec; i++)
			{
				start = chrono::system_clock::now();
				solution = evolution(nbSymboles, nbColonnes, nbLignes, nbParents, nbEnfants, nullptr, pourcMut, essaisDesc, type1, type2, maxTps);
				nbTentatives++;
				realTime = chrono::system_clock::now()-start;
				temps = 1000*realTime.count();
				if(nbTentatives == 1)
				{
					coutMin = solution->erreurs;
					itMin = solution->nbIt;
					tempsMin = temps;
					coutMoyen = 0;
					itMoyen = 0;
					tempsMoyen = 0;
					coutMax = 0;
					itMax = 0;
					tempsMax = 0;
				}
				if(coutMin > solution->erreurs)
				{
					coutMin = solution->erreurs;
				}
				if(itMin > solution->nbIt)
				{
					itMin = solution->nbIt;
				}
				if(tempsMin > temps)
				{
					tempsMin = temps;
				}
				if(coutMax < solution->erreurs)
				{
					coutMax = solution->erreurs;
				}
				if(itMax < solution->nbIt)
				{
					itMax = solution->nbIt;
				}
				if(tempsMax < temps)
				{
					tempsMax = temps;
				}
				coutMoyen += solution->erreurs;

				itMoyen += solution->nbIt;
				tempsMoyen += temps;

				// Affichage résultat et abaissement du nombre de lignes le cas échéant
				cout << "Tentative " << i << ", nombre de lignes " << nbLignes << ", cout " << solution->erreurs << endl;
				if(solution->erreurs == 0) {
					nbLignes--;
					nbTentatives = 0;
				}
				delete solution;
			}
			if (nbTentatives == 0) {nbTentatives = 1;}
			coutMoyen /= nbTentatives;
			itMoyen /= nbTentatives;
			tempsMoyen /= nbTentatives;
			tempsMoyen *= (8.6/12.4);
			tempsMin *= (8.6/12.4);
			tempsMax *= (8.6/12.4);

			// On enregistre les donnees dans le fichier output
			outfile << nbSymboles << " "
				<< nbColonnes << " "
				<< nbLignes << " "
				<< coutMin << " "
				<< coutMoyen << " "
				<< coutMax << " "
				<< itMin << " "
				<< itMoyen << " "
				<< itMax << " "
				<< tempsMin << " "
				<< tempsMoyen << " "
				<< tempsMax << endl;
		}
		outfile.close();
	}
}


void generateur()
{
}


int main()
{
	int seed = time(NULL);
	srand(seed);

	cout << "Croisement aleatoire puis descente" << endl;
	genererResultats(10, CROISEMENT_RAND, DEUX_ETAPE_DESCENTE, 10, 10, 0.0001, 100, "output_RD");
	cout << "Pas de croisement puis descente" << endl;
	genererResultats(10, CROISEMENT_AUCUN, DEUX_ETAPE_DESCENTE, 10, 10, 0.0001, 100, "output_AD");
	cout << "Croisement glouton puis mutation" << endl;
	genererResultats(10, CROISEMENT_GLOUTON, DEUX_ETAPE_MUTATION, 30, 30, 0.0001, 100, "output_GM");
	cout << "Croisement aleatoire puis mutation" << endl;
	genererResultats(10, CROISEMENT_RAND, DEUX_ETAPE_MUTATION, 30, 30, 0.0001, 100, "output_RM");
	cout << "Croisement avec echanges puis mutation" << endl;
	genererResultats(10, CROISEMENT_ECHANGE, DEUX_ETAPE_MUTATION, 30, 30, 0.0001, 100, "output_EM");
}