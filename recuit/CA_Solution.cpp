#include "CA_Solution.h"


CA_Solution::CA_Solution(int val, int col, vector<int> resultat)
{
	v = val;
	k = col;
	solution = resultat;
	N = solution.size()/k;
    nbIt = 0;
    // Génère toutes les contraintes élémentaires
    nbContraintes = (k*(k-1)*v*v)/2;

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

/// TODO : a verifier
CA_Solution::~CA_Solution()
{
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

void CA_Solution::ecrireFichier(string chemin) { // écrit dans un fichier à partir d'une solution

    ofstream fichier(chemin.c_str());

    verifierSolution();

	if(fichier)
	{
		// Première ligne
        fichier << v << " " << k << " " << N << " " << erreurs << endl;

		// Ligne suivantes
        for(int s=0; s<N; s++) {
            for(int t=0; t<k; t++) {
                fichier << solution[s*k+t] << " ";
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
			int v1 = solution[l*k+c1];
            for(int c2=c1+1; c2<k; c2++)
            {
				int v2 = solution[l*k+c2];
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
} // Fin fonction vérification

int CA_Solution::verifierSolution(Mouvement mv)
{
    for(int i1=mv.mCol+1; i1<k; i1++)
    {
        for(int i2=0; i2<v; i2++)
        {
            if(contraintes[mv.mCol][i1][mv.mAncienSymbole][i2])
            {
                contraintes[mv.mCol][i1][mv.mAncienSymbole][i2] = false;
                erreurs++;
            }
        }
    }
    for(int i1=0; i1<mv.mCol; i1++)
    {
        for(int i2=0; i2<v; i2++)
        {
            if(contraintes[i1][mv.mCol][i2][mv.mAncienSymbole])
            {
                contraintes[i1][mv.mCol][i2][mv.mAncienSymbole] = false;
                erreurs++;
            }
        }
    }

    for(int l=0; l<N; l++)
    {
        if(solution[l*k+mv.mCol] == mv.mAncienSymbole)
        {
            for(int i1=0; i1<mv.mCol; i1++)
            {
                for(int i2=0; i2<v; i2++)
                {
                    if(contraintes[i1][mv.mCol][i2][mv.mAncienSymbole])
                    {
                        contraintes[i1][mv.mCol][i2][mv.mAncienSymbole] = false;
                        erreurs++;
                    }
                }
            }
            for(int i1=mv.mCol+1; i1<k; i1++)
            {
                for(int i2=0; i2<v; i2++)
                {
                    if(contraintes[mv.mCol][i1][mv.mAncienSymbole][i2])
                    {
                        contraintes[mv.mCol][i1][mv.mAncienSymbole][i2] = false;
                        erreurs++;
                    }
                }
            }
        }
    }

    return erreurs;
}

Mouvement CA_Solution::mouvement() { // effectue un mouvement aléatoire dans une configuration donnée
	// Tirage au sort d'une colonne et d'une ligne
    Mouvement mv;
    mv.mCol = rand()%k;
    mv.mLigne = rand()%N;

	// Tirage au sort d'un nouveau symbole
    int symboleActuel = solution[k*mv.mLigne+mv.mCol];
    mv.mAncienSymbole = symboleActuel;
	int nouveauSymbole = symboleActuel;
	while(nouveauSymbole == symboleActuel) {
		nouveauSymbole = rand()%v;
	}
	
    mv.mSymbole = nouveauSymbole;

    return mv;
}

void CA_Solution::appliquerMouvement(Mouvement mv)
{
    solution[k*mv.mLigne+mv.mCol] = mv.mSymbole;
}

void CA_Solution::enleverLigne()
{
    solution.resize(k*(--N));
}
