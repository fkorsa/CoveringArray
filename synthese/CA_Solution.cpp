#include "CA_Solution.h"

void CA_Solution::allouerMemoire()
{
    nbIt = 0;
    nbMvt = 0;
    // Génère toutes les contraintes élémentaires
    nbContraintes = (k*(k-1)*v*v)/2;
    erreurs = -1;
    erreursDernierMv = -1;

    mvtCourant.mAncienSymbole = solution[0];
    if(solution[0] != 0)
    {
        mvtCourant.mSymbole = 0;
    }
    else
    {
        mvtCourant.mSymbole = 1;
    }

    // Allocation des ressources pour les contraintes
    occurencesCouples = new int***[k];
    for(int i1=0; i1<k; i1++) {
        occurencesCouples[i1] = new int**[k];
        for(int i2=0; i2<k; i2++) {
            occurencesCouples[i1][i2] = new int*[v];
            for(int i3=0; i3<v; i3++) {
                occurencesCouples[i1][i2][i3] = new int[v];
                for(int i4=0; i4<v; i4++) {
                    occurencesCouples[i1][i2][i3][i4] = 0;
                }
            }
        }
    }

    sousContraintesSymboles = new int*[k];
    for(int i1=0; i1<k; i1++)
    {
        sousContraintesSymboles[i1] = new int[v];
        for(int i2=0; i2<v; i2++)
        {
            sousContraintesSymboles[i1][i2] = 0;
        }
    }

    sousContraintesColonnes = new int[k];
    for(int i1=0; i1<k; i1++)
    {
        sousContraintesColonnes[i1] = 0;
    }

    deltas = new int**[k];
    for(int i1=0; i1<k; i1++)
    {
        deltas[i1] = new int*[N];
        for(int i2=0; i2<N; i2++)
        {
            deltas[i1][i2] = new int[v];
            for(int i3=0; i3<v; i3++)
            {
                deltas[i1][i2][i3] = 0;
            }
        }
    }
}

CA_Solution::CA_Solution(int val, int col, vector<int> resultat)
{
	v = val;
	k = col;
	solution = resultat;
	N = solution.size()/k;

    allouerMemoire();
}

CA_Solution::CA_Solution(int val, int col, int lignes)
{
    v = val;
    k = col;
    N = lignes;
    solution = vector<int>(k*N);

    allouerMemoire();
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
    occurencesCouples = new int***[k];
    for(int i1=0; i1<k; i1++) {
        occurencesCouples[i1] = new int**[k];
        for(int i2=0; i2<k; i2++) {
            occurencesCouples[i1][i2] = new int*[v];
            for(int i3=0; i3<v; i3++) {
                occurencesCouples[i1][i2][i3] = new int[v];
                for(int i4=0; i4<v; i4++) {
                    occurencesCouples[i1][i2][i3][i4] = sol.occurencesCouples[i1][i2][i3][i4];
                }
            }
        }
    }

    sousContraintesSymboles = new int*[k];
    for(int i1=0; i1<k; i1++)
    {
        sousContraintesSymboles[i1] = new int[v];
        for(int i2=0; i2<v; i2++)
        {
            sousContraintesSymboles[i1][i2] = sol.sousContraintesSymboles[i1][i2];
        }
    }

    sousContraintesColonnes = new int[k];
    for(int i1=0; i1<k; i1++)
    {
        sousContraintesColonnes[i1] = sol.sousContraintesColonnes[i1];
    }

    deltas = new int**[k];
    for(int i1=0; i1<k; i1++)
    {
        deltas[i1] = new int*[N];
        for(int i2=0; i2<N; i2++)
        {
            deltas[i1][i2] = new int[v];
            for(int i3=0; i3<v; i3++)
            {
                deltas[i1][i2][i3] = sol.deltas[i1][i2][i3];
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
                delete[] occurencesCouples[i1][i2][i3];
            }
            delete[] occurencesCouples[i1][i2];
        }
        delete[] occurencesCouples[i1];
    }
    delete[] occurencesCouples;

    for(int i1=0; i1<k; i1++)
    {
        delete[] sousContraintesSymboles[i1];
    }
    delete[] sousContraintesSymboles;
    delete[] sousContraintesColonnes;

    for(int i1=0; i1<k; i1++)
    {
        for(int i2=0; i2<N; i2++)
        {
            delete[] deltas[i1][i2];
        }
        delete[] deltas[i1];
    }
    delete[] deltas;
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
    for(int i1=0; i1<k - 1; i1++)
    {
        for(int i2=i1+1; i2<k; i2++)
        {
            for(int i3=0; i3<v; i3++)
            {
                for(int i4=0; i4<v; i4++)
                {
                    occurencesCouples[i1][i2][i3][i4] = 0;
                }
            }
        }
        for(int i4=0; i4<v; i4++)
        {
            sousContraintesSymboles[i1][i4] = (k-1)*v;
        }
        sousContraintesColonnes[i1] = (k-1)*v*v;
    }
    for(int i1=0; i1<k; i1++)
    {
        for(int i2=0; i2<N; i2++)
        {
            for(int i3=0; i3<v; i3++)
            {
                deltas[i1][i2][i3] = 0;
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
                if (!occurencesCouples[c1][c2][v1][v2])
                {
					contraintesSatisfaites++;
                    sousContraintesSymboles[c1][v1]--;
                    sousContraintesSymboles[c2][v2]--;
                    sousContraintesColonnes[c1]--;
                    sousContraintesColonnes[c2]--;
				}
                occurencesCouples[c1][c2][v1][v2]++;
			}
		}
	}

    for(int c1 = 0; c1 < k; c1++)
    {
        for(int l = 0; l < N; l++)
        {
            int s1 = solution[l*k+c1];
            for(int s = 0; s < v; s++)
            {
                if(s != s1)
                {
                    deltas[c1][l][s] = calculerDelta(Mouvement(l, c1, s, s1)) - erreurs;
                }
                else
                {
                    deltas[c1][l][s] = 0;
                }
            }
        }
    }

    erreurs = (nbContraintes - contraintesSatisfaites);

    return(erreurs);
} // Fin fonction vérification

int CA_Solution::verifierSolution(Mouvement mv)
{
    //return erreurs + deltas[mv.mCol][mv.mLigne][mv.mSymbole];
    return calculerDelta(mv);
}

// Compte le nombre d'erreurs de la matrice resultant du mouvement mv
// Pour etre efficace, on ne recalcule pas toutes les contraintes. On regarde d'abord
// le symbole qui disparait, puis on enleve toutes les contraintes associees avec les autres colonnes de
// la ligne. Ensuite, on parcourt la colonne du mouvement de haut en bas, en regardant si le symbole dans la ligne courante
// est celui qu'on vient d'enlever : si c'est le cas, on retablit les contraintes associees dans la ligne.
// Enfin, on valide les contraintes induites par le rajout du nouveau symbole dans la case du mouvement.
int CA_Solution::calculerDelta(Mouvement mv)
{
    erreursDernierMv = erreurs;
    for(int i=0; i<mv.mCol; i++)
    {
        if(occurencesCouples[i][mv.mCol][solution[mv.mLigne*k+i]][mv.mAncienSymbole] == 1)
        {
            erreursDernierMv++;
        }
        if(occurencesCouples[i][mv.mCol][solution[mv.mLigne*k+i]][mv.mSymbole] == 0)
        {
            erreursDernierMv--;
        }
    }
    for(int i=mv.mCol + 1; i<k; i++)
    {
        if(occurencesCouples[mv.mCol][i][mv.mAncienSymbole][solution[mv.mLigne*k+i]] == 1)
        {
            erreursDernierMv++;
        }
        if(occurencesCouples[mv.mCol][i][mv.mSymbole][solution[mv.mLigne*k+i]] == 0)
        {
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

void CA_Solution::reinitialiserMouvement()
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

void CA_Solution::mouvementCritiqueSuivant()
{
    mvtCourant.mLigne++;
    trouverMouvementSuivant();
    //return mvtCourant;
}

void CA_Solution::trouverMouvementSuivant()
{
    bool trouve = false;
    while(!trouve && mvtCourant.mCol < k)
    {
        if(sousContraintesColonnes[mvtCourant.mCol] == 0)
        {
            mvtCourant.mCol++;
        }
        else
        {
            while(!trouve && mvtCourant.mSymbole < v)
            {
                if(sousContraintesSymboles[mvtCourant.mCol][mvtCourant.mSymbole] == 0)
                {
                    mvtCourant.mSymbole++;
                }
                else
                {
                    while(mvtCourant.mLigne < N && solution[mvtCourant.mLigne*k+mvtCourant.mCol] == mvtCourant.mSymbole)
                    {
                        mvtCourant.mLigne++;
                    }
                    if(mvtCourant.mLigne < N)
                    {
                        trouve = true;
                    }
                    if(mvtCourant.mLigne == N)
                    {
                        mvtCourant.mLigne = 0;
                        mvtCourant.mSymbole++;
                    }
                }
            }
            if(!trouve)
            {
                mvtCourant.mSymbole = 0;
                mvtCourant.mCol++;
            }
        }
    }
    mvtCourant.mAncienSymbole = solution[mvtCourant.mLigne*k + mvtCourant.mCol];
    if(!trouve)
    {
        mvtCourant.estFinal = true;
        mvtCourant.mLigne = 0;
        mvtCourant.mCol = 0;
        mvtCourant.mAncienSymbole = 0;
        mvtCourant.mSymbole = 0;
    }
}

void CA_Solution::reinitialiserMouvementCritique()
{
    mvtCourant.estFinal = false;
    mvtCourant.mLigne = 0;
    mvtCourant.mCol = 0;
    mvtCourant.mAncienSymbole = 0;
    mvtCourant.mSymbole = 0;
    trouverMouvementSuivant();
}

// Effectue le mouvement mv : remplace le symbole dans la matrice et recalcule les erreurs et les contraintes.
// Nous appelons verifierSolution(mv) dans le corps de cette fonction : nous pouvons donc nous contenter
// de copier les valeurs des contraintes generees par verifierSolution.
void CA_Solution::appliquerMouvement(Mouvement mv)
{
    erreursDernierMv = erreurs;
    for(int i=0; i<mv.mCol; i++)
    {
        int s = solution[mv.mLigne*k+i];
        if(occurencesCouples[i][mv.mCol][s][mv.mAncienSymbole] == 1)
        {
            erreursDernierMv++;
            sousContraintesSymboles[i][s]++;
            sousContraintesColonnes[i]++;
            sousContraintesSymboles[mv.mCol][mv.mAncienSymbole]++;
            sousContraintesColonnes[mv.mCol]++;
            /*for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s)
                {
                    deltas[mv.mCol][l][mv.mAncienSymbole]--;
                }
                if(solution[l*k+mv.mCol] == mv.mAncienSymbole && l != mv.mLigne)
                {
                    deltas[i][l][s]--;
                }
            }*/
        }
        /*if(occurencesCouples[i][mv.mCol][s][mv.mAncienSymbole] == 2)
        {
            for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s && solution[l*k+mv.mCol] == mv.mAncienSymbole && l != mv.mLigne)
                {
                    for(int s2 = 0; s2 < v; s2++)
                    {
                        if(s2 != s)
                        {
                            deltas[i][l][s2]++;
                        }
                        if(s2 != mv.mAncienSymbole)
                        {
                            deltas[mv.mCol][l][s2]++;
                        }
                    }
                }
            }
        }*/
        occurencesCouples[i][mv.mCol][s][mv.mAncienSymbole]--;
        if(occurencesCouples[i][mv.mCol][s][mv.mSymbole] == 0)
        {
            erreursDernierMv--;
            sousContraintesSymboles[i][s]--;
            sousContraintesColonnes[i]--;
            sousContraintesSymboles[mv.mCol][mv.mSymbole]--;
            sousContraintesColonnes[mv.mCol]--;
            /*for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s)
                {
                    deltas[mv.mCol][l][mv.mSymbole]++;
                }
                if(solution[l*k+mv.mCol] == mv.mSymbole)
                {
                    deltas[i][l][s]++;
                }
            }
            for(int s2 = 0; s2 < v; s2++)
            {
                if(s2 != s)
                {
                    deltas[i][mv.mLigne][s2]++;
                }
                if(s2 != mv.mSymbole)
                {
                    deltas[mv.mCol][mv.mLigne][s2]++;
                }
            }*/
        }
        /*if(occurencesCouples[i][mv.mCol][s][mv.mSymbole] == 1)
        {
            for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s && solution[l*k+mv.mCol] == mv.mSymbole)
                {
                    for(int s2 = 0; s2 < v; s2++)
                    {
                        if(s2 != s)
                        {
                            deltas[i][l][s2]++;
                        }
                        if(s2 != mv.mSymbole)
                        {
                            deltas[mv.mCol][l][s2]++;
                        }
                    }
                }
            }
        }*/
        occurencesCouples[i][mv.mCol][s][mv.mSymbole]++;
    }
    for(int i=mv.mCol + 1; i<k; i++)
    {
        int s = solution[mv.mLigne*k+i];
        if(occurencesCouples[mv.mCol][i][mv.mAncienSymbole][s] == 1)
        {
            erreursDernierMv++;
            sousContraintesSymboles[i][s]++;
            sousContraintesColonnes[i]++;
            sousContraintesSymboles[mv.mCol][mv.mAncienSymbole]++;
            sousContraintesColonnes[mv.mCol]++;
            /*for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s)
                {
                    deltas[mv.mCol][l][mv.mAncienSymbole]--;
                }
                if(solution[l*k+mv.mCol] == mv.mAncienSymbole && l != mv.mLigne)
                {
                    deltas[i][l][s]--;
                }
            }*/
        }
        /*if(occurencesCouples[mv.mCol][i][mv.mAncienSymbole][s] == 2)
        {
            for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s && solution[l*k+mv.mCol] == mv.mAncienSymbole && l != mv.mLigne)
                {
                    for(int s2 = 0; s2 < v; s2++)
                    {
                        if(s2 != s)
                        {
                            deltas[i][l][s2]++;
                        }
                        if(s2 != mv.mAncienSymbole)
                        {
                            deltas[mv.mCol][l][s2]++;
                        }
                    }
                }
            }
        }*/
        occurencesCouples[mv.mCol][i][mv.mAncienSymbole][s]--;
        if(occurencesCouples[mv.mCol][i][mv.mSymbole][s] == 0)
        {
            erreursDernierMv--;
            sousContraintesSymboles[i][s]--;
            sousContraintesColonnes[i]--;
            sousContraintesSymboles[mv.mCol][mv.mSymbole]--;
            sousContraintesColonnes[mv.mCol]--;
            /*for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s)
                {
                    deltas[mv.mCol][l][mv.mSymbole]++;
                }
                if(solution[l*k+mv.mCol] == mv.mSymbole)
                {
                    deltas[i][l][s]++;
                }
            }
            for(int s2 = 0; s2 < v; s2++)
            {
                if(s2 != s)
                {
                    deltas[i][mv.mLigne][s2]++;
                }
                if(s2 != mv.mSymbole)
                {
                    deltas[mv.mCol][mv.mLigne][s2]++;
                }
            }*/
        }
        /*if(occurencesCouples[mv.mCol][i][mv.mSymbole][s] == 1)
        {
            for(int l = 0; l < N; l++)
            {
                if(solution[l*k+i] == s && solution[l*k+mv.mCol] == mv.mSymbole)
                {
                    for(int s2 = 0; s2 < v; s2++)
                    {
                        if(s2 != s)
                        {
                            deltas[i][l][s2]++;
                        }
                        if(s2 != mv.mSymbole)
                        {
                            deltas[mv.mCol][l][s2]++;
                        }
                    }
                }
            }
        }*/
        occurencesCouples[mv.mCol][i][mv.mSymbole][s]++;
    }
    solution[k*mv.mLigne+mv.mCol] = mv.mSymbole;
    erreurs = erreursDernierMv;
    /*for(int c1 = 0; c1 < k; c1++)
    {
        for(int l = 0; l < N; l++)
        {
            int s1 = solution[l*k+c1];
            for(int s = 0; s < v; s++)
            {
                if(s != s1)
                {
                    deltas[c1][l][s] = calculerDelta(Mouvement(l, c1, s, s1)) - erreurs;
                }
                else
                {
                    deltas[c1][l][s] = 0;
                }
            }
        }
    }*/

}

// Supprime la derniere ligne de la matrice
void CA_Solution::enleverLigne()
{
    solution.resize(k*(--N));

    for(int i1=0; i1<k; i1++)
    {
        delete[] deltas[i1][N];
    }
}

// effectue un mouvement aléatoire dans une configuration donnée
Mouvement CA_Solution::mouvementAleatoire()
{
    // Tirage au sort d'une colonne et d'une ligne
    Mouvement mv;
    mv.mCol = rand()%k;
    mv.mLigne = rand()%N;

    // Tirage au sort d'un nouveau symbole
    int symboleActuel = solution[k*mv.mLigne+mv.mCol];
    mv.mAncienSymbole = symboleActuel;
    int nouveauSymbole = symboleActuel;
    while(nouveauSymbole == symboleActuel)
    {
        nouveauSymbole = rand()%v;
    }

    mv.mSymbole = nouveauSymbole;

    return mv;
}
