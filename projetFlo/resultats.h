#ifndef RESULTATS_H
#define RESULTATS_H

class Resultats
{
public:
    Resultats(double temps, int meilleurCout, int nbIterations, bool valide = true);
    double temps;
    int meilleurCout, nbIterations;
    bool valide;
};

#endif // RESULTATS_H
