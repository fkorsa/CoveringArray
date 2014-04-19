#ifndef CA_SOLUTION_H
#define CA_SOLUTION_H

using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <cstdlib>

class CA_Solution
{
public:
	CA_Solution(int val, int col, int** resultat);
	CA_Solution(int val, int col, int lignes);
	CA_Solution(const CA_Solution& sol);
	~CA_Solution();

	void ecrireFichier(string chemin);
	int verifierSolution();
	void ajouterLigne();
	void enleverLigne();

	int v;
	int k;
	int N;
	int nbContraintes;

	int** solution;
	bool ****contraintes;
	int erreurs;
	int nbIt; 
};

#endif //CA_SOLUTION_H