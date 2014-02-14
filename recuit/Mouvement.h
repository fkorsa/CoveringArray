#ifndef MOUVEMENT_H
#define MOUVEMENT_H

using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

// Classe represantant un mouvement dans notre voisinage
class Mouvement
{
public:
    Mouvement(int ligne, int col, int symbole, int ancienSymbole){mLigne=ligne;mCol=col;mSymbole=symbole;mAncienSymbole=ancienSymbole;}
    Mouvement(){mLigne=0;mCol=0;mSymbole=0;mAncienSymbole=0;}

    int mLigne;
    int mCol;
    int mSymbole, mAncienSymbole;
};

#endif //MOUVEMENT_H
