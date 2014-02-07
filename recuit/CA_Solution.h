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

class CA_Solution
{
public:
	CA_Solution(int val, int col, vector<int> resultat);
    CA_Solution(const CA_Solution& sol);

    void ecrireFichier(string chemin);
    int verifierSolution();
    int verifierSolution(Mouvement mv);
    Mouvement mouvement();
    void appliquerMouvement(Mouvement mv);

	int v;
	int k;
	int N;
	vector<int> solution;
};

#endif //CA_SOLUTION_H
