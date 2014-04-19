#include "Population.h"


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
	int cnt, cout, left, right, pivot, i;
	Population *populationCopy = new Population(tailleParents, tailleEnfants);
	couts[0] = generation[0]->verifierSolution();
	populationCopy->generation[0] = generation[0];

	// Tri dichotimique
	for(cnt = 1; cnt < format; cnt++)
	{
		// Calcul du cout courant
		if(generation[cnt]->erreurs != -1)
		{
			cout = generation[cnt]->erreurs;
		}
		else
		{
			cout = generation[cnt]->verifierSolution();
		}
		left = 0;
		right = cnt-1;

		// Determination de la place du nouveau cout dans le tableau (trie)
		while(left != right)
		{
			pivot = (left+right)/2;
			if(cout > couts[pivot])
			{
				left = pivot + 1;
			}
			if(cout < couts[pivot])
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
			if(cout == couts[pivot])
			{
				left = right = pivot;
			}
		}
		// Ajustement
		if(couts[left] < cout)
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
		couts[left] = cout;
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

	int e, p1, p2, ligne, r, c;
	for(e=0; e<tailleEnfants; e++) {
		p1 = rand()%tailleParents;
		p2 = rand()%tailleParents;
		generation[tailleParents+e] = new CA_Solution(v, k, 0);

		for(ligne = 0; ligne < N; ligne++)
		{
			CA_Solution* hypo1 = new CA_Solution(*generation[tailleParents+e]);
			CA_Solution* hypo2 = new CA_Solution(*generation[tailleParents+e]);

			hypo1->ajouterLigne();
			hypo2->ajouterLigne();

			for(int j=0; j<k; j++) {
				hypo1->solution[ligne][j] = generation[p1]->solution[ligne][j];
				hypo2->solution[ligne][j] = generation[p2]->solution[ligne][j];
			}

			int err1 = hypo1->verifierSolution();
			int err2 = hypo2->verifierSolution();

			if (err1<err2) {
				generation[tailleParents+e] = hypo1;
				delete hypo2;
			} else { 
				generation[tailleParents+e] = hypo2;
				delete hypo1;
			}
		}
		cout << "Enfant " << e << " genere" <<endl;
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
	// TODO
}

// Descente
void Population::descente() {
	// TODO
}

// Opérateur de mutation
void Population::mutation(float pourcent)
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
		for(indice = 0; indice < limite; indice++)
		{
			col = rand()%k;
			ligne = rand()%N;
			symbole = rand()%v;
			generation[tailleParents+e]->solution[ligne][col] = symbole;
		}
	}
}