#include <iostream>

using namespace std;

#include "generateur.h"

int main()
{
    Generateur gen(10, 5.4/8.6);
    gen.TesterTout();
    /*vector<int> res(12);
    res = {0, 0, 0, 0, 1, 0, 1, 2, 1, 0, 2, 1};
    CA_Solution *s = new CA_Solution(3, 3, res);
    int coutSol = s->verifierSolution();
    coutSol = s->verifierSolution(Mouvement(0, 0, 1, 0));
    s->appliquerMouvement(Mouvement(0, 0, 1, 0));
    coutSol = s->verifierSolution(Mouvement(2, 1, 0, 1));
    s->appliquerMouvement(Mouvement(2, 1, 0, 1));
    coutSol = s->verifierSolution();
    cout << endl;*/
}
