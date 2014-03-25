#include "generateur.h"

Generateur::Generateur(int tempsMax, int nbExec) :
    m_tempsMax(tempsMax),
    m_nbExec(nbExec)
{
    srand(time(NULL));
}

// Generer une matrice aleatoirement
CA_Solution* Generateur::ConfigurationAleatoire(int v, int k, int N)
{
    vector<int> matrice(N*k);

    for(int i=0; i<N*k; i++)
    {
        int symb = rand()%v;
        matrice[i] = symb;
    }

    CA_Solution* configuration = new CA_Solution(v, k, matrice);
    return configuration;
}

void Generateur::GenererResultats(ALGO_TYPE type)
{
    ifstream infile;
    ofstream outfile;

    int nbSymboles, nbColonnes, nbLignes, coutSolution, longueurListe;
    double coutMoyen, coutMin, coutMax;
    double tempsMoyen, temps;
    double itMoyen, nbIt;
    Resultats resultatsCourants(0, 0, 0);
    CA_Solution* configInit;

    switch(type)
    {
    case GLOUTON:
        infile.open("inputGlouton");
        outfile.open("outputGlouton");
        break;
    case DESCENTE:
        infile.open("inputDescente");
        outfile.open("outputDescente");
        break;
    case TABOU:
        infile.open("inputTabou");
        outfile.open("outputTabou");
        break;
    case RECUIT_SIMULE:
        infile.open("inputRecuit");
        outfile.open("outputRecuit");
        break;
    }

    if(outfile.is_open())
    {
        outfile << "NombreSymboles NombreColonnes NombreLignes CoutMin CoutMoyen CoutMax TempsMoyen ItMoyen" << endl;
        while (infile >> nbSymboles >> nbColonnes >> nbLignes)
        {
            if(type == TABOU)
            {
                infile >> longueurListe;
            }
            coutMin = 0;
            coutMoyen = 0;
            coutMax = 0;
            tempsMoyen = 0;
            itMoyen = 0;
            cout << nbSymboles << " "
                    << nbColonnes << " "
                    << nbLignes << " " << endl;
            // On execute l'algorithme 10 fois
            for(int i = 0; i < m_nbExec; i++)
            {
                switch(type)
                {
                case GLOUTON:
                    TesterGlouton();
                    break;
                case DESCENTE:
                    TesterDescente();
                    break;
                case TABOU:
                    configInit = ConfigurationAleatoire(nbSymboles, nbColonnes, nbLignes);
                    resultatsCourants = TesterTabou(configInit, longueurListe, true);
                    delete configInit;
                    break;
                case RECUIT_SIMULE:
                    configInit = ConfigurationAleatoire(nbSymboles, nbColonnes, nbLignes);
                    resultatsCourants = TesterRecuitSimule(configInit, 0.1, 0.5);
                    delete configInit;
                    break;
                case EVOLUTION:
                    resultatsCourants = TesterEvolution(nbSymboles, nbColonnes, nbLignes, 20, 20, 0.0001, CROISEMENT_LIGNE);
                    break;
                }
                temps = resultatsCourants.temps;
                coutSolution = resultatsCourants.meilleurCout;
                nbIt = resultatsCourants.nbIterations;
                if(i == 0)
                {
                    coutMin = coutSolution;
                }
                if(coutMin > coutSolution)
                {
                    coutMin = coutSolution;
                }
                if(coutMax < coutSolution)
                {
                    coutMax = coutSolution;
                }
                coutMoyen += coutSolution;
                tempsMoyen += temps;
                itMoyen += nbIt;
            }
            coutMoyen /= m_nbExec;
            itMoyen /= m_nbExec;
            tempsMoyen /= m_nbExec;
            tempsMoyen *= (8.6/5.4);
            // On enregistre les donnees dans le fichier output
            outfile << nbSymboles << " "
                    << nbColonnes << " "
                    << nbLignes << " "
                    << coutMin << " "
                    << coutMoyen << " "
                    << coutMax << " "
                    << tempsMoyen << " "
                    << itMoyen << endl;
        }
        outfile.close();
    }
}


void Generateur::TesterTout()
{
    cout << "Generation de glouton" << endl;
    GenererResultats(GLOUTON);
    cout << "Generation de descente" << endl;
    GenererResultats(DESCENTE);
    cout << "Generation de tabou" << endl;
    GenererResultats(TABOU);
    cout << "Generation de recuit" << endl;
    GenererResultats(RECUIT_SIMULE);
    cout << "Generation de evolution" << endl;
    GenererResultats(EVOLUTION);
}
