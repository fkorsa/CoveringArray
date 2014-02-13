#ifndef MOUVEMENT_H
#define MOUVEMENT_H

using namespace std;
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

class Mouvement
{
public:
    Mouvement(int val, int col, int symbole, int ancienSymbole){mLigne=val;mCol=col;mSymbole=symbole;mAncienSymbole=ancienSymbole;}
    Mouvement(){mLigne=0;mCol=0;mSymbole=0;mAncienSymbole=0;}
    //Mouvement(const Mouvement& mv){mLigne=mv.mLigne;mCol=mv.mCol;mSymbole=mv.mSymbole;}

    int mLigne;
    int mCol;
    int mSymbole, mAncienSymbole;
};

#endif //MOUVEMENT_H
