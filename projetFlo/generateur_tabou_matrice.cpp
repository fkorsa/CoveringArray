#include "generateur.h"


// Choix du meilleur voisin, avec diversification et liste taboue : la fonction de cout est perturbee avec les frequences
// des symboles, pour qu'on se dirige vers des regions inexplorees de l'espace de recherche
void Generateur::TabouChoixDiversificationMatrice(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, set<CA_Solution> *listeTaboue,
                          bool*** presence, int ***dernierePresence, int ***frequence)
{
    bool minDefini;
    int coutTest, coutMin = INT_MAX, frequenceNouveau, frequenceAncien;
    Mouvement *mouvementActuel;
    CA_Solution copieConfig = *configTestee;
    set<CA_Solution>::iterator itConfig;

    minDefini = false;
    //mouvementActuel = configTestee->mouvementCourant();
    mouvementActuel = configTestee->mouvementCritique();
    while(!mouvementActuel->estFinal)
    {
        frequenceNouveau = frequence[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mSymbole];
        // Ajustement de la frequence du nouveau symbole pour tenir compte de la presence eventuelle du symbole
        // jusqu'ici
        if(presence[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mSymbole])
        {
            frequenceNouveau += dernierePresence[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mSymbole];
        }
        frequenceAncien = frequence[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mAncienSymbole];
        // Ajustement de la frequence de l'ancien symbole pour tenir compte de sa presence eventuelle
        // jusqu'ici
        if(presence[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mAncienSymbole])
        {
            frequenceAncien += dernierePresence[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mAncienSymbole];
        }
        coutTest = configTestee->verifierSolution(*mouvementActuel) + frequenceNouveau - frequenceAncien;
        copieConfig.solution[copieConfig.k*mouvementActuel->mLigne+mouvementActuel->mCol] = mouvementActuel->mSymbole;
        itConfig = listeTaboue->find(copieConfig);
        // Critere tabou
        if(itConfig == listeTaboue->end()
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
            listeMeilleurs->push_back(*mouvementActuel);
        }
        // La methode mouvement() parcourt tous les mouvements possibles : a chaque appel,
        // elle renvoie le mouvement courant puis prend le mouvement suivant dans l'espace
        // des mouvements possibles.
        configTestee->mouvementCritiqueSuivant();
        //configTestee->mouvementSuivant();
        copieConfig.solution[copieConfig.k*mouvementActuel->mLigne+mouvementActuel->mCol] = mouvementActuel->mAncienSymbole;
    }
}

// Choix du meilleur voisin avec liste taboue. On enregistre la liste des meilleurs mouvement, qui seront departages
// aleatoirement par la fonction appelante (qui est toujours la fonction tabou(...))
void Generateur::TabouChoixMouvementMatrice(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, set<CA_Solution> *listeTaboue,
                                            int coutMeilleure)
{
    bool minDefini;
    int coutTest, coutMin = INT_MAX;
    Mouvement *mouvementActuel;
    CA_Solution copieConfig = *configTestee;
    set<CA_Solution>::iterator itConfig;

    minDefini = false;
    mouvementActuel = configTestee->mouvementCourant();
    //mouvementActuel = configTestee->mouvementCritique();
    while(!mouvementActuel->estFinal)
    {
        coutTest = configTestee->verifierSolution(*mouvementActuel);
        copieConfig.solution[copieConfig.k*mouvementActuel->mLigne+mouvementActuel->mCol] = mouvementActuel->mSymbole;
        itConfig = listeTaboue->find(copieConfig);
        // Critere tabou
        if((itConfig == listeTaboue->end()
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
            listeMeilleurs->push_back(*mouvementActuel);
        }
        copieConfig.solution[copieConfig.k*mouvementActuel->mLigne+mouvementActuel->mCol] = mouvementActuel->mAncienSymbole;
        // La methode mouvementCritiqueSuivant() parcourt le voisinage critique : a chaque appel,
        // elle renvoie le mouvement courant puis prend le mouvement suivant dans l'espace
        // des mouvements critiques.
        //configTestee->mouvementSuivant();
        configTestee->mouvementCritiqueSuivant();
    }
}

Resultats Generateur::TesterTabouMatrice(CA_Solution* configInit, bool diversification)
{
    Mouvement mv;
    int iteration = 1, nbLignesCourant = configInit->N, dernierNbLignesValide = -1;
    int vraisMouvementsTotal = 0;
    int k = configInit->k, v = configInit->v, N = configInit->N;
    CA_Solution* configTestee = new CA_Solution(*configInit); // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configInit); // Meilleures des configurations testées jusqu'alors
    configTestee->allouerMemoire();
    meilleureConfig->allouerMemoire();
    list<Mouvement> listeMeilleurs;

    // Variables pour la prise en compte du temps d'execution
    Date dateDebut = chrono::system_clock::now(), dateDebutPhase = dateDebut, dateNow;
    list<Date> datePasse;
    datePasse.push_back(dateDebut);
    chrono::duration<double> duree, dureePhase;
    double dureeMillisecondes;
    const double tempsMax = m_tempsMax*m_dfmax;

    // Initialisation de la liste taboue : table de hachage contenant les matrices entieres associees
    // au numero d'iteration jusqu'a laquelle elles sont taboues
    set<CA_Solution> listeTaboue;

    // Variables pour la diversification
    bool ***presence = nullptr;
    int ***dernierePresence = nullptr;
    int ***frequence = nullptr;

    // Allocation de la memoire pour les memoires a long terme : presence, frequence et derniere presence des symboles
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

        dernierePresence = Allocate3DArray(N, k, v, 0);
        frequence = Allocate3DArray(N, k, v, 0);
    }

    int coutMeilleure = configTestee->verifierSolution();
    meilleureConfig->erreurs = coutMeilleure;

    duree = chrono::system_clock::now()-dateDebut;
    dureeMillisecondes = 1000*duree.count();

    // Critere d'arret : une minute de temps d'execution, ou bien une solution trouvee
    while(dureeMillisecondes < tempsMax)
    {
        //configTestee->reinitialiserMouvement();
        configTestee->reinitialiserMouvementCritique();
        listeMeilleurs.clear();

        dureePhase = chrono::system_clock::now()-dateDebutPhase;
        if(!diversification || dureePhase.count() < 9)
        {
            // Parcours de tous les voisins et determination du meilleur, non tabou
            TabouChoixMouvementMatrice(configTestee, &listeMeilleurs, &listeTaboue, coutMeilleure);
        }
        else if(diversification && dureePhase.count() >= 9)
        {
            // Parcours des voisins, avec une fonction de cout prenant en compte la frequence d'apparition des symboles
            TabouChoixDiversificationMatrice(configTestee, &listeMeilleurs, &listeTaboue, presence, dernierePresence, frequence);
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
            // La nouvelle matrice est maintenant taboue
            listeTaboue.insert(*configTestee);
            // Actualisation des memoires a long terme
            if(diversification)
            {
                presence[mv.mLigne][mv.mCol][mv.mAncienSymbole] = false;
                presence[mv.mLigne][mv.mCol][mv.mSymbole] = true;
                frequence[mv.mLigne][mv.mCol][mv.mAncienSymbole] += iteration - dernierePresence[mv.mLigne][mv.mCol][mv.mAncienSymbole];
                dernierePresence[mv.mLigne][mv.mCol][mv.mAncienSymbole] = iteration;
            }
            if(configTestee->erreurs < coutMeilleure)
            {
                if(configTestee->erreurs == 0)
                {
                    configTestee->enleverLigne();
                    configTestee->verifierSolution();
                    nbLignesCourant--;
                    meilleureConfig->solution = configTestee->solution;
                    meilleureConfig->erreurs = configTestee->erreurs;
                    coutMeilleure = configTestee->erreurs;
                    cout << "Reduction de lignes : " << nbLignesCourant << endl;
                    dernierNbLignesValide = nbLignesCourant + 1;
                    listeTaboue.clear();
                }
                else
                {
                    meilleureConfig->solution = configTestee->solution;
                    meilleureConfig->erreurs = configTestee->erreurs;
                    coutMeilleure = configTestee->erreurs;
                    //cout << "Meilleure solution trouvee a l'iteration : " << iteration << " de cout : " << coutMeilleure << endl;
                }
            }
            vraisMouvementsTotal++;
        }

        iteration++;

        // Actualisation du temps passe
        dateNow = chrono::system_clock::now();
        duree = dateNow-dateDebut;
        dureeMillisecondes = 1000*duree.count();
        datePasse.push_back(dateNow);
        if(datePasse.size() > 1000)
        {
            duree = dateNow - datePasse.front();
            dureeMillisecondes = 1000*duree.count();
            cout << "It/msec : " << 1000/dureeMillisecondes << endl;
            cout << "taille tabou" << listeTaboue.size() << endl;
            datePasse.pop_front();
        }
    }

    // Liberation des ressources
    if(diversification)
    {
        Delete3DArray(presence, N, k);
        Delete3DArray(frequence, N, k);
        Delete3DArray(dernierePresence, N, k);
    }

    delete meilleureConfig;
    delete configTestee;
    if(dernierNbLignesValide != -1)
    {
        return Resultats(dureeMillisecondes, dernierNbLignesValide, iteration, true);
    }
    return Resultats(dureeMillisecondes, N, iteration, false);
}
