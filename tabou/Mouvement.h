#ifndef MOUVEMENT_H
#define MOUVEMENT_H

// Classe represantant un mouvement dans notre voisinage
class Mouvement
{
public:
    Mouvement(int ligne, int col, int symbole, int ancienSymbole)
    {
        mLigne=ligne;mCol=col;mSymbole=symbole;mAncienSymbole=ancienSymbole;
        estFinal = false;
    }
    Mouvement(){mLigne=0;mCol=0;mSymbole=0;mAncienSymbole=0;estFinal=false;}
    void incrementer(int k, int v, int N)
    {
        if(mSymbole < v - 1)
        {
            mSymbole++;
        }
        else if(mCol < k - 1)
        {
            mCol++;
            mSymbole = 0;
        }
        else if(mLigne < N - 1)
        {
            mLigne++;
            mSymbole = 0;
            mCol = 0;
        }
        else
        {
            mLigne = 0;
            mSymbole = 0;
            mCol = 0;
        }
        if(mLigne == N - 1 && mSymbole == v - 1 && mCol == k - 1)
        {
            estFinal = true;
        }
        else
        {
            estFinal = false;
        }
    }

    int mLigne;
    int mCol;
    int mSymbole, mAncienSymbole;
    bool estFinal;
};

#endif //MOUVEMENT_H
