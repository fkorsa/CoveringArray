#include "generateur.h"

Resultats Generateur::TesterRecuitSimule(CA_Solution* configInit, double tempInit, double coeff)
{
    Mouvement mouvementActuel;
    double T = tempInit;
    int itCpt = 0, statiqueCpt = 0, totalIt = 0, nbLignesCourant = configInit->N, dernierNbLignesValide = -1;
    int vraisMouvements = 0, vraisMouvementsTotal = 0, fmin, fmax, coutTest, coutActuelle;
    CA_Solution* configTestee = configInit; // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configInit); // Meilleures des configurations testées jusqu'alors

    int coutMeilleure = configInit->verifierSolution();
    meilleureConfig->erreurs = coutMeilleure;
    fmin = fmax = coutMeilleure;


    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now();
    chrono::duration<double> duree;
    double dureeMillisecondes;
    const double tempsMax = m_tempsMax*m_dfmax;

    duree = chrono::system_clock::now()-dateDebut;
    dureeMillisecondes = 1000*duree.count();

    // Critere d'arret : une minute de temps d'execution, ou bien une solution trouvee
    while(dureeMillisecondes < tempsMax)
    {
        // On génère un voisin aléatoire de S
        mouvementActuel = configTestee->mouvementAleatoire();
        coutActuelle = configTestee->erreurs;
        coutTest = configTestee->verifierSolution(mouvementActuel);

        double delta = coutTest - coutActuelle;

        // On teste le critère de Métropolis
        bool metropolis;
        if(delta <= 0)
        {
            metropolis = true;
        }
        // si T=0, on ne prend pas de mouvement qui degrade la solution
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
                if(configTestee->erreurs == 0)
                {
                    meilleureConfig->enleverLigne();
                    delete configTestee;
                    meilleureConfig->verifierSolution();
                    configTestee = new CA_Solution(*meilleureConfig);
                    nbLignesCourant--;
                    coutMeilleure = configTestee->erreurs;
                    //cout << "Reduction de lignes" << endl;
                    dernierNbLignesValide = nbLignesCourant + 1;
                }
                else
                {
                    delete meilleureConfig;
                    meilleureConfig = new CA_Solution(*configTestee);
                    coutMeilleure = coutTest;
                    //cout << "Meilleure solution trouvee a l'iteration : " << iteration << " de cout : " << coutMeilleure << endl;
                }
                statiqueCpt = 0;
                itCpt--;
            }
            vraisMouvements++;
            vraisMouvementsTotal++;
        }
        statiqueCpt++;

        // Refroidissement par paliers
        if(itCpt > 1000)
        {
            itCpt = 0;
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

        // Actualisation du temps passe
        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();
    }
    delete meilleureConfig;
    if(dernierNbLignesValide != -1)
    {
        return Resultats(dureeMillisecondes, dernierNbLignesValide, totalIt, true);
    }
    return Resultats(dureeMillisecondes, nbLignesCourant, totalIt, false);
}
