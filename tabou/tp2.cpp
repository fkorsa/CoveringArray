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

/// TODO : ce n'est qu'un stub
void choixDiversification(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration,
                          bool*** presence, int ***dernierePresence, int ***frequence, int coutMeilleure)
{
    bool minDefini, premiereIteration;
    int coutTest, coutMin, frequenceNouveau, frequenceAncien;
    Mouvement mouvementActuel;

    minDefini = false;
    premiereIteration = true;
    mouvementActuel = configTestee->mouvementCourant();
    while(!mouvementActuel.estFinal)
    {
        // La methode mouvement() parcourt tous les mouvements possibles : a chaque appel,
        // elle renvoie le mouvement courant puis prend le mouvement suivant dans l'espace
        // des mouvements possibles.
        if(!premiereIteration)
        {
            mouvementActuel = configTestee->mouvementSuivant();
        }
        premiereIteration = false;
        frequenceNouveau = frequence[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mSymbole];
        if(presence[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mSymbole])
        {
            frequenceNouveau += dernierePresence[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mSymbole];
        }
        frequenceAncien = frequence[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mAncienSymbole];
        if(presence[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mAncienSymbole])
        {
            frequenceAncien += dernierePresence[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mAncienSymbole];
        }
        coutTest = configTestee->verifierSolution(mouvementActuel) + frequenceNouveau - frequenceAncien;
        // Critere tabou
        if(listeTaboue[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mSymbole] < iteration
            // si le mouvement est meilleur que les precedents
            && (!minDefini || coutTest <= coutMin))
        {
            // Si on trouve un mouvement strictement meilleur, on vide la liste des meilleurs mouvements
            if(!minDefini || coutTest < coutMin)
            {
                listeMeilleurs->clear();
            }
            // Enregistrement de cette configuration en tant que meilleure config
            coutMin = coutTest;
            minDefini = true;
            // Ajout de ce mouvement dans la liste des meilleurs
            listeMeilleurs->push_back(mouvementActuel);
        }
    }
}

void choixMouvement(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration, int coutMeilleure)
{
    bool minDefini, premiereIteration;
    int coutTest, coutActuelle, coutMin;
    Mouvement mouvementActuel;

    minDefini = false;
    premiereIteration = true;
    coutActuelle = configTestee->erreurs;
    mouvementActuel = configTestee->mouvementCourant();
    while(!mouvementActuel.estFinal)
    {
        // La methode mouvement() parcourt tous les mouvements possibles : a chaque appel,
        // elle renvoie le mouvement courant puis prend le mouvement suivant dans l'espace
        // des mouvements possibles.
        if(!premiereIteration)
        {
            mouvementActuel = configTestee->mouvementSuivant();
        }
        premiereIteration = false;
        coutTest = configTestee->verifierSolution(mouvementActuel);
        // Critere tabou
        if((listeTaboue[mouvementActuel.mLigne][mouvementActuel.mCol][mouvementActuel.mSymbole] < iteration
            // Critere d'aspiration
            || coutTest < coutMeilleure)
            // si le mouvement est meilleur que les precedents
            && (!minDefini || coutTest <= coutMin))
        {
            // Si on trouve un mouvement strictement meilleur, on vide la liste des meilleurs mouvements
            if(!minDefini || coutTest < coutMin)
            {
                listeMeilleurs->clear();
            }
            // Enregistrement de cette configuration en tant que meilleure config
            coutMin = coutTest;
            minDefini = true;
            // Ajout de ce mouvement dans la liste des meilleurs
            listeMeilleurs->push_back(mouvementActuel);
        }
    }
}

// Algorithme tabou
CA_Solution* tabou(CA_Solution* configInit, ofstream *fichier, int longueurListe, bool diversification)
{
    // Variables pour l'algorithme tabou de base
    Mouvement mv;
    ofstream& fichierLocal = *fichier;
    int iteration = 1;
    int vraisMouvementsTotal = 0, fmin, fmax, coutActuelle;
    int k = configInit->k, v = configInit->v, N = configInit->N;
	CA_Solution* configTestee = configInit; // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configInit); // Meilleures des configurations testées jusqu'alors
    list<Mouvement> listeMeilleurs;

    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now(), dateDebutPhase = dateDebut;
    chrono::duration<double> duree, dureePhase;
    double dureeMillisecondes;
    //const double tempsMax = 60000*5.4/8.6;
    const double tempsMax = 1200000;

    // Variables pour la diversification
    bool ***presence;
    int ***dernierePresence;
    int ***frequence;
    if(diversification)
    {
        presence = new bool**[N];
        for(int i1=0; i1<N; i1++)
        {
            presence[i1] = new bool*[k];
            for(int i2=0; i2<k; i2++)
            {
                presence[i1][i2] = new bool[v];
                for(int i3=0; i3<v; i3++)
                {
                    presence[i1][i2][i3] = (configInit->solution[i1*k+i2] == i3);
                }
            }
        }

        dernierePresence = new int**[N];
        for(int i1=0; i1<N; i1++)
        {
            dernierePresence[i1] = new int*[k];
            for(int i2=0; i2<k; i2++)
            {
                dernierePresence[i1][i2] = new int[v];
                for(int i3=0; i3<v; i3++)
                {
                    dernierePresence[i1][i2][i3] = 0;
                }
            }
        }

        frequence = new int**[N];
        for(int i1=0; i1<N; i1++)
        {
            frequence[i1] = new int*[k];
            for(int i2=0; i2<k; i2++)
            {
                frequence[i1][i2] = new int[v];
                for(int i3=0; i3<v; i3++)
                {
                    frequence[i1][i2][i3] = 0;
                }
            }
        }
    }

    // Initialisation de la liste taboue : grande matrice contenant le numero de l'iteration jusqu'a
    // laquelle l'attribut est tabou
    // Un attribut est de la forme (ligne, colonne, symbole)
    int ***listeTaboue;
    listeTaboue = new int**[N];
    for(int i1=0; i1<N; i1++)
    {
        listeTaboue[i1] = new int*[k];
        for(int i2=0; i2<k; i2++)
        {
            listeTaboue[i1][i2] = new int[v];
            for(int i3=0; i3<v; i3++)
            {
                listeTaboue[i1][i2][i3] = 0;
            }
        }
    }

    int coutMeilleure = configInit->verifierSolution();
    meilleureConfig->erreurs = coutMeilleure;
    fmin = fmax = coutMeilleure;

    duree = chrono::system_clock::now()-dateDebut;
    dureeMillisecondes = 1000*duree.count();

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
            choixDiversification(configTestee, &listeMeilleurs, listeTaboue, iteration, presence, dernierePresence, frequence, coutMeilleure);
            /*if(iteration%100 == 0)
            {
                cout << "Diversification" << endl;
            }*/
        }
        if(dureePhase.count() >= 10)
        {
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

        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();
        // Enregistrement des parametres de la simulation
        coutActuelle = configTestee->erreurs;
        if(fichier && iteration%5 == 0)
        {
            fichierLocal << iteration << " " << coutActuelle << endl;
            cout << iteration << " " << coutActuelle << endl;
        }
        /*if(iteration%100 == 0)
        {
            cout << iteration << " " << coutActuelle << " " << dureeMillisecondes << endl;
        }*/
	}

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


    meilleureConfig->nbIt = iteration;
    meilleureConfig->nbMvt = vraisMouvementsTotal;
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
                CA_Solution* configInit = configurationAleatoire(nbSymboles, nbColonnes, nbLignes);
                start = chrono::system_clock::now();
                solution = tabou(configInit, NULL, longueurListe, diversification);
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

    // Pour effectuer des tests sur une configuration en particulier, decommenter cette section
    CA_Solution* configInit = configurationAleatoire(8, 15, 108);
    ofstream outfile("output");
    if(outfile.is_open())
    {
        CA_Solution* configTabou = tabou(configInit, &outfile, 40, false);
        cout << "Erreurs : " << configTabou->erreurs << " iterations : " << configTabou->nbIt << endl;
        delete configTabou;
    }
    delete configInit;


    // Pour generer toutes les stats sur les differentes configs, decommenter cette section
    //genererResultats(10, true);
}
