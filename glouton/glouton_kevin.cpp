#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <ctime>

#include <windows.h>
using namespace std;

#include "CA_Solution.h"

vector<int> couvertureGlouton(int v, int k, float tolerance) // Glouton si tolerance = 0, compl�tement al�atoire si tolerance = 1
{
	// Nombre de contraintes � satisfaire
	int nbContraintes((k*(k-1)*v*v)/2);

	bool**** contraintes; // contraintes[k][k][v][v]
	// G�n�re toutes les contraintes �l�mentaires
	contraintes = (bool****) malloc(k*sizeof(bool***));
	for(int i1=0; i1<k; i1++) {
		contraintes[i1] = (bool***) malloc(k*sizeof(bool**));
		for(int i2=0; i2<k; i2++) {
			contraintes[i1][i2] = (bool**) malloc(v*sizeof(bool*));
			for(int i3=0; i3<v; i3++) {
				contraintes[i1][i2][i3] = (bool*) malloc(v*sizeof(bool));
				for(int i4=0; i4<v; i4++) {
					contraintes[i1][i2][i3][i4] = false;
				}
			}
		}
	}

	// Algorithme glouton (avec part d'al�atoire si tolerance != 0)
	vector<int> resultat(0);
	int indexLignes = -1; // Nombre de lignes du tableau (indice de la ligne sur laquelle on est en train d'�crire)
	int contraintesSatisfaites = 0;

	while (contraintesSatisfaites < nbContraintes) {
		resultat.insert(resultat.end(),k,-1); // Ajout d'une ligne au tableau de r�sultat
		indexLignes++;

		// Remplissage de la ligne
		for(int colonne = 0; colonne<k ; colonne++) {

			if (colonne == 0) { // Remplissage du d�but de la ligne

				int symbMax = -1; // Symbole qui maximise le crit�re glouton
				int compMax = -1; // Compteur de contraintes qu'il peut satisfaire associ�
				int* scores = new int[v]; // Score de r�solution de contraintes pour chaque symbole possible

				for(int symb=0; symb<v; symb++) { // Test de chaque symbole possible
					int symbComp = 0; // Compteur pour le symbole test�

					for(int k2=1; k2<k; k2++) {
						for(int v2=0; v2<v; v2++) {
							if (!contraintes[0][k2][symb][v2]) {
								symbComp++;
							}
						}
					} // Fin boucle k2

					scores[symb] = symbComp;

					if (symbComp > compMax) {
						symbMax = symb;
						compMax = symbComp;
					}

				} // Fin test symbole

				// On conna�t le nombre de contraintes que peut r�soudre le meilleur symbole glouton, on introduit alors un seuil de contraintes inf�rieur en fonction du param�tre de tol�rance.
				// Les symboles r�solvant au moins ce nombre de contraintes seront des candidats au tirage au sort pour remplir la colonne.

				int seuil = floor((1-tolerance)*compMax);
				int nbCandidats = 0;
				vector<int> candidats(0);

				for(int j=0; j<v; j++) { // On regarde si chaque symbole peut �tre candidat
					if(scores[j] >= seuil) {
						candidats.insert(candidats.end(),1,j); // Ajout d'un candidat
						nbCandidats++;
					}
				}

				// Tirage au sort du candidat
				int candidatElu = rand()%nbCandidats;
				int symbElu;

				if (tolerance==0) {
					symbElu = symbMax; // On ne prend pas en compte le tirage au sort
				} else {
					symbElu = candidats[candidatElu]; 
				}
				resultat[indexLignes*k+colonne] = symbElu; // Insertion du symbole �lu

			} else { // Remplissage des autres colonnes de la ligne

				int premierPassage = 1; // A voir comme un bool�en, on n'a pas encore effectu� de second passage (voir suite)
				int symbMax = -1; // Symbole qui maximise le crit�re glouton
				int compMax = -1; // Compteur de contraintes qu'il peut satisfaire associ�
				int* scores = new int[v]; // Score de r�solution de contraintes pour chaque symbole possible

				for(int symb=0; symb<v; symb++) { // Test de chaque symbole possible
					int symbComp = 0; // Compteur pour le symbole test� en cours

					// Test avec les �l�ments des colonnes pr�c�dentes
					for(int k1=0; k1<colonne; k1++) {
						int v1 = resultat[indexLignes*k+k1];
						if (!contraintes[k1][colonne][v1][symb]) {
							symbComp++;
						}
					} // Fin boucle k1

					scores[symb] = symbComp;

					if (symbComp > compMax) {
						symbMax = symb;
						compMax = symbComp;
					}

				} // Fin test symbole

				if (compMax == 0) {
					// Si jamais toutes les contraintes par rapport aux colonnes pr�c�dentes sont d�j� satisfaites, il faut proc�der comme lors du choix pour la premi�re colonne et
					// choisir le symbole � rentrer en fonction des contraintes l'impliquant avec les colonnes suivantes. Il faut donc recommencer le choix dans cette optique.

					premierPassage = 0; // On a effectu� un second passage

					symbMax = -1; // Symbole qui maximise le crit�re glouton
					compMax = -1; // Compteur de contraintes qu'il peut satisfaire associ�
					scores = new int[v]; // Score de r�solution de contraintes pour chaque symbole possible

					for(int symb=0; symb<v; symb++) { // Test de chaque symbole
						int symbComp = 0; // Compteur pour le symbole en cours

						// Test avec les �l�ments des colonnes suivantes si jamais toutes les contraintes avec les colonnes pr�c�dentes sont r�gl�es
						for(int k2=colonne+1; k2<k; k2++) {
							for(int v2=0; v2<v; v2++) {
								if (!contraintes[colonne][k2][symb][v2]) {
									symbComp++;
								}
							}
						} // Fin boucle k2

						scores[symb] = symbComp;

						if (symbComp > compMax) {
							symbMax = symb;
							compMax = symbComp;
						}
					} // Fin test symbole
				} // Fin du deuxi�me passage

				// On conna�t le nombre de contraintes que peut r�soudre le meilleur symbole glouton, on introduit alors un seuil de contraintes inf�rieur en fonction du param�tre de tol�rance.
				// Les symboles r�solvant au moins ce nombre de contraintes seront des candidats au tirage au sort pour remplir la colonne.
				int seuil = floor((1-tolerance)*compMax);
				int nbCandidats = 0;
				vector<int> candidats(0);

				for(int j=0; j<v; j++) { // On regarde si chaque symbole peut �tre candidat
					if(scores[j] >= seuil) {
						candidats.insert(candidats.end(),1,j); // Ajout d'un candidat
						nbCandidats++;
					}
				}

				// Tirage au sort du candidat
				int candidatElu = rand()%nbCandidats;
				int symbElu;

				if (tolerance==0) {
					symbElu = symbMax; // On ne prend pas en compte le tirage au sort
				} else {
					symbElu = candidats[candidatElu]; 
				}

				resultat[indexLignes*k+colonne] = symbElu; // Insertion du symbole �lu

				for(int k1=0; k1<colonne; k1++) { // R�solution des contraintes avec �l�ments de toutes les autres colonnes (inf�rieures � colonne)
					int symbAss = resultat[indexLignes*k+k1];
					contraintes[k1][colonne][symbAss][symbElu] = true;
				}

				// Mise � jour du nombre de contraintes satisfaites si on a pas effectu� de second passage (qui ne r�soud en fait aucune contrainte)
				contraintesSatisfaites+=scores[symbElu]*premierPassage;

			} // Fin IF colonne ELSE
		} // Fin remplissage d'une colonne

		// cout << "Ligne " << indexLignes << " ecrite, " << contraintesSatisfaites << " contraintes satisfaites au total, reste " << nbContraintes-contraintesSatisfaites << endl;
	} // Fin WHILE
	return resultat;
} // Fin fonction couvertureGlouton


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


int main()
{
	/* Exemple de calcul, lecture, �criture de solution */

	vector<int> resultatExemple = couvertureGlouton(2,4,0);
	CA_Solution* sol_exemple = new CA_Solution(2, 4, resultatExemple);
	sol_exemple->ecrireFichier("../Resultats/sol_exemple.txt");

	CA_Solution* test_lecture = lireFichierSolution("../Resultats/sol_exemple.txt");
	test_lecture->ecrireFichier("../Resultats/test_lecture.txt");
	int erreurs_lecture = test_lecture->verifierSolution();

	/* Exp�rimentations */
	srand(time(NULL)); // initialisation de rand

	// Donn�es utilis�es pour la tourn�e de tests
	int v = 2;
	int k = 4;
	float tol = 0./3.;

	float facteur = 8.6/12; // facteur multiplicatif pour calculer le temps CPU sur la machine de r�f�rence
	int nbTests = 100;

	// Structures de r�cup�ration des donn�es
	int* couts = new int[nbTests];
	int* exec = new int[nbTests];

	for(int test=0; test < nbTests ; test++) { // Boucle effectuant les tests successifs

		int debut = clock();
		vector<int> resultatExp = couvertureGlouton(v,k,tol);
		int fin = clock();
		int temps = (facteur*(fin - debut))/ CLOCKS_PER_SEC * 1000;

		CA_Solution* sol_exp = new CA_Solution(v, k, resultatExp);
		int N_exp = sol_exp->N;

		couts[test] = N_exp;
		exec[test] = temps;
	}

	// Calcul des statistiques
	double moyenneCouts = 0;
	double moyenneExec = 0;
	double* ecarts = new double[nbTests];
	double ecartType = 0;

	// Calcul des moyennes
	for (int i=0 ; i<nbTests ; i++) {
		moyenneCouts += couts[i];
		moyenneExec += exec[i];

	}
	moyenneCouts /= nbTests;
	moyenneExec /= nbTests;

	// Calcul des extrema pour les co�ts
	int min = couts[0];
	int max = couts[0];
	for (int i=0 ; i<nbTests ; i++) {
		if(couts[i] < min) {
			min = couts[i];
		}
		if(couts[i] > max) {
			max = couts[i];
		}
	}

	// Calcul de l'�cart type pour les co�ts
	for (int i=0 ; i<nbTests ; i++)
	{
		ecarts[i]=pow(couts[i]-moyenneCouts, 2);
		ecartType += ecarts[i];
	}
	ecartType = sqrt(ecartType / nbTests);

	// Enregistrement dans un fichier de tous les co�ts pour pouvoir les porter sur un graphique par la suite
	ofstream fichier("../Resultats/couts.txt");
	if(fichier)
	{
		for (int i=0 ; i<nbTests ; i++) {
			fichier << couts[i] << endl;
		}
		fichier.close();
	}
	else {
		cerr << "Impossible d'ouvrir le fichier !" << endl; }
	// Fin de l'enregistrement

	Sleep(20000);
}