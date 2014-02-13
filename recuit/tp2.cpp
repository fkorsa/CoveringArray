#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <math.h>
#include <chrono>

using namespace std;

#include "CA_Solution.h"
#include "Mouvement.h"


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


CA_Solution* recuitSimule(CA_Solution* configInit, double tempInit, double coeff, ofstream* fichier, int nombreEssais) {
    Mouvement mouvementActuel;
    ofstream& fichierLocal = *fichier;
    double T = tempInit;
    int itCpt = 0, statiqueCpt = 0, totalIt = 0;
    int vraisMouvements = 0, vraisMouvementsTotal = 0, fmin, fmax, coutTest, coutActuelle;
	CA_Solution* configTestee = configInit; // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configInit); // Meilleures des configurations testées jusqu'alors

    int coutMeilleure = configInit->verifierSolution();
    meilleureConfig->erreurs = coutMeilleure;
    fmin = fmax = coutMeilleure;

    while(statiqueCpt < nombreEssais && (coutMeilleure > 0 || fichier))
    {
		// On génère un voisin aléatoire de S
        mouvementActuel = configTestee->mouvement();
        coutActuelle = configTestee->erreurs;
        coutTest = configTestee->verifierSolution(mouvementActuel);

        double delta = coutTest - coutActuelle;

		// On teste le critère de Métropolis
		bool metropolis;
        if(delta <= 0)
        {
			metropolis = true;
        }
        else if(T != 0)
        {
            double prob;
            prob = exp(-delta/T);
            double numTire = (double)rand()/RAND_MAX;
            metropolis = (numTire <= prob);
		}

		// Application des conséquences
        itCpt++;
        if(metropolis)
        {
            configTestee->appliquerMouvement(mouvementActuel); // Déplacement entériné
            if(coutTest < coutMeilleure)
            { // Mise à jour de la meilleure configuration (pas forcément S' si on a tiré au sort en faveur de la dégradation)
                delete meilleureConfig;
                meilleureConfig = new CA_Solution(*configTestee);
				coutMeilleure = coutTest;
                statiqueCpt = 0;
                itCpt--;
            }
            vraisMouvements++;
            vraisMouvementsTotal++;
        }
        statiqueCpt++;

        if(itCpt > 1000)
        {
            itCpt = 0;
            // Refroidissement (à élaborer encore plus)
            T*=coeff;
        }
        totalIt++;
        if(coutTest < fmin)
        {
            fmin = coutTest;
        }
        if(coutTest > fmax)
        {
            fmax = coutTest;
        }
        if(fichier && totalIt%50 == 0)
        {
            fichierLocal << totalIt << " " << vraisMouvements << " " << fmin << " " << fmax << " " << (float)vraisMouvements/50.0f << endl;
            fmin = fmax = coutTest;
            vraisMouvements = 0;
        }
	}
    meilleureConfig->nbIt = totalIt;
    meilleureConfig->nbMvt = vraisMouvementsTotal;
	return(meilleureConfig);
}

void testerParametres(int seed)
{
    double temp[] = {1.6, 0.8, 0.4, 0.2, 0.1, 0.05, 0.025};
    ofstream fichier("resultsParam");
    CA_Solution *ancienneConfig, *configRecuit;
    if(fichier)
    {
        for(int indexTemp = 0; indexTemp<sizeof(temp)/sizeof(double); indexTemp++)
        {
            fichier << "temp seed " << temp[indexTemp] << " " << seed << endl;
            fichier << "trial mov fmin fmax acc" << endl;
            ancienneConfig = configurationAleatoire(3, 20, 23);
            configRecuit = recuitSimule(ancienneConfig, temp[indexTemp], 1, &fichier, 10000);
            delete ancienneConfig;
        }
        fichier.close();
    }
    else {
        cerr << "Impossible d'ouvrir le fichier !" << endl; }
    delete configRecuit;
}

int trouverMeilleure(int nbSymboles, int nbColonnes, int nbLignesDebut, double tempInit, double alpha)
{
    int nbLignes;
    CA_Solution* ancienneConfig = configurationAleatoire(nbSymboles, nbColonnes, nbLignesDebut);
    CA_Solution* bestConfig = new CA_Solution(*ancienneConfig);
    CA_Solution* configRecuit = recuitSimule(ancienneConfig, tempInit, alpha, NULL, 10000000);
    delete ancienneConfig;
    while(configRecuit->erreurs == 0)
    {
        delete bestConfig;
        bestConfig = new CA_Solution(*configRecuit);
        configRecuit->enleverLigne();
        ancienneConfig = configRecuit;
        configRecuit = recuitSimule(configRecuit, tempInit, alpha, NULL, 10000000);
        cout << "Nombre d'iterations pour cette simulation : " << configRecuit->nbIt << endl;
        delete ancienneConfig;
    }
    cout << "Nombre de lignes : " << bestConfig->N << " Nombre d'erreurs : " << bestConfig->erreurs << endl;
    nbLignes = bestConfig->N;
    delete bestConfig;
    delete configRecuit;
    return nbLignes;
}

CA_Solution* executerRecuit(int nbSymboles, int nbColonnes, int nbLignes, double tempInit, double alpha)
{
    CA_Solution* ancienneConfig = configurationAleatoire(nbSymboles, nbColonnes, nbLignes);
    CA_Solution* configRecuit = recuitSimule(ancienneConfig, tempInit, alpha, NULL, 10000000);
    delete ancienneConfig;
    return configRecuit;
}

void genererResultats()
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
            for(int i = 0; i < 10; i++)
            {
                start = chrono::system_clock::now();
                solution = executerRecuit(nbSymboles, nbColonnes, nbLignes, 0.0, 0.5);
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
            coutMoyen /= 10;
            itMoyen /= 10;
            mvMoyen /= 10;
            tempsMoyen /= 10;
            tempsMoyen *= (8.6/5.4) ;
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

void rechercherOptimum()
{
    bool trouve = false;
    int coutRecuit, coutDescente;
    int col = 16;
    CA_Solution *configRecuit, *ancienneConfig;
    while(!trouve)
    {
        ancienneConfig = configurationAleatoire(3, col, 15);
        configRecuit = recuitSimule(ancienneConfig, 0.8, 0.999, NULL, 10000000);
        coutRecuit = configRecuit->erreurs;
        configRecuit->ecrireFichier("outputRecuit");
        delete configRecuit;
        configRecuit = recuitSimule(ancienneConfig, 0, 0.5, NULL, 10000000);
        configRecuit->ecrireFichier("outputDescente");
        coutDescente = configRecuit->erreurs;
        delete configRecuit;
        cout << "nbcolonnes : " << col << endl;
        cout << "recuit : " << coutRecuit << " descente : " << coutDescente << endl;
        if(coutRecuit < coutDescente)
        {
            trouve = true;
        }
        else
        {
            col++;
        }
    }
    delete ancienneConfig;
}

int main()
{
    int seed = time(NULL);
    srand(seed);
    //testerParametres(seed);
    genererResultats();
    //cout << "Meilleure : " << trouverMeilleure(8, 15, 116, 0.1, 0.5) << endl;

}
