#include "generateur.h"

// Calcul des couts de la population (qui peut etre la population des parents ou la population totale)
// Cette fonction trie aussi les couts et les CA_Solution par ordre de cout croissant. Le tri est
// effectue avec un quicksort pour optimiser le temps d'execution pour des grandes tailles de populations.
void Generateur::EvolutionCalculerCouts(CA_Solution **population, int *couts, int taille)
{
    int cnt, cout, left, right, pivot, i;
    CA_Solution **populationCopy = new CA_Solution*[taille];
    couts[0] = population[0]->verifierSolution();
    populationCopy[0] = population[0];
    //tri dichotimique
    for(cnt = 1; cnt < taille; cnt++)
    {
        // Calcul du cout courant
        if(population[cnt]->erreurs != -1)
        {
            cout = population[cnt]->erreurs;
        }
        else
        {
            cout = population[cnt]->verifierSolution();
        }
        left = 0;
        right = cnt - 1;
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
            populationCopy[i+1] = populationCopy[i];
        }
        // Insertion
        couts[left] = cout;
        populationCopy[left] = population[cnt];
    }
    // Copie des pointeurs tries vers le tableau de pointeurs passe en parametre
    for(cnt = 0; cnt < taille; cnt++)
    {
        population[cnt] = populationCopy[cnt];
    }
    delete[] populationCopy;
}

// Croisement sur une population, effectuee sur les symboles. Deux parents choisis aleatoirement produisent un enfant
// qui a pour symboles les symboles des deux parents, choisis aleatoirement avec une probabilite uniforme (0.5).
// On produit 'tailleEnfants' enfants.
void Generateur::EvolutionCroisementSymbole(CA_Solution **population, int tailleParents, int tailleEnfants, int v, int k, int N)
{
    int e, p1, p2, indice, r;
    for(e = 0; e < tailleEnfants; e++)
    {
        p1 = rand()%tailleParents;
        p2 = rand()%tailleParents;
        population[tailleParents+e] = new CA_Solution(v, k, N);
        for(indice = 0; indice < k*N; indice++)
        {
            r = rand()%2;
            if(r == 0)
            {
                population[tailleParents+e]->solution[indice] = population[p1]->solution[indice];
            }
            else
            {
                population[tailleParents+e]->solution[indice] = population[p2]->solution[indice];
            }
        }
    }
}

// Croisement sur une population, effectuee sur les lignes. Deux parents choisis aleatoirement produisent un enfant
// qui a pour lignes les lignes des deux parents, choisies aleatoirement avec une probabilite uniforme (0.5).
// On produit 'tailleEnfants' enfants.
void Generateur::EvolutionCroisementLigne(CA_Solution **population, int tailleParents, int tailleEnfants, int v, int k, int N)
{
    int e, p1, p2, ligne, r, c;
    for(e = 0; e < tailleEnfants; e++)
    {
        p1 = rand()%tailleParents;
        p2 = rand()%tailleParents;
        population[tailleParents+e] = new CA_Solution(v, k, N);
        for(ligne = 0; ligne < N; ligne++)
        {
            r = rand()%2;
            if(r == 0)
            {
                for(c = 0; c < k; c++)
                {
                    population[tailleParents+e]->solution[ligne*k + c] = population[p1]->solution[ligne*k + c];
                }
            }
            else
            {
                for(c = 0; c < k; c++)
                {
                    population[tailleParents+e]->solution[ligne*k + c] = population[p2]->solution[ligne*k + c];
                }
            }
        }
    }
}

// Mutation des enfants. On modifie 'pourcent' symboles de chaque enfant, le nouveau symbole
// etant determine aleatoirement.
void Generateur::EvolutionMutation(CA_Solution **populationEnfants, int tailleEnfants, int v, int k, int N, float pourcent)
{
    int e, indice, col, ligne, symbole;
    int limite = pourcent*k*N;
    /*if(limite == 0)
    {
        limite = 1;
    }*/
    for(e = 0; e < tailleEnfants; e++)
    {
        for(indice = 0; indice < limite; indice++)
        {
            col = rand()%k;
            ligne = rand()%N;
            symbole = rand()%v;
            populationEnfants[e]->solution[ligne * k + col] = symbole;
        }
    }
}

Resultats Generateur::TesterEvolution(int v, int k, int N, int tailleParents, int tailleEnfants, float pourcentMutation, TYPE_CROISEMENT type)
{
    // Variables pour l'algorithme tabou de base
    int iteration = 1;
    int i;
    CA_Solution **population = new CA_Solution*[tailleParents+tailleEnfants];
    int *couts = new int[tailleParents+tailleEnfants];
    CA_Solution* meilleureConfig = nullptr; // Meilleures des configurations testées jusqu'alors

    // Initialisation de la population
    for(i = 0; i < tailleParents; i++)
    {
        population[i] = ConfigurationAleatoire(v, k, N);
    }
    // Calcul et tri de leurs couts
    EvolutionCalculerCouts(population, couts, tailleParents);

    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now();
    chrono::duration<double> duree;
    double dureeMillisecondes;
    const double tempsMax = m_tempsMax*5.4/8.6;

    // Initialisation de la meilleure config
    int coutMeilleure = couts[0];
    meilleureConfig = new CA_Solution(*(population[0]));

    duree = chrono::system_clock::now()-dateDebut;
    dureeMillisecondes = 1000*duree.count();

    // Critere d'arret : une minute de temps d'execution, ou bien une solution trouvee
    while(coutMeilleure > 0 && dureeMillisecondes < tempsMax)
    {
        // Croisement
        if(type == CROISEMENT_LIGNE)
        {
            EvolutionCroisementLigne(population, tailleParents, tailleEnfants, v, k, N);
        }
        else
        {
            EvolutionCroisementSymbole(population, tailleParents, tailleEnfants, v, k, N);
        }

        // Mutation
        EvolutionMutation(&population[tailleParents], tailleEnfants, v, k, N, pourcentMutation);

        // Calcul et des couts, tri dans l'ordre croissant du tableau des couts et tri de la population
        // dans le meme ordre
        EvolutionCalculerCouts(population, couts, tailleParents+tailleEnfants);

        // Selection
        for(i = tailleParents; i < tailleParents+tailleEnfants; i++)
        {
            delete population[i];
        }

        // Mise à jour de la meilleure configuration
        if(couts[0] < coutMeilleure)
        {
            delete meilleureConfig;
            meilleureConfig = new CA_Solution(*(population[0]));
            coutMeilleure = couts[0];
            //cout << "Meilleure solution trouvee a l'iteration : " << iteration << " de cout : " << coutMeilleure << endl;
        }

        iteration++;

        // Actualisation du temps passe
        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();
    }
    delete meilleureConfig;
    for(i = 0; i < tailleParents; i++)
    {
        delete population[i];
    }
    delete[] population;
    delete[] couts;
    return Resultats(dureeMillisecondes, coutMeilleure, iteration);
}
