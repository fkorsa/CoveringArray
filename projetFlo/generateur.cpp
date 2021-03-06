#include "generateur.h"

Generateur::Generateur(int nbExec, float dfmax) :
    m_nbExec(nbExec),
    m_dfmax(dfmax)
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

    int nbSymboles, nbColonnes, nbLignes, coutSolution, longueurListe, nbSolutionsValides, numeroExec;
    int *lignes = new int[m_nbExec];
    double coutMoyen, coutMin, coutMax;
    double tempsMoyen, temps;
    double itMoyen, nbIt, ecartType;
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
    case TABOU_DIVERSIFIE:
        infile.open("inputTabou");
        outfile.open("outputTabouDiversifie");
        break;
    case TABOU_MATRICE:
        infile.open("inputTabouMatrice");
        outfile.open("outputTabouMatrice");
        break;
    case TABOU_DIVERSIFIE_MATRICE:
        infile.open("inputTabouMatrice");
        outfile.open("outputTabouDiversifieMatrice");
        break;
    case RECUIT_SIMULE:
        infile.open("inputRecuit");
        outfile.open("outputRecuit");
        break;
    case EVOLUTION:
        infile.open("inputEvolution");
        outfile.open("outputEvolution");
        break;
    }

    if(outfile.is_open())
    {
        outfile << "NombreSymboles NombreColonnes CoutMin CoutMoyen CoutMax TempsMoyen ItMoyen EcartType NbEchantillons" << endl;
        while (infile >> nbSymboles >> nbColonnes >> nbLignes >> m_tempsMax)
        {
            m_tempsMax *= 1000;
            if(type == TABOU || type == TABOU_DIVERSIFIE)
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
            nbSolutionsValides = 0;
            numeroExec = 1;
            // On execute l'algorithme 10 fois
            while(nbSolutionsValides < m_nbExec && numeroExec < 20)
            {
                cout << "Execution numero " << numeroExec++ << " en cours..." << endl;
                switch(type)
                {
                case TABOU:
                    configInit = ConfigurationAleatoire(nbSymboles, nbColonnes, nbLignes);
                    resultatsCourants = TesterTabou(configInit, longueurListe, false);
                    delete configInit;
                    break;
                case TABOU_DIVERSIFIE:
                    configInit = ConfigurationAleatoire(nbSymboles, nbColonnes, nbLignes);
                    resultatsCourants = TesterTabou(configInit, longueurListe, true);
                    delete configInit;
                    break;
                case TABOU_MATRICE:
                    configInit = ConfigurationAleatoire(nbSymboles, nbColonnes, nbLignes);
                    resultatsCourants = TesterTabouMatrice(configInit, false);
                    delete configInit;
                    break;
                case TABOU_DIVERSIFIE_MATRICE:
                    configInit = ConfigurationAleatoire(nbSymboles, nbColonnes, nbLignes);
                    resultatsCourants = TesterTabouMatrice(configInit, true);
                    delete configInit;
                    break;
                default:
                    break;
                }
                if(resultatsCourants.valide)
                {
                    temps = resultatsCourants.temps;
                    coutSolution = resultatsCourants.meilleurCout;
                    nbIt = resultatsCourants.nbIterations;
                    if(nbSolutionsValides == 0)
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
                    lignes[nbSolutionsValides] = coutSolution;
                    nbSolutionsValides++;
                }
            }
            coutMoyen /= nbSolutionsValides;
            itMoyen /= nbSolutionsValides;
            tempsMoyen /= nbSolutionsValides;
            tempsMoyen *= 1/m_dfmax;
            ecartType = 0;
            for(int i=0; i<nbSolutionsValides; i++)
            {
                ecartType += pow(lignes[i]-coutMoyen, 2);
            }
            ecartType = sqrt(ecartType/nbSolutionsValides);
            // On enregistre les donnees dans le fichier output
            outfile << nbSymboles << " "
                    << nbColonnes << " "
                    << coutMin << " "
                    << coutMoyen << " "
                    << coutMax << " "
                    << tempsMoyen << " "
                    << itMoyen << " "
                    << ecartType << " "
                    << numeroExec - 1 << endl;
        }
        outfile.close();
    }
    delete[] lignes;
}

void Generateur::TesterTout()
{
    cout << "Generation de tabou" << endl;
    //GenererResultats(TABOU);
    cout << "Generation de tabou diversifie" << endl;
    //GenererResultats(TABOU_DIVERSIFIE);
    cout << "Generation de tabou (matrice)" << endl;
    GenererResultats(TABOU_MATRICE);
    cout << "Generation de tabou diversifie (matrice)" << endl;
    GenererResultats(TABOU_DIVERSIFIE_MATRICE);
}
