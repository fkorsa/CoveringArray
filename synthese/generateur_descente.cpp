#include "generateur.h"


// Choix du meilleur voisin avec liste taboue. On enregistre la liste des meilleurs mouvement, qui seront departages
// aleatoirement par la fonction appelante (qui est toujours la fonction tabou(...))
int Generateur::DescenteChoixMouvement(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs)
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
        if(!minDefini || coutTest <= coutMin)
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
    return coutMin;
}


Resultats Generateur::TesterDescente(int v, int k, int N)
{
    Mouvement mv;
    int iteration = 1, nbLignesCourant = N, dernierNbLignesValide = -1;
    int vraisMouvementsTotal = 0, coutMeilleurVoisin, coutActuelle;
    CA_Solution* configTestee = ConfigurationAleatoire(v, k, N); // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configTestee); // Meilleures des configurations testées jusqu'alors
    list<Mouvement> listeMeilleurs;

    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now(), dateDebutPhase = dateDebut;
    chrono::duration<double> duree, dureePhase;
    double dureeMillisecondes;
    const double tempsMax = m_tempsMax*m_dfmax;

    int coutMeilleure = configTestee->verifierSolution();
    coutActuelle = coutMeilleure;
    meilleureConfig->erreurs = coutMeilleure;

    duree = chrono::system_clock::now()-dateDebut;
    dureeMillisecondes = 1000*duree.count();

    // Critere d'arret : une minute de temps d'execution, ou bien une solution trouvee
    while(dureeMillisecondes < tempsMax)
    {
        configTestee->reinitialiserMouvement();
        listeMeilleurs.clear();

        dureePhase = chrono::system_clock::now()-dateDebutPhase;
        // Parcours de tous les voisins et determination du meilleur, non tabou
        coutMeilleurVoisin = DescenteChoixMouvement(configTestee, &listeMeilleurs);

        double tailleListe = listeMeilleurs.size();

        // Application des conséquences
        if(tailleListe > 0 && coutMeilleurVoisin < coutActuelle)
        {
            mv = listeMeilleurs.front();

            // On departage les mouvements ex aequo aleatoirement, selon une distribution uniforme
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

            configTestee->appliquerMouvement(mv); // Déplacement entériné
            if(configTestee->erreurs < coutMeilleure)
            { // Mise à jour de la meilleure configuration
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
                    coutMeilleure = configTestee->erreurs;
                    //cout << "Meilleure solution trouvee a l'iteration : " << iteration << " de cout : " << coutMeilleure << endl;
                }
            }
            vraisMouvementsTotal++;
        }
        else
        {
            // Relance !
            delete configTestee;
            configTestee = ConfigurationAleatoire(v, k, nbLignesCourant);
            configTestee->verifierSolution();
            //cout << "Relance !" << endl;
        }

        iteration++;


        coutActuelle = configTestee->erreurs;

        // Actualisation du temps passe
        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();
    }
    delete meilleureConfig;
    delete configTestee;
    if(dernierNbLignesValide != -1)
    {
        return Resultats(dureeMillisecondes, dernierNbLignesValide, iteration, true);
    }
    return Resultats(dureeMillisecondes, N, iteration, false);
}
