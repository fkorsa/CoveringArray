#include <iostream>
#include <map>
using namespace std;

#include "generateur.h"
#include "utils.h"

int main()
{
    Generateur gen(10, 5.4/8.6);
    gen.TesterTout();
    return EXIT_SUCCESS;
}
