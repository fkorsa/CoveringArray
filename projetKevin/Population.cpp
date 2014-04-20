#include "Population.h"
#include "Mouvement.h"
#include <list>
#include <chrono>

Population::Population(int m, int n)
{
	tailleParents = m;
	tailleEnfants = n;
	taille = tailleParents + tailleEnfants;

	generation = new CA_Solution*[taille];
	couts = new int[taille];
}


Population::~Population(void)
{
}


void Population::remplirGeneration(int v, int k, int N)
{
	// On remplit la population initiale des parents par des configurations aléatoires
	for(int g=0; g<tailleParents; g++) {
		int** matrice = new int*[N];
		for(int li=0; li<N; li++)
		{
			matrice[li] = new int[k];
			for(int j=0; j<k ; j++) {
				int symb = rand()%v;
				matrice[li][j] = symb;
			}
		}
		generation[g] = new CA_Solution(v, k, N);
		generation[g]->solution = matrice;
	}
}


void Population::calculerCouts(int format)
{
	int cnt, cost, left, right, pivot, i;
	Population *populationCopy = new Population(tailleParents, tailleEnfants);
	couts[0] = generation[0]->verifierSolution();
	populationCopy->generation[0] = generation[0];

	// Tri dichotimique
	for(cnt = 1; cnt < format; cnt++)
	{
		// Calcul du cout courant
		if(generation[cnt]->erreurs != -1)
		{
			cost = generation[cnt]->erreurs;
		}
		else
		{
			cost = generation[cnt]->verifierSolution();
		}
		left = 0;
		right = cnt-1;

		// Determination de la place du nouveau cout dans le tableau (trie)
		while(left != right)
		{
			pivot = (left+right)/2;
			if(cost > couts[pivot])
			{
				left = pivot + 1;
			}
			if(cost < couts[pivot])
			{
				if(right==left + 1)
				{
					right = left;
				}
				else
				{
					right = pivot - 1;
				}
			}
			if(cost == couts[pivot])
			{
				left = right = pivot;
			}
		}
		// Ajustement
		if(couts[left] < cost)
		{
			left++;
		}
		// Decalage vers la droite pour inserer le nouvel element
		for(i = cnt - 1; i > left - 1; i--)
		{
			couts[i+1] = couts[i];
			populationCopy->generation[i+1] = populationCopy->generation[i];
		}
		// Insertion
		couts[left] = cost;
		populationCopy->generation[left] = generation[cnt];
	}
	// Copie des pointeurs tries vers le tableau de pointeurs passe en parametre
	for(cnt = 0; cnt < format; cnt++)
	{
		generation[cnt] = populationCopy->generation[cnt];
	}
	delete populationCopy;
}


void Population::enleverIndividu(int index)
{
	delete generation[index];
}


// Croisement optimisé sur les lignes avec critère glouton
void Population::croisementGlouton() {
	int v = generation[0]->v;
	int k = generation[0]->k;
	int N = generation[0]->N;

	int e, p1, p2, ligne, j;
	for(e=0; e<tailleEnfants; e++) {
		p1 = rand()%tailleParents;
		p2 = rand()%tailleParents;
		generation[tailleParents+e] = new CA_Solution(v, k, N);

		bool**** aResoudre = new bool***[k];
		for(int i1=0; i1<k; i1++) {
			aResoudre[i1] = new bool**[k];
			for(int i2=0; i2<k; i2++) {
				aResoudre[i1][i2] = new bool*[v];
				for(int i3=0; i3<v; i3++) {
					aResoudre[i1][i2][i3] = new bool[v];
					for(int i4=0; i4<v; i4++) {
						aResoudre[i1][i2][i3][i4] = generation[tailleParents+e]->contraintes[i1][i2][i3][i4];
					}
				}
			}
		}

		for(ligne = 0; ligne < N; ligne++)
		{
			int* ligne1 = new int[k];
			int* ligne2 = new int[k];
			int score1 = 0;
			int score2 = 0;

			for(int j=0; j<k; j++) {
				ligne1[j] = generation[p1]->solution[ligne][j];
				ligne2[j] = generation[p2]->solution[ligne][j];
			}

			// On compte le nombre de contraintes qui seraient résolues par chaque ligne
			for(int k1=0; k1<k-1; k1++) {
				int v1_l1 = ligne1[k1];
				int v1_l2 = ligne2[k1];
				for(int k2=k1+1; k2<k; k2++) {
					int v2_l1 = ligne1[k2];
					int v2_l2 = ligne2[k2];
					if(!aResoudre[k1][k2][v1_l1][v2_l1]) {
						score1++;
					}
					if(!aResoudre[k1][k2][v1_l2][v2_l2]) {
						score2++;
					}
				}
			}

			if(score1>score2) { // On copie la ligne du parent 1 dans l'enfant
				for(int j=0; j<k; j++) {
					generation[tailleParents+e]->solution[ligne][j] = ligne1[j];
				}
			}
			else { // On copie la ligne du parent 2 dans l'enfant
				for(int j=0; j<k; j++) {
					generation[tailleParents+e]->solution[ligne][j] = ligne2[j];
				}
			}

			// On résout maintenant les contraintes
			for(int k1=0; k1<k-1; k1++) {
				int v1 = generation[tailleParents+e]->solution[ligne][k1];
				for(int k2=k1+1; k2<k; k2++) {
					int v2 = generation[tailleParents+e]->solution[ligne][k2];
					aResoudre[k1][k2][v1][v2] = true;
				}
			}
		}
	}
}

// Croisement aléatoire avec choix sur les lignes
void Population::croisementRand() {
	int v = generation[0]->v;
	int k = generation[0]->k;
	int N = generation[0]->N;

	int e, p1, p2, ligne, r, c;
	for(e=0; e<tailleEnfants; e++) {
		p1 = rand()%tailleParents;
		p2 = rand()%tailleParents;
		generation[tailleParents+e] = new CA_Solution(v, k, N);

		for(ligne = 0; ligne < N; ligne++)
		{
			r = rand()%2;
			if(r == 0)
			{
				for(c = 0; c < k; c++)
				{
					generation[tailleParents+e]->solution[ligne][c] = generation[p1]->solution[ligne][c];
				}
			}
			else
			{
				for(c = 0; c < k; c++)
				{
					generation[tailleParents+e]->solution[ligne][c] = generation[p2]->solution[ligne][c];
				}
			}
		}
	}
}

// Croisement avec échange de lignes entre deux configurations
void Population::croisementEchange() {
	int v = generation[0]->v;
	int k = generation[0]->k;
	int N = generation[0]->N;

	int e, p1, p2, ligne, r, c;
	for(e=0; e<tailleEnfants/2; e++) { // Deux enfants sont générés par chaque couple
		p1 = rand()%tailleParents;
		p2 = rand()%tailleParents;
		int e1 = 2*e;
		int e2 = 2*e+1;

		generation[tailleParents+e1] = new CA_Solution(*generation[p1]);
		generation[tailleParents+e2] = new CA_Solution(*generation[p2]);

		CA_Solution* test1 = new CA_Solution(*generation[p1]);
		CA_Solution* test2 = new CA_Solution(*generation[p2]);

		int worstLigne1 = 0;
		int worstLigne2 = 0;

		int perfBestTronc1 = INT_MAX;
		int perfBestTronc2 = INT_MAX;

		for(int li=0; li<N; li++) {
			int perfTronc1 = test1->verifierSolutionPartielle(li);
			int perfTronc2 = test2->verifierSolutionPartielle(li);

			if(perfTronc1 < perfBestTronc1) { // On met à jour les meilleures troncatures donc indice pire ligne
				perfBestTronc1 = perfTronc1;
				worstLigne1 = li;
			}
			if(perfTronc2 < perfBestTronc2) {
				perfBestTronc2 = perfTronc2;
				worstLigne2 = li;
			}
		}

		// On procède aux échanges de lignes pour créer les enfants
		int* tampon = new int[k];
		for(int i=0 ; i<k ; i++) {
			tampon[i] = generation[tailleParents+e1]->solution[worstLigne1][i];
			generation[tailleParents+e1]->solution[worstLigne1][i] = generation[tailleParents+e2]->solution[worstLigne2][i];
			generation[tailleParents+e2]->solution[worstLigne2][i] = tampon[i];
		}
	}
}

// Descente
void Population::descente(int nombreEssais, bool croisement) {
	int v = generation[0]->v;
	int k = generation[0]->k;
	int N = generation[0]->N;

	int e;
	for(e = 0; e < tailleEnfants; e++)
	{
		int index = e;
		if(croisement) {
			index += tailleParents;
		}
		int itCpt = 0;
		int coutActuelle, coutTest;
		Mouvement mouvementActuel;
		CA_Solution* configInit = generation[index];
		CA_Solution* configTestee = configInit;
		CA_Solution* meilleureConfig = new CA_Solution(*configInit);
		int coutMeilleure = generation[index]->verifierSolution();
		while(itCpt < nombreEssais && (coutMeilleure > 0))
		{
			// On génère un voisin aléatoire de S
			mouvementActuel = configTestee->mouvement();
			coutActuelle = configTestee->erreurs;
			coutTest = configTestee->verifierSolution(mouvementActuel);

			int delta = coutTest - coutActuelle;

			if(delta <= 0)
			{
				configTestee->appliquerMouvement(mouvementActuel);
				// Mise à jour de la meilleure configuration
				delete meilleureConfig;
				meilleureConfig = new CA_Solution(*configTestee);
				coutMeilleure = coutTest;
			}
			itCpt++;
		}
		generation[index] = new CA_Solution(*meilleureConfig);
	}
}

// Opérateur de mutation
void Population::mutation(float pourcent, bool croisement)
{
	int v = generation[0]->v;
	int k = generation[0]->k;
	int N = generation[0]->N;

	int e, indice, col, ligne, symbole;
	int limite = pourcent*k*N;

	if(limite == 0)
	{
		limite = 1;
	}

	for(e = 0; e < tailleEnfants; e++)
	{
		int index = e;
		if(croisement) {
			index += tailleParents;
		}
		for(indice = 0; indice < limite; indice++)
		{
			col = rand()%k;
			ligne = rand()%N;
			symbole = rand()%v;
			generation[index]->solution[ligne][col] = symbole;
		}
	}
}