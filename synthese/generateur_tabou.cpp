#include "generateur.h"


// Choix du meilleur voisin, avec diversification et liste taboue : la fonction de cout est perturbee avec les frequences
// des symboles, pour qu'on se dirige vers des regions inexplorees de l'espace de recherche
void Generateur::TabouChoixDiversification(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration,
                          bool*** presence, int ***dernierePresence, int ***frequence, int coutMeilleure)
{
    bool minDefini;
    int coutTest, coutMin, frequenceNouveau, frequenceAncien;
    Mouvement *mouvementActuel;

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
        // Critere tabou
        if(listeTaboue[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mSymbole] < iteration
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
    }
}

// Choix du meilleur voisin avec liste taboue. On enregistre la liste des meilleurs mouvement, qui seront departages
// aleatoirement par la fonction appelante (qui est toujours la fonction tabou(...))
void Generateur::TabouChoixMouvement(CA_Solution* configTestee, list<Mouvement> *listeMeilleurs, int ***listeTaboue, int iteration, int coutMeilleure)
{
    bool minDefini, premiereIteration;
    int coutTest, coutActuelle, coutMin;
    Mouvement *mouvementActuel;

    minDefini = false;
    premiereIteration = true;
    coutActuelle = configTestee->erreurs;
    //mouvementActuel = configTestee->mouvementCourant();
    mouvementActuel = configTestee->mouvementCritique();
    while(!mouvementActuel->estFinal)
    {
        coutTest = configTestee->verifierSolution(*mouvementActuel);
        // Critere tabou
        if((listeTaboue[mouvementActuel->mLigne][mouvementActuel->mCol][mouvementActuel->mSymbole] < iteration
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
        // La methode mouvementCritiqueSuivant() parcourt le voisinage critique : a chaque appel,
        // elle renvoie le mouvement courant puis prend le mouvement suivant dans l'espace
        // des mouvements critiques.
        configTestee->mouvementCritiqueSuivant();
    }
}


Resultats Generateur::TesterTabou(CA_Solution* configInit, int longueurListe, bool diversification)
{
    Mouvement mv;
    int iteration = 1, nbLignesCourant = configInit->N, dernierNbLignesValide = -1;
    int vraisMouvementsTotal = 0, fmin, fmax, coutActuelle;
    int k = configInit->k, v = configInit->v, N = configInit->N;
    CA_Solution* configTestee = configInit; // Configuration S' suite à un mouvement
    CA_Solution* meilleureConfig = new CA_Solution(*configInit); // Meilleures des configurations testées jusqu'alors
    list<Mouvement> listeMeilleurs;

    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now(), dateDebutPhase = dateDebut;
    chrono::duration<double> duree, dureePhase;
    double dureeMillisecondes;
    const double tempsMax = m_tempsMax*m_dfmax;

    // Variables pour la diversification
    bool ***presence;
    int ***dernierePresence;
    int ***frequence;

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
            TabouChoixMouvement(configTestee, &listeMeilleurs, listeTaboue, iteration, coutMeilleure);
        }
        else if(diversification && dureePhase.count() >= 9)
        {
            // Parcours des voisins, avec une fonction de cout prenant en compte la frequence d'apparition des symboles
            TabouChoixDiversification(configTestee, &listeMeilleurs, listeTaboue, iteration, presence, dernierePresence, frequence, coutMeilleure);
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
            {
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

        iteration++;

        // Actualisation du temps passe
        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();

        coutActuelle = configTestee->erreurs;
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
    delete meilleureConfig;
    if(dernierNbLignesValide != -1)
    {
        return Resultats(dureeMillisecondes, dernierNbLignesValide, iteration, true);
    }
    return Resultats(dureeMillisecondes, N, iteration, false);
}
