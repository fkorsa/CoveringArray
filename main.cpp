#include <iostream>
#include <time.h>
#include <chrono>
#include <string>
#include <ca_solution.h>
#include <utils.h>

using namespace std;

void printUsage();

int main(int argc, char **argv)
{
    int argumentIndex;
    char **argument = argv+1;
    int iterationsNumber = 100;
    double alpha = 1;
    long seed = time(NULL);
    char printMatrixFile[100] = "outputMatrix", printStatsFile[100] = "output";
    bool printStats = true, printMatrix = false;

    CA_Solution solution;

    if(argc > 1)
    {
        argumentIndex = 0;
        while(argumentIndex<argc-1)
        {
            if(!strncmp(*argument, "--help", 7))
            {
                cout << "Options :" << endl;
                cout << "   -i number" << endl;
                cout << "       Specifies the number of iterations. There will be 'number' matrices generated for each value of alpha, the randomization parameter." << endl;
                cout << "       In case of the purely greedy version, this parameter is ignored : only one matrix is generated." << endl;
                cout << "   -r pure_greedy|greedy_stochastic|random_linear|random_exp" << endl;
                cout << "       Specifies the type of algorithm that generates the matrix." << endl;
                cout << "       pure_greedy : Purely greedy. No randomization." << endl;
                cout << "       greedy_stochastic : Greedy, but symbols with same scores are randomly chosen." << endl;
                cout << "       random_linear : Greedy with a linear chance to choose symbols with poor scores." << endl;
                cout << "       random_exp : Greedy with an exponential chance to choose symbols with poor scores." << endl;
                cout << "       Default : random_linear." << endl;
                cout << "   -a number" << endl;
                cout << "       Specifies the parameter of the randomization as a floating point number. Default 1." << endl;
                cout << "   -s seed" << endl;
                cout << "       Specifies the seed for the randomization. If not specified, the program uses time(NULL) as a seed." << endl;
                cout << "   -p file" << endl;
                cout << "       Prints the best generated matrix in the file 'file'." << endl;
                cout << "       By default, the matrix is not printed." << endl;
                cout << "   -o file" << endl;
                cout << "       Prints the statistics in the file 'file'." << endl;
                cout << "       By default, the results are printed in the file 'output' in the current directory." << endl;
                cout << "       To prevent from printing, use -d." << endl;
                cout << "   -d" << endl;
                cout << "       Don't print the statistics." << endl;
                return 0;
            }
            else if(!strncmp(*argument, "-i", 3))
            {
                argument++;
                argumentIndex++;
                iterationsNumber = strtol(*argument, NULL, 10);
                if(argumentIndex>=argc-1 || !iterationsNumber)
                {
                    printUsage();
                    return 0;
                }
            }
            else if(!strncmp(*argument, "-a", 3))
            {
                argument++;
                argumentIndex++;
                alpha = strtol(*argument, NULL, 10);
                if(argumentIndex>=argc-1 || !alpha)
                {
                    printUsage();
                    return 0;
                }
            }
            else if(!strncmp(*argument, "-s", 3))
            {
                argument++;
                argumentIndex++;
                seed = strtol(*argument, NULL, 10);
                if(argumentIndex>=argc-1 || seed<=0)
                {
                    printUsage();
                    return 0;
                }
            }
            else if(!strncmp(*argument, "-d", 3))
            {
                printStats = false;
            }
            else if(!strncmp(*argument, "-p", 7))
            {
                argument++;
                argumentIndex++;
                if(argumentIndex>=argc-1)
                {
                    printUsage();
                    return 0;
                }
                strncpy(printMatrixFile, *argument, 100);
                printMatrix = true;
            }
            else if(!strncmp(*argument, "-o", 7))
            {
                argument++;
                argumentIndex++;
                if(argumentIndex>=argc-1)
                {
                    printUsage();
                    return 0;
                }
                strncpy(printStatsFile, *argument, 100);
            }
            else if(!strncmp(*argument, "-r", 7))
            {
                argument++;
                argumentIndex++;
                if(argumentIndex>=argc-1)
                {
                    printUsage();
                    return 0;
                }
                if(!strncmp(*argument, "pure_greedy", 18))
                {
                    solution.setAlgoType(PURE_GREEDY);
                }
                else if(!strncmp(*argument, "greedy_stochastic", 18))
                {
                    solution.setAlgoType(GREEDY_STOCHASTIC);
                }
                else if(!strncmp(*argument, "random_linear", 18))
                {
                    solution.setAlgoType(RANDOM_LINEAR);
                }
                else if(!strncmp(*argument, "random_exp", 18))
                {
                    solution.setAlgoType(RANDOM_EXP);
                }
                else
                {
                    printUsage();
                    return 0;
                }
            }
            argumentIndex++;
            argument++;
        }
    }

    srand(seed);

    /*CA_Solution solution("output");
    cout << "Nombre d'erreurs : " << solution.getErrorsNumber() << endl;*/



    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    clock_t clockTime = clock();
    //solution.generateGlouton(15, 50);
    solution.generateMatrix(0.03);
    float userTime = 1000*((float)(clock() - clockTime))/CLOCKS_PER_SEC;
    end = chrono::system_clock::now();
    chrono::duration<double> realTime = end-start;
    cout << "Temps d'execution (user) : " << userTime << "ms" << endl;
    cout << "Temps d'execution (real) : " << 1000*realTime.count() << "ms" << endl;
    cout << "Nombre d'erreurs : " << solution.getErrorsNumber() << endl;
    cout << "Nombre de lignes : " << solution.getRowsNumber() << endl;
    solution.saveMatrixToFile("output");


    //CA_Solution solution("input", "output");
    return 0;
}

void printUsage()
{
    cout << "Usage :" << endl;
    cout << "./covering_array [-i number] [-r pure_greedy|greedy_stochastic|random_linear|random_exp] [-a number] [-s seed] [-p file] [-o file] [-d]" << endl;
    cout << "--help to display the list of options. See README for details." << endl;
}
