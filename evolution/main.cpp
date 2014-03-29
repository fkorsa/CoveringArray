#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <chrono>
#include <list>

using namespace std;

#include "CA_Solution.h"
#include "Mouvement.h"

// Lire une matrice a partir d'un fichier
CA_Solution* lireFichierSolution(const char* chemin)
{
	ifstream infile(chemin);
	int v = 0;
	int k = 0;
	int N = 0;
	int erreurs = 0;

	infile >> v >> k >> N >> erreurs;

	vector<int> solution(N*k);
	int index = 0;
	int val = 0;

	while (infile >> val)
	{
		solution[index] = val;
		index++;
	}
	CA_Solution* ca_sol = new CA_Solution(v,k,solution);
	return ca_sol;
}

// Generer une matrice aleatoirement
CA_Solution* configurationAleatoire(int v, int k, int N) {
	vector<int> matrice(N*k);

    for(int i=0; i<N*k; i++)
    {
		int symb = rand()%v;
        matrice[i] = symb;
	}

	CA_Solution* configuration = new CA_Solution(v, k, matrice);
	return configuration;
}

// Calcul des couts de la population (qui peut etre la population des parents ou la population totale)
// Cette fonction trie aussi les couts et les CA_Solution par ordre de cout croissant. Le tri est
// effectue avec un quicksort pour optimiser le temps d'execution pour des grandes tailles de populations.
void calculerCouts(CA_Solution **population, int *couts, int taille)
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

enum TYPE_CROISEMENT
{
    CROISEMENT_SYMBOLE,
    CROISEMENT_LIGNE
};

// Croisement sur une population, effectuee sur les symboles. Deux parents choisis aleatoirement produisent un enfant
// qui a pour symboles les symboles des deux parents, choisis aleatoirement avec une probabilite uniforme (0.5).
// On produit 'tailleEnfants' enfants.
void croisementSymbole(CA_Solution **population, int tailleParents, int tailleEnfants, int v, int k, int N)
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
void croisementLigne(CA_Solution **population, int tailleParents, int tailleEnfants, int v, int k, int N)
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
void mutation(CA_Solution **populationEnfants, int tailleEnfants, int v, int k, int N, float pourcent)
{
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
            populationEnfants[e]->solution[ligne * k + col] = symbole;
        }
    }
}

// Calcul de l'indicateur de diversite. Cette fonction est tres couteuse en temps de calcul,
// elle n'est donc pas utilisee lors de la generation des resultats mais seulement au moment
// de la determination des valeurs des parametres.
int calculerDiversification(CA_Solution **population, int taille, int k, int N)
{
    int i, j, indice;
    int d = 0;
    for(i = 0; i < taille - 1; i++)
    {
        for(j = i+1; j < taille; j++)
        {
            for(indice = 0; indice < k*N; indice++)
            {
                if(population[i]->solution[indice] != population[j]->solution[indice])
                {
                    d++;
                }
            }
        }
    }
    return d;
}

// Algorithme genetique
CA_Solution* evolution(int v, int k, int N, int tailleParents, int tailleEnfants,
                       ofstream *fichier, float pourcentMutation, TYPE_CROISEMENT type,
                       bool afficherDiversite)
{
    // Variables pour l'algorithme tabou de base
    ofstream& fichierLocal = *fichier;
    int iteration = 1;
    int i;
    CA_Solution **population = new CA_Solution*[tailleParents+tailleEnfants];
    int *couts = new int[tailleParents+tailleEnfants];
    CA_Solution* meilleureConfig = nullptr; // Meilleures des configurations testées jusqu'alors

    // Initialisation de la population
    for(i = 0; i < tailleParents; i++)
    {
        population[i] = configurationAleatoire(v, k, N);
    }
    // Calcul et tri de leurs couts
    calculerCouts(population, couts, tailleParents);

    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now();   
    chrono::duration<double> duree;
    double dureeMillisecondes;
    const double tempsMax = 60000*5.4/8.6;

    // Initialisation de la meilleure config
    int coutMeilleure = couts[0];
    meilleureConfig = new CA_Solution(*(population[0]));

    duree = chrono::system_clock::now()-dateDebut;
    dureeMillisecondes = 1000*duree.count();

    // Critere d'arret : une minute de temps d'execution, ou bien une solution trouvee
    while((coutMeilleure > 0 || fichier) && dureeMillisecondes < tempsMax)
    {
        // Croisement
        if(type == CROISEMENT_LIGNE)
        {
            croisementLigne(population, tailleParents, tailleEnfants, v, k, N);
        }
        else
        {
            croisementSymbole(population, tailleParents, tailleEnfants, v, k, N);
        }

        // Mutation
        mutation(&population[tailleParents], tailleEnfants, v, k, N, pourcentMutation);

        if(afficherDiversite)
        {
            cout << "Diversite : " << calculerDiversification(population, tailleParents + tailleEnfants, k, N) << endl;
        }

        // Enregistrement de la progression du cout dans un fichier
        if(fichier && iteration%50 == 0)
        {
            fichierLocal << iteration << " " << calculerDiversification(population, tailleParents + tailleEnfants, k, N)
                         << " " <<coutMeilleure << endl;
            cout << iteration << " " << coutMeilleure << endl;
        }

        // Calcul et des couts, tri dans l'ordre croissant du tableau des couts et tri de la population
        // dans le meme ordre
        calculerCouts(population, couts, tailleParents+tailleEnfants);

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
            cout << "Meilleure solution trouvee a l'iteration : " << iteration << " de cout : " << coutMeilleure << endl;
        }

        iteration++;

        // Actualisation du temps passe
        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();
	}
    // Enregistrement pour les statistiques
    meilleureConfig->nbIt = iteration;

	return(meilleureConfig);
}

// Lit le fichier "inputData" pour executer 'nbExec' fois l'algorithme tabou, et relever des
// statistiques dans le fichier "output"
void genererResultats(int nbExec, TYPE_CROISEMENT type)
{
    ifstream infile("inputData");
    ofstream outfile("output");
    int nbSymboles, nbColonnes, nbLignes;
    double coutMoyen, itMoyen, mvMoyen, tempsMoyen;
    double coutMin, itMin, mvMin, tempsMin;
    double coutMax, itMax, mvMax, tempsMax, temps;
    chrono::time_point<chrono::system_clock> start;
    chrono::duration<double> realTime;
    CA_Solution* solution;
    if(outfile.is_open())
    {
        outfile << "NombreSymboles NombreColonnes NombreLignes CoutMin CoutMoyen CoutMax ItMin ItMoyen ItMax MvMin MvMoyen MvMax TempsMin TempsMoyen TempsMax" << endl;
        // Pour chaque configuration dans le fichier "inputData"
        while (infile >> nbSymboles >> nbColonnes >> nbLignes)
        {
            coutMoyen = 0;
            itMoyen = 0;
            mvMoyen = 0;
            tempsMoyen = 0;
            coutMin = 0;
            itMin = 0;
            mvMin = 0;
            tempsMin = 0;
            coutMax = 0;
            itMax = 0;
            mvMax = 0;
            tempsMax = 0;
            cout << nbSymboles << " "
                    << nbColonnes << " "
                    << nbLignes << " " << endl;
            // On execute l'algorithme plusieurs fois
            for(int i = 0; i < nbExec; i++)
            {
                start = chrono::system_clock::now();
                solution = evolution(nbSymboles, nbColonnes, nbLignes, 20, 20, nullptr, 0.0001, type, false);
                realTime = chrono::system_clock::now()-start;
                temps = 1000*realTime.count();
                if(i == 0)
                {
                    coutMin = solution->erreurs;
                    itMin = solution->nbIt;
                    mvMin = solution->nbMvt;
                    tempsMin = temps;
                }
                if(coutMin > solution->erreurs)
                {
                    coutMin = solution->erreurs;
                }
                if(itMin > solution->nbIt)
                {
                    itMin = solution->nbIt;
                }
                if(mvMin > solution->nbMvt)
                {
                    mvMin = solution->nbMvt;
                }
                if(tempsMin > temps)
                {
                    tempsMin = temps;
                }
                if(coutMax < solution->erreurs)
                {
                    coutMax = solution->erreurs;
                }
                if(itMax < solution->nbIt)
                {
                    itMax = solution->nbIt;
                }
                if(mvMax < solution->nbMvt)
                {
                    mvMax = solution->nbMvt;
                }
                if(tempsMax < temps)
                {
                    tempsMax = temps;
                }
                coutMoyen += solution->erreurs;
                itMoyen += solution->nbIt;
                mvMoyen += solution->nbMvt;
                tempsMoyen += temps;
                delete solution;
            }
            coutMoyen /= nbExec;
            itMoyen /= nbExec;
            mvMoyen /= nbExec;
            tempsMoyen /= nbExec;
            tempsMoyen *= (8.6/5.4);
            tempsMin *= (8.6/5.4);
            tempsMax *= (8.6/5.4);
            // On enregistre les donnees dans le fichier output
            outfile << nbSymboles << " "
                    << nbColonnes << " "
                    << nbLignes << " "
                    << coutMin << " "
                    << coutMoyen << " "
                    << coutMax << " "
                    << itMin << " "
                    << itMoyen << " "
                    << itMax << " "
                    << mvMin << " "
                    << mvMoyen << " "
                    << mvMax << " "
                    << tempsMin << " "
                    << tempsMoyen << " "
                    << tempsMax << endl;
        }
        outfile.close();
    }
}

int main()
{
    int seed = time(NULL);
    srand(seed);

    // Pour generer toutes les stats sur les differentes configs, decommenter cette section
    genererResultats(1, CROISEMENT_LIGNES);

    // Pour enregistrer l'evolution de la diversite dans le fichier "output" sur une config donne, decommenter
    // cette section
    /*ofstream outfile("output");
    if(outfile.is_open())
    {
        CA_Solution* configEvolution = evolution(3, 20, 17, 20, 20, &outfile, 0.000, CROISEMENT_LIGNE, false);
        cout << "Erreurs : " << configEvolution->erreurs << " iterations : " << configEvolution->nbIt << endl;
        delete configEvolution;
    }*/

    // Pour effectuer des tests sur une configuration en particulier, decommenter cette section
    /*CA_Solution* configEvolution = evolution(3, 20, 20, 20, 20, nullptr, 0.001, CROISEMENT_SYMBOLE, false);
    cout << "Erreurs : " << configEvolution->erreurs << " iterations : " << configEvolution->nbIt << endl;*/
}
