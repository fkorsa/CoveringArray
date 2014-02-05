using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <windows.h>

class CA_Solution
{
public:
	CA_Solution(int val, int col, vector<int> resultat);

	void CA_Solution::ecrireFichier(string chemin);
	int CA_Solution::verifierSolution();
	void CA_Solution::mouvement();

	int v;
	int k;
	int N;
	vector<int> solution;
};