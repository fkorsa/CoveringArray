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

void calculerCouts(CA_Solution **population, int *couts, int taille)
{
    int cnt, cout, left, right, pivot, i;
    CA_Solution **populationCopy = new CA_Solution*[taille];
    couts[0] = population[0]->verifierSolution();
    populationCopy[0] = population[0];
    // 1 3 4
    for(cnt = 1; cnt < taille; cnt++)
    {
        cout = population[cnt]->verifierSolution();
        left = 0;
        right = cnt - 1;
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
        if(couts[left] < cout)
        {
            left++;
        }
        for(i = cnt - 1; i > left - 1; i--)
        {
            couts[i+1] = couts[i];
            populationCopy[i+1] = population[i];
        }
        couts[left] = cout;
        populationCopy[left] = population[cnt];
    }
    delete[] population;
    population = populationCopy;
}

// Algorithme tabou
CA_Solution* evolution(int v, int k, int N, int tailleParents, int tailleEnfants, ofstream *fichier)
{
    // Variables pour l'algorithme tabou de base
    ofstream& fichierLocal = *fichier;
    int iteration = 1;
    int fmin, fmax, i;
    CA_Solution **populationParents = new CA_Solution*[tailleParents];
    CA_Solution **populationEnfants = new CA_Solution*[tailleEnfants];
    int *coutsParents = new int[tailleParents];
    int *coutsEnfants = new int[tailleEnfants];
    CA_Solution* meilleureConfig = nullptr; // Meilleures des configurations testées jusqu'alors

    for(i = 0; i < tailleParents; i++)
    {
        populationParents[i] = configurationAleatoire(v, k, N);
    }
    calculerCouts(populationParents, coutsParents, tailleParents);

    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now(), dateDebutPhase = dateDebut;
    chrono::duration<double> duree, dureePhase;
    double dureeMillisecondes;
    const double tempsMax = 60000*5.4/8.6;

#if 0
    int coutMeilleure = configInit->verifierSolution();
    meilleureConfig->erreurs = coutMeilleure;
    fmin = fmax = coutMeilleure;

    duree = chrono::system_clock::now()-dateDebut;
    dureeMillisecondes = 1000*duree.count();

    // Critere d'arret : une minute de temps d'execution, ou bien une solution trouvee
    while((coutMeilleure > 0 || fichier) && dureeMillisecondes < tempsMax)
    {
        configTestee->reinitialiserMouvementCourant();
        listeMeilleurs.clear();

        dureePhase = chrono::system_clock::now()-dateDebutPhase;
        if(!diversification || dureePhase.count() < 9)
        {
            // Parcours de tous les voisins et determination du meilleur, non tabou
            choixMouvement(configTestee, &listeMeilleurs, listeTaboue, iteration, coutMeilleure);
        }
        else if(diversification && dureePhase.count() >= 9)
        {
            // Parcours des voisins, avec une fonction de cout prenant en compte la frequence d'apparition des symboles
            choixDiversification(configTestee, &listeMeilleurs, listeTaboue, iteration, presence, dernierePresence, frequence, coutMeilleure);
        }
        if(dureePhase.count() >= 10)
        { // Fin de la phase de diversification
            dateDebutPhase = chrono::system_clock::now();
        }

        mv = listeMeilleurs.front();

        // On departage les mouvements ex aequo aleatoirement, selon une distribution uniforme
        double tailleListe = listeMeilleurs.size();
        if(tailleListe > 1)
        {
            double r = (double)rand()/RAND_MAX;
            double i = 1;
            for(list<Mouvement>::iterator it = listeMeilleurs.begin(); it != listeMeilleurs.end(); it++, i++)
            {
                if(r < i/tailleListe)
                {
                    mv = *it;
                    break;
                }
            }
        }

		// Application des conséquences
        if(tailleListe > 0)
        {
            configTestee->appliquerMouvement(mv); // Déplacement entériné
            // L'ancien symbole est maintenant tabou
            listeTaboue[mv.mLigne][mv.mCol][mv.mAncienSymbole] = iteration + longueurListe;
            // Actualisation des memoires a long terme
            if(diversification)
            {
                presence[mv.mLigne][mv.mCol][mv.mAncienSymbole] = false;
                presence[mv.mLigne][mv.mCol][mv.mSymbole] = true;
                frequence[mv.mLigne][mv.mCol][mv.mAncienSymbole] += iteration - dernierePresence[mv.mLigne][mv.mCol][mv.mAncienSymbole];
                dernierePresence[mv.mLigne][mv.mCol][mv.mAncienSymbole] = iteration;
            }
            if(configTestee->erreurs < coutMeilleure)
            { // Mise à jour de la meilleure configuration
                delete meilleureConfig;
                meilleureConfig = new CA_Solution(*configTestee);
                coutMeilleure = configTestee->erreurs;
                //cout << "Meilleure solution trouvee a l'iteration : " << iteration << " de cout : " << coutMeilleure << endl;
            }
            vraisMouvementsTotal++;
        }

        iteration++;

        // Actualisation du temps passe
        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();

        coutActuelle = configTestee->erreurs;
        // Enregistrement de la progression du cout dans un fichier
        if(fichier && iteration%5 == 0)
        {
            fichierLocal << iteration << " " << coutActuelle << endl;
            cout << iteration << " " << coutActuelle << endl;
        }
        // A des fins de debug, decommenter pour afficher la progression du cout
        /*if(iteration%100 == 0)
        {
            cout << iteration << " " << coutActuelle << " " << dureeMillisecondes << endl;
        }*/
	}

    // Liberation des ressources
    if(diversification)
    {
        for(int i1=0; i1<N; i1++)
        {
            for(int i2=0; i2<k; i2++)
            {
                delete[] presence[i1][i2];
            }
            delete[] presence[i1];
        }
        delete[] presence;


        for(int i1=0; i1<N; i1++)
        {
            for(int i2=0; i2<k; i2++)
            {
                delete[] frequence[i1][i2];
            }
            delete[] frequence[i1];
        }
        delete[] frequence;


        for(int i1=0; i1<N; i1++)
        {
            for(int i2=0; i2<k; i2++)
            {
                delete[] dernierePresence[i1][i2];
            }
            delete[] dernierePresence[i1];
        }
        delete[] dernierePresence;
    }

    for(int i1=0; i1<N; i1++)
    {
        for(int i2=0; i2<k; i2++)
        {
            delete[] listeTaboue[i1][i2];
        }
        delete[] listeTaboue[i1];
    }
    delete[] listeTaboue;

    // Enregistrement pour les statistiques
    meilleureConfig->nbIt = iteration;
    meilleureConfig->nbMvt = vraisMouvementsTotal;
#endif
	return(meilleureConfig);
}

// Lit le fichier "inputData" pour executer 'nbExec' fois l'algorithme tabou, et relever des
// statistiques dans le fichier "output"
void genererResultats(int nbExec, bool diversification)
{
    ifstream infile("inputData");
    ofstream outfile("output");
    int nbSymboles, nbColonnes, nbLignes, longueurListe;
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
        while (infile >> nbSymboles >> nbColonnes >> nbLignes >> longueurListe)
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
                solution = evolution(nbSymboles, nbColonnes, nbLignes, 50, 200, nullptr);
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
    //genererResultats(10, true);

    // Pour effectuer des tests sur une configuration en particulier, decommenter cette section
    ofstream outfile("output");
    if(outfile.is_open())
    {
        CA_Solution* configEvolution = evolution(3, 20, 25, 50, 200, &outfile);
        cout << "Erreurs : " << configEvolution->erreurs << " iterations : " << configEvolution->nbIt << endl;
        delete configEvolution;
    }
}
