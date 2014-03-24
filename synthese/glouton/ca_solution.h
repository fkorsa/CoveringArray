#ifndef CA_SOLUTION_H
#define CA_SOLUTION_H

#include <fstream>
#include <cstring>
#include <cmatrix.h>
#include <cstdlib>
#include <chrono>
#include <vector>
#include <iostream>
#include <math.h>

#include <utils.h>

using namespace std;

class CA_Solution
{
public:
    CA_Solution();
    ~CA_Solution();
    void saveMatrixToFile(const char* filename);
    void getMatrixFromFile(const char* filename);
    void testAlgo();
    void generateMatrix(double alpha);
    int chooseSymbol(double *symbolsScores, double scoresSum, double alpha = 1);
    int getErrorsNumber();
    int getRowsNumber();
private:
    CMatrix *m_currentMat;
    unsigned int m_nbSymbols, m_nbErrors, m_nbColumns, m_nbAlphas;
    ALGO_TYPE m_algoType;
    char m_inputFileData[100], m_inputFileAlphas[100], m_inputFileMatrix[100], m_outputFileMatrix[100], m_outputFileStats[100];
    vector<double> m_alphasVector;
};

#endif // CA_SOLUTION_H
