#include "CA_Solution.h"


CA_Solution::CA_Solution(int val, int col, vector<int> resultat)
{
	v = val;
	k = col;
	solution = resultat;
	N = solution.size()/k;
    nbIt = 0;
    nbMvt = 0;
    // Génère toutes les contraintes élémentaires
    nbContraintes = (k*(k-1)*v*v)/2;
    erreurs = -1;
    erreursDernierMv = -1;

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

    // Allocation des ressources pour les contraintes temporaires
    copieContraintesAncien = new bool*[k];
    copieContraintesNouveau = new bool*[k];
    for(int i1=0; i1<k; i1++)
    {
        copieContraintesAncien[i1] = new bool[v];
        copieContraintesNouveau[i1] = new bool[v];
    }
    mvtCourant.mAncienSymbole = solution[0];
    if(solution[0] != 0)
    {
        mvtCourant.mSymbole = 0;
    }
    else
    {
        mvtCourant.mSymbole = 1;
    }

}

CA_Solution::CA_Solution(int val, int col, int lignes)
{
    v = val;
    k = col;
    N = lignes;
    solution = vector<int>(k*N);
    nbIt = 0;
    nbMvt = 0;
    // Génère toutes les contraintes élémentaires
    nbContraintes = (k*(k-1)*v*v)/2;
    erreurs = -1;
    erreursDernierMv = -1;

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

    // Allocation des ressources pour les contraintes temporaires
    copieContraintesAncien = new bool*[k];
    copieContraintesNouveau = new bool*[k];
    for(int i1=0; i1<k; i1++)
    {
        copieContraintesAncien[i1] = new bool[v];
        copieContraintesNouveau[i1] = new bool[v];
    }
    mvtCourant.mAncienSymbole = solution[0];
    if(solution[0] != 0)
    {
        mvtCourant.mSymbole = 0;
    }
    else
    {
        mvtCourant.mSymbole = 1;
    }
}

CA_Solution::CA_Solution(const CA_Solution& sol)
{
    // Copie membre a membre
    v = sol.v;
    k = sol.k;
    solution = sol.solution;
    N = sol.N;
    nbIt = sol.nbIt;
    nbMvt = sol.nbMvt;
    erreurs = sol.erreurs;
    erreursDernierMv = sol.erreursDernierMv;
    nbContraintes = sol.nbContraintes;

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
    copieContraintesAncien = new bool*[k];
    copieContraintesNouveau = new bool*[k];
    for(int i1=0; i1<k; i1++)
    {
        copieContraintesAncien[i1] = new bool[v];
        copieContraintesNouveau[i1] = new bool[v];
    }
}

CA_Solution::~CA_Solution()
{
    // Liberation des ressources
    for(int i1=0; i1<k; i1++)
    {
        delete[] copieContraintesAncien[i1];
        delete[] copieContraintesNouveau[i1];
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
    delete[] copieContraintesAncien;
    delete[] copieContraintesNouveau;
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

// Compte le nombre d'erreurs de la matrice resultant du mouvement mv
// Pour etre efficace, on ne recalcule pas toutes les contraintes. On regarde d'abord
// le symbole qui disparait, puis on enleve toutes les contraintes associees avec les autres colonnes de
// la ligne. Ensuite, on parcourt la colonne du mouvement de haut en bas, en regardant si le symbole dans la ligne courante
// est celui qu'on vient d'enlever : si c'est le cas, on retablit les contraintes associees dans la ligne.
// Enfin, on valide les contraintes induites par le rajout du nouveau symbole dans la case du mouvement.
int CA_Solution::verifierSolution(Mouvement mv)
{
    // Copie du nombre d'erreurs
    erreursDernierMv = erreurs;

    // Copie de la partie du tableau de contraintes concernee
    for(int i1=0; i1<k; i1++)
    {
        for(int i2=0; i2<v; i2++)
        {
            if(i1 < mv.mCol)
            {
                copieContraintesAncien[i1][i2] = contraintes[i1][mv.mCol][i2][mv.mAncienSymbole];
                copieContraintesNouveau[i1][i2] = contraintes[i1][mv.mCol][i2][mv.mSymbole];
            }
            if(i1 > mv.mCol)
            {
                copieContraintesAncien[i1][i2] = contraintes[mv.mCol][i1][mv.mAncienSymbole][i2];
                copieContraintesNouveau[i1][i2] = contraintes[mv.mCol][i1][mv.mSymbole][i2];
            }
        }
    }

    // Suppression des contraintes entre l'ancien symbole et les autres symboles de la ligne
    for(int i1=0; i1<k; i1++)
    {
        if(copieContraintesAncien[i1][solution[mv.mLigne*k+i1]] && i1 != mv.mCol)
        {
            copieContraintesAncien[i1][solution[mv.mLigne*k+i1]] = false;
            erreursDernierMv++;
        }
    }

    // Retablissement des contraintes enlevees de maniere superflue
    for(int l=0; l<N; l++)
    {
        if(l != mv.mLigne && solution[l*k+mv.mCol] == mv.mAncienSymbole)
        {
            for(int i1=0; i1<k; i1++)
            {
                if(!copieContraintesAncien[i1][solution[l*k+i1]] && i1 != mv.mCol)
                {
                    copieContraintesAncien[i1][solution[l*k+i1]] = true;
                    erreursDernierMv--;
                }
            }
        }
    }

    // Validation des contraintes remplies par l'ajout du nouveau symbole
    for(int i1=0; i1<k; i1++)
    {
        if(!copieContraintesNouveau[i1][solution[mv.mLigne*k+i1]] && i1 != mv.mCol)
        {
            copieContraintesNouveau[i1][solution[mv.mLigne*k+i1]] = true;
            erreursDernierMv--;
        }
    }

    return erreursDernierMv;
}

// effectue le mouvement suivant dans la configuration courante
Mouvement CA_Solution::mouvementSuivant()
{
    Mouvement mv = mvtCourant;
    bool estFinal = false;
    // Mouvement suivant dans le parcours de tous les mouvements possibles
    mvtCourant.incrementer(k, v, N);
    estFinal |= mvtCourant.estFinal;
    while(mvtCourant.mSymbole == solution[k*mvtCourant.mLigne+mvtCourant.mCol])
    {
        mvtCourant.incrementer(k, v, N);
        estFinal |= mvtCourant.estFinal;
    }
    mvtCourant.mAncienSymbole = solution[k*mvtCourant.mLigne+mvtCourant.mCol];
    mvtCourant.estFinal = estFinal;
    return mv;
}

void CA_Solution::reinitialiserMouvementCourant()
{
    mvtCourant.estFinal = false;
    mvtCourant.mLigne = 0;
    mvtCourant.mCol = 0;
    mvtCourant.mSymbole = 0;
    mvtCourant.mAncienSymbole = solution[0];
    while(mvtCourant.mSymbole == solution[k*mvtCourant.mLigne+mvtCourant.mCol])
    {
        mvtCourant.incrementer(k, v, N);
    }
}

// Effectue le mouvement mv : remplace le symbole dans la matrice et recalcule les erreurs et les contraintes.
// Nous appelons verifierSolution(mv) dans le corps de cette fonction : nous pouvons donc nous contenter
// de copier les valeurs des contraintes generees par verifierSolution.
void CA_Solution::appliquerMouvement(Mouvement mv)
{
    solution[k*mv.mLigne+mv.mCol] = mv.mSymbole;

    verifierSolution(mv);
    for(int i1=0; i1<k; i1++)
    {
        for(int i2=0; i2<v; i2++)
        {
            if(i1 < mv.mCol)
            {
                contraintes[i1][mv.mCol][i2][mv.mAncienSymbole] = copieContraintesAncien[i1][i2];
                contraintes[i1][mv.mCol][i2][mv.mSymbole] = copieContraintesNouveau[i1][i2];
            }
            if(i1 > mv.mCol)
            {
                contraintes[mv.mCol][i1][mv.mAncienSymbole][i2] = copieContraintesAncien[i1][i2];
                contraintes[mv.mCol][i1][mv.mSymbole][i2] = copieContraintesNouveau[i1][i2];
            }
        }
    }
    erreurs = erreursDernierMv;
}

// Supprime la derniere ligne de la matrice
void CA_Solution::enleverLigne()
{
    solution.resize(k*(--N));
}
