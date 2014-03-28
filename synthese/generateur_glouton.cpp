#include "generateur.h"


vector<int> Generateur::couvertureGlouton(int v, int k, float tolerance) // Glouton si tolerance = 0, complètement aléatoire si tolerance = 1
{
    // Nombre de contraintes à satisfaire
    int nbContraintes((k*(k-1)*v*v)/2);

    bool**** contraintes; // contraintes[k][k][v][v]
    // Génère toutes les contraintes élémentaires
    contraintes = (bool****) malloc(k*sizeof(bool***));
    for(int i1=0; i1<k; i1++) {
        contraintes[i1] = (bool***) malloc(k*sizeof(bool**));
        for(int i2=0; i2<k; i2++) {
            contraintes[i1][i2] = (bool**) malloc(v*sizeof(bool*));
            for(int i3=0; i3<v; i3++) {
                contraintes[i1][i2][i3] = (bool*) malloc(v*sizeof(bool));
                for(int i4=0; i4<v; i4++) {
                    contraintes[i1][i2][i3][i4] = false;
                }
            }
        }
    }

    // Algorithme glouton (avec part d'aléatoire si tolerance != 0)
    vector<int> resultat(0);
    int indexLignes = -1; // Nombre de lignes du tableau (indice de la ligne sur laquelle on est en train d'écrire)
    int contraintesSatisfaites = 0;

    while (contraintesSatisfaites < nbContraintes) {
        resultat.insert(resultat.end(),k,-1); // Ajout d'une ligne au tableau de résultat
        indexLignes++;

        // Remplissage de la ligne
        for(int colonne = 0; colonne<k ; colonne++) {

            if (colonne == 0) { // Remplissage du début de la ligne

                int symbMax = -1; // Symbole qui maximise le critère glouton
                int compMax = -1; // Compteur de contraintes qu'il peut satisfaire associé
                int* scores = new int[v]; // Score de résolution de contraintes pour chaque symbole possible

                for(int symb=0; symb<v; symb++) { // Test de chaque symbole possible
                    int symbComp = 0; // Compteur pour le symbole testé

                    for(int k2=1; k2<k; k2++) {
                        for(int v2=0; v2<v; v2++) {
                            if (!contraintes[0][k2][symb][v2]) {
                                symbComp++;
                            }
                        }
                    } // Fin boucle k2

                    scores[symb] = symbComp;

                    if (symbComp > compMax) {
                        symbMax = symb;
                        compMax = symbComp;
                    }

                } // Fin test symbole

                // On connaît le nombre de contraintes que peut résoudre le meilleur symbole glouton, on introduit alors un seuil de contraintes inférieur en fonction du paramètre de tolérance.
                // Les symboles résolvant au moins ce nombre de contraintes seront des candidats au tirage au sort pour remplir la colonne.

                int seuil = floor((1-tolerance)*compMax);
                int nbCandidats = 0;
                vector<int> candidats(0);

                for(int j=0; j<v; j++) { // On regarde si chaque symbole peut être candidat
                    if(scores[j] >= seuil) {
                        candidats.insert(candidats.end(),1,j); // Ajout d'un candidat
                        nbCandidats++;
                    }
                }

                // Tirage au sort du candidat
                int candidatElu = rand()%nbCandidats;
                int symbElu;

                if (tolerance==0) {
                    symbElu = symbMax; // On ne prend pas en compte le tirage au sort
                } else {
                    symbElu = candidats[candidatElu];
                }
                resultat[indexLignes*k+colonne] = symbElu; // Insertion du symbole élu

            } else { // Remplissage des autres colonnes de la ligne

                int premierPassage = 1; // A voir comme un booléen, on n'a pas encore effectué de second passage (voir suite)
                int symbMax = -1; // Symbole qui maximise le critère glouton
                int compMax = -1; // Compteur de contraintes qu'il peut satisfaire associé
                int* scores = new int[v]; // Score de résolution de contraintes pour chaque symbole possible

                for(int symb=0; symb<v; symb++) { // Test de chaque symbole possible
                    int symbComp = 0; // Compteur pour le symbole testé en cours

                    // Test avec les éléments des colonnes précédentes
                    for(int k1=0; k1<colonne; k1++) {
                        int v1 = resultat[indexLignes*k+k1];
                        if (!contraintes[k1][colonne][v1][symb]) {
                            symbComp++;
                        }
                    } // Fin boucle k1

                    scores[symb] = symbComp;

                    if (symbComp > compMax) {
                        symbMax = symb;
                        compMax = symbComp;
                    }

                } // Fin test symbole

                if (compMax == 0) {
                    // Si jamais toutes les contraintes par rapport aux colonnes précédentes sont déjà satisfaites, il faut procéder comme lors du choix pour la première colonne et
                    // choisir le symbole à rentrer en fonction des contraintes l'impliquant avec les colonnes suivantes. Il faut donc recommencer le choix dans cette optique.

                    premierPassage = 0; // On a effectué un second passage

                    symbMax = -1; // Symbole qui maximise le critère glouton
                    compMax = -1; // Compteur de contraintes qu'il peut satisfaire associé
                    scores = new int[v]; // Score de résolution de contraintes pour chaque symbole possible

                    for(int symb=0; symb<v; symb++) { // Test de chaque symbole
                        int symbComp = 0; // Compteur pour le symbole en cours

                        // Test avec les éléments des colonnes suivantes si jamais toutes les contraintes avec les colonnes précédentes sont réglées
                        for(int k2=colonne+1; k2<k; k2++) {
                            for(int v2=0; v2<v; v2++) {
                                if (!contraintes[colonne][k2][symb][v2]) {
                                    symbComp++;
                                }
                            }
                        } // Fin boucle k2

                        scores[symb] = symbComp;

                        if (symbComp > compMax) {
                            symbMax = symb;
                            compMax = symbComp;
                        }
                    } // Fin test symbole
                } // Fin du deuxième passage

                // On connaît le nombre de contraintes que peut résoudre le meilleur symbole glouton, on introduit alors un seuil de contraintes inférieur en fonction du paramètre de tolérance.
                // Les symboles résolvant au moins ce nombre de contraintes seront des candidats au tirage au sort pour remplir la colonne.
                int seuil = floor((1-tolerance)*compMax);
                int nbCandidats = 0;
                vector<int> candidats(0);

                for(int j=0; j<v; j++) { // On regarde si chaque symbole peut être candidat
                    if(scores[j] >= seuil) {
                        candidats.insert(candidats.end(),1,j); // Ajout d'un candidat
                        nbCandidats++;
                    }
                }

                // Tirage au sort du candidat
                int candidatElu = rand()%nbCandidats;
                int symbElu;

                if (tolerance==0) {
                    symbElu = symbMax; // On ne prend pas en compte le tirage au sort
                } else {
                    symbElu = candidats[candidatElu];
                }

                resultat[indexLignes*k+colonne] = symbElu; // Insertion du symbole élu

                for(int k1=0; k1<colonne; k1++) { // Résolution des contraintes avec éléments de toutes les autres colonnes (inférieures à colonne)
                    int symbAss = resultat[indexLignes*k+k1];
                    contraintes[k1][colonne][symbAss][symbElu] = true;
                }

                // Mise à jour du nombre de contraintes satisfaites si on a pas effectué de second passage (qui ne résoud en fait aucune contrainte)
                contraintesSatisfaites+=scores[symbElu]*premierPassage;

            } // Fin IF colonne ELSE
        } // Fin remplissage d'une colonne

        // cout << "Ligne " << indexLignes << " ecrite, " << contraintesSatisfaites << " contraintes satisfaites au total, reste " << nbContraintes-contraintesSatisfaites << endl;
    } // Fin WHILE
    return resultat;
} // Fin fonction couvertureGlouton


Resultats Generateur::TesterGlouton(int v, int k)
{
    int minN = 0;
    int iteration = 0;

    // Variables pour la prise en compte du temps d'execution
    chrono::time_point<chrono::system_clock> dateDebut = chrono::system_clock::now();
    chrono::duration<double> duree;
    double dureeMillisecondes;
    const double tempsMax = m_tempsMax*5.4/8.6;

    while(dureeMillisecondes < tempsMax)
    {
        int taille = couvertureGlouton(v,k,0.2).size()/k;
        if(minN == 0 || minN > taille)
        {
            minN = taille;
        }
        iteration++;
        // Actualisation du temps passe
        duree = chrono::system_clock::now()-dateDebut;
        dureeMillisecondes = 1000*duree.count();
    }
    return Resultats(dureeMillisecondes, minN, iteration);
}
