#include "CA_Solution.h"


CA_Solution::CA_Solution(int val, int col, vector<int> resultat)
{
	v = val;
	k = col;
	solution = resultat;
	N = solution.size()/k;
}


void CA_Solution::ecrireFichier(string chemin) { // écrit dans un fichier à partir d'une solution

    ofstream fichier(chemin.c_str());

	int erreurs = this->verifierSolution();

	if(fichier)
	{
		// Première ligne
		fichier << this->v << " " << this->k << " " << this->N << " " << erreurs << endl;

		// Ligne suivantes
		for(int s=0; s<this->N; s++) {
			for(int t=0; t<this->k; t++) {
				fichier << this->solution[s*this->k+t] << " ";
			}
			fichier << endl;
		}

		fichier.close();
	}
	else {
		cerr << "Impossible d'ouvrir le fichier !" << endl; }
}

int CA_Solution::verifierSolution() { // vérifie si une solution est valide sachant les v,k,N,sol et renvoie le nombre d'erreurs

	// Génère toutes les contraintes élémentaires
	int nbContraintes((k*(k-1)*v*v)/2);

	bool**** contraintes; // contraintes[k][k][v][v]

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

	int contraintesSatisfaites = 0;
	// On observe chaque paire de caractères sur les lignes et les colonnes pour remplir les contraintes
	for(int l=0; l<N; l++) {
		for(int c1=0; c1<k-1; c1++) {
			int v1 = solution[l*k+c1];
			for(int c2=c1+1; c2<k; c2++) {
				int v2 = solution[l*k+c2];
				if (!contraintes[c1][c2][v1][v2]) {
					contraintesSatisfaites++;
					contraintes[c1][c2][v1][v2] = true;
				}
			}
		}
	}

	int erreurs = (nbContraintes - contraintesSatisfaites);
	if(erreurs == 0) {
		cout << "Solution valable." << endl;} else {
			cout << "Solution erronée. Il reste " << erreurs << " contraintes non satisfaites." << endl;
		}

		return(erreurs);
} // Fin fonction vérification


void CA_Solution::mouvement() { // effectue un mouvement aléatoire dans une configuration donnée
	// Tirage au sort d'une colonne et d'une ligne
	int c = rand()%k;
	int l = rand()%N;

	// Tirage au sort d'un nouveau symbole
	int symboleActuel = solution[k*l+c];
	int nouveauSymbole = symboleActuel;
	while(nouveauSymbole == symboleActuel) {
		nouveauSymbole = rand()%v;
	}
	
	solution[k*l+c] = nouveauSymbole;
}
