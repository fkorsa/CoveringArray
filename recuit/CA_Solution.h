using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

class CA_Solution
{
public:
	CA_Solution(int val, int col, vector<int> resultat);

    void ecrireFichier(string chemin);
    int verifierSolution();
    void mouvement();

	int v;
	int k;
	int N;
	vector<int> solution;
};
