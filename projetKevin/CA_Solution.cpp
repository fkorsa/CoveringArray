#include "CA_Solution.h"

CA_Solution::CA_Solution(int val, int col, int** resultat)
{
	v = val;
	k = col;
	solution = resultat;

	int tailleTotale = sizeof(solution);
	int tailleLigne = sizeof(solution[0]);
	N = tailleTotale/tailleLigne;
	nbIt = 0;

	// Génère toutes les contraintes élémentaires
	nbContraintes = (k*(k-1)*v*v)/2;
	erreurs = -1;

	// Allocation des ressources pour les contraintes
	contraintes = new bool***[k];
	for(int i1=0; i1<k; i1++) {
		contraintes[i1] = new bool**[k];
		for(int i2=0; i2<k; i2++) {
			contraintes[i1][i2] = new bool*[v];
			for(int i3=0; i3<v; i3++) {
				contraintes[i1][i2][i3] = new bool[v];
				for(int i4=0; i4<v; i4++) {
					contraintes[i1][i2][i3][i4] = false;
				}
			}
		}
	}
}


CA_Solution::CA_Solution(int val, int col, int lignes)
{
	v = val;
	k = col;
	N = lignes;

	solution = new int*[N];
	for(int li=0; li<N; li++) {
		solution[li] = new int[k];
	}
	nbIt = 0;

	// Génère toutes les contraintes élémentaires
	nbContraintes = (k*(k-1)*v*v)/2;
	erreurs = -1;

	// Allocation des ressources pour les contraintes
	contraintes = new bool***[k];
	for(int i1=0; i1<k; i1++) {
		contraintes[i1] = new bool**[k];
		for(int i2=0; i2<k; i2++) {
			contraintes[i1][i2] = new bool*[v];
			for(int i3=0; i3<v; i3++) {
				contraintes[i1][i2][i3] = new bool[v];
				for(int i4=0; i4<v; i4++) {
					contraintes[i1][i2][i3][i4] = false;
				}
			}
		}
	}
}


CA_Solution::CA_Solution(const CA_Solution& sol)
{
	// Copie membre a membre
	v = sol.v;
	k = sol.k;
	N = sol.N;
	nbIt = sol.nbIt;
	erreurs = sol.erreurs;
	nbContraintes = sol.nbContraintes;

	// Copie de la solution
	solution = new int*[N];
	for(int j1=0; j1 < N; j1++) {
		solution[j1] = new int[k];
		for(int j2=0; j2<k; j2++) {
			solution[j1][j2] = sol.solution[j1][j2];
		}
	}

	// Copie des contraintes
	contraintes = new bool***[k];
	for(int i1=0; i1<k; i1++) {
		contraintes[i1] = new bool**[k];
		for(int i2=0; i2<k; i2++) {
			contraintes[i1][i2] = new bool*[v];
			for(int i3=0; i3<v; i3++) {
				contraintes[i1][i2][i3] = new bool[v];
				for(int i4=0; i4<v; i4++) {
					contraintes[i1][i2][i3][i4] = sol.contraintes[i1][i2][i3][i4];
				}
			}
		}
	}
}


CA_Solution::~CA_Solution()
{
	// Liberation des ressources
	for(int i1=0; i1<k; i1++)
	{
		for(int i2=0; i2<k; i2++)
		{
			for(int i3=0; i3<v; i3++)
			{
				delete[] contraintes[i1][i2][i3];
			}
			delete[] contraintes[i1][i2];
		}
		delete[] contraintes[i1];
	}
	delete[] contraintes;
}


void CA_Solution::ecrireFichier(string chemin) // écrit dans un fichier à partir d'une solution
{
	ofstream fichier(chemin.c_str());
	erreurs = verifierSolution();

	if(fichier)
	{
		// Première ligne
		fichier << v << " " << k << " " << N << " " << erreurs << endl;

		// Ligne suivantes
		for(int s=0; s<N; s++) {
			for(int t=0; t<k; t++) {
				fichier << solution[s][t] << " ";
			}
			fichier << endl;
		}

		fichier.close();
	}
	else {
		cerr << "Impossible d'ouvrir le fichier !" << endl; }
}


int CA_Solution::verifierSolution()
{ // vérifie si une solution est valide sachant les v,k,N,sol et renvoie le nombre d'erreurs
	int contraintesSatisfaites = 0;
	for(int i1=0; i1<k; i1++)
	{
		for(int i2=0; i2<k; i2++)
		{
			for(int i3=0; i3<v; i3++)
			{
				for(int i4=0; i4<v; i4++)
				{
					contraintes[i1][i2][i3][i4] = false;
				}
			}
		}
	}

	// On observe chaque paire de caractères sur les lignes et les colonnes pour remplir les contraintes
	for(int l=0; l<N; l++)
	{
		for(int c1=0; c1<k-1; c1++)
		{
			int v1 = solution[l][c1];
			for(int c2=c1+1; c2<k; c2++)
			{
				int v2 = solution[l][c2];
				if (!contraintes[c1][c2][v1][v2])
				{
					contraintesSatisfaites++;
					contraintes[c1][c2][v1][v2] = true;
				}
			}
		}
	}

	erreurs = (nbContraintes - contraintesSatisfaites);
	return(erreurs);
}

void CA_Solution::ajouterLigne()
{
	solution[N] = new int[k];
	N++;
}

void CA_Solution::enleverLigne()
{
	delete[] solution[N-1];
}