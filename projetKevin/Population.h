#include "CA_Solution.h"

class Population
{
public:
	Population(int m, int n);
	~Population(void);

	void remplirGeneration(int v, int k, int N);
	void calculerCouts(int format);
	void enleverIndividu(int index);

	void croisementGlouton();
	void croisementRand();
	void croisementEchange();
	void descente(int nombreEssais, bool croisement);
	void mutation(float pourcent, bool croisement);

	int taille;
	int tailleParents;
	int tailleEnfants;

	CA_Solution** generation;
	int* couts;
};

