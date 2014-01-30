#include "ca_solution.h"

CA_Solution::CA_Solution() :
    m_currentMat(NULL),
    m_nbSymbols(3),
    m_nbErrors(0),
    m_nbColumns(20),
    m_nbAlphas(7),
    m_algoType(RANDOM_LINEAR)
{
    strncpy(m_outputFileStats, "outputStats", 20);
    strncpy(m_inputFileData, "inputData", 20);
    strncpy(m_inputFileAlphas, "inputAlphas", 20);
    strncpy(m_inputFileMatrix, "inputMatrix", 20);
    strncpy(m_outputFileMatrix, "outputMatrix", 20);
    m_alphasVector = {-1, -0.66, -0.33, 0, 0.33, 0.66, 1};
}

void CA_Solution::testAlgo()
{
    ifstream infile(m_inputFileData);
    ofstream outfile(m_outputFileStats);
    int minRows, maxRows, rows[100];
    double meanTime, meanRows, standardDev;
    if(outfile.is_open())
    {
        while (infile >> m_nbSymbols >> m_nbColumns)
        {
            outfile << m_nbSymbols << " " << m_nbColumns << endl;
            outfile << "Parametre_aleatoire Numero_iteration Nombre_de_symboles nombre_de_colonnes nombre_de_lignes nombre_derreurs temps_dexecution" << endl;

            for(unsigned int indexAlpha=0; indexAlpha<m_nbAlphas; indexAlpha++)
            {
                meanTime = 0;
                meanRows = 0;
                for(int i=0; i<100; i++)
                {
                    chrono::time_point<chrono::system_clock> start, end;
                    start = chrono::system_clock::now();
                    generateMatrix(m_alphasVector[indexAlpha]);
                    end = chrono::system_clock::now();
                    chrono::duration<double> realTime = end-start;
                    outfile << m_alphasVector[indexAlpha] << " " << i << " " << m_nbSymbols << " " << m_nbColumns << " " << m_currentMat->m_nbRows << " " << getErrorsNumber()
                           << " " << 1000*realTime.count() << endl;
                    if(i==0)
                    {
                        minRows = maxRows = m_currentMat->m_nbRows;
                    }
                    if(minRows > m_currentMat->m_nbRows)
                    {
                        minRows = m_currentMat->m_nbRows;
                    }
                    if(maxRows < m_currentMat->m_nbRows)
                    {
                        maxRows = m_currentMat->m_nbRows;
                    }
                    meanTime += 1000*realTime.count();
                    rows[i] = m_currentMat->m_nbRows;
                    meanRows += m_currentMat->m_nbRows;

                }
                meanRows /= 100;
                standardDev = 0;
                for(int i=0; i<100; i++)
                {
                    standardDev += pow(rows[i]-meanRows, 2);
                }
                standardDev = sqrt(standardDev/100);
                outfile << "Nombre_lignes_min nombre_lignes_max ecart_type temps_moyen" << endl;
                outfile << minRows << " " << maxRows << " " << standardDev << " " << meanTime/100 << endl;
            }
        }
        outfile.close();
    }
}

void CA_Solution::getMatrixFromFile(const char* filename)
{
    ifstream infile(filename);
    int nbColumns, nbRows, coeff, indexCoeff = 0;
    infile >> m_nbSymbols >> nbColumns >> nbRows >> m_nbErrors;
    m_currentMat = new CMatrix(nbColumns, nbRows);
    while (infile >> coeff)
    {
        m_currentMat->m_data[indexCoeff++] = coeff;
    }
}

int CA_Solution::getErrorsNumber()
{
    int nbRows = m_currentMat->m_nbRows;
    unsigned int column1, column2, row, symbol1, symbol2;
    int offsetRow, nbErrors = 0;
    bool pairAppears[m_nbSymbols][m_nbSymbols];
    int *matrixCoeffs = m_currentMat->m_data;
    for(column1=0; column1<m_nbColumns-1; column1++)
    {
        for(column2=column1+1; column2<m_nbColumns; column2++)
        {
            memset(pairAppears, 0, m_nbSymbols*m_nbSymbols*sizeof(bool));
            for(row=0; row<nbRows; row++)
            {
                offsetRow = row*m_nbColumns;
                pairAppears[matrixCoeffs[offsetRow+column1]][matrixCoeffs[offsetRow+column2]] = true;
            }
            for(symbol1=0; symbol1<m_nbSymbols; symbol1++)
            {
                for(symbol2=0; symbol2<m_nbSymbols; symbol2++)
                {
                    if(!pairAppears[symbol1][symbol2])
                    {
                        nbErrors++;
                    }
                }
            }
        }
    }
    return nbErrors;
}

CA_Solution::~CA_Solution()
{
    if(m_currentMat)
        delete m_currentMat;
}

void CA_Solution::generateMatrix(double alpha)
{
    int nbPairsSymbols = m_nbSymbols*m_nbSymbols;
    int nbPairsSymbolsResolved = 0, nbPairsToResolve = nbPairsSymbols*((m_nbColumns*(m_nbColumns-1))/2);
    vector<int> matrixData(nbPairsSymbols + m_nbColumns, 0);
    bool ****pairAppears = new bool***[m_nbColumns];
    int i, j, k;
    for(i=0; i<m_nbColumns; i++)
    {
        pairAppears[i] = new bool**[m_nbColumns];
        for(j=0; j<m_nbColumns; j++)
        {
            pairAppears[i][j] = new bool*[m_nbSymbols];
            for(k=0; k<m_nbSymbols; k++)
            {
                pairAppears[i][j][k] = new bool[m_nbSymbols];
                memset(pairAppears[i][j][k], 0, m_nbSymbols*sizeof(bool));
            }
        }
    }
    double *symbolsScores = new double[m_nbSymbols], scoresSum, invNbColumns = 1/(double)m_nbColumns;
    int nbRows = 0, bestSymbol;
    int currentColumn, currentSymbol, currentSymbol2, offsetRow, otherColumn;
    m_nbSymbols = m_nbSymbols;
    while(nbPairsSymbolsResolved < nbPairsToResolve)
    {
        offsetRow = nbRows*m_nbColumns;
        for(currentColumn=0; currentColumn<m_nbColumns; currentColumn++)
        {
            memset(symbolsScores, 0, m_nbSymbols*sizeof(double));
            scoresSum = 0;
            for(currentSymbol=0; currentSymbol<m_nbSymbols; currentSymbol++)
            {
                for(otherColumn=0; otherColumn<currentColumn; otherColumn++)
                {
                    if(!pairAppears[otherColumn][currentColumn][matrixData[offsetRow+otherColumn]][currentSymbol])
                    {
                        symbolsScores[currentSymbol]++;
                        scoresSum++;
                    }
                }
                for(otherColumn=currentColumn+1; otherColumn<m_nbColumns; otherColumn++)
                {
                    for(currentSymbol2=0; currentSymbol2<m_nbSymbols; currentSymbol2++)
                    {
                        if(!pairAppears[currentColumn][otherColumn][currentSymbol][currentSymbol2])
                        {
                            symbolsScores[currentSymbol]+=invNbColumns;
                            scoresSum+=invNbColumns;
                        }
                    }
                }
            }
            bestSymbol = chooseSymbol(symbolsScores, scoresSum, alpha);
            if(offsetRow+currentColumn>matrixData.size())
                bestSymbol = matrixData.size();
            matrixData[offsetRow+currentColumn] = bestSymbol;
            for(otherColumn=0; otherColumn<currentColumn; otherColumn++)
            {
                if(!pairAppears[otherColumn][currentColumn][matrixData[offsetRow+otherColumn]][bestSymbol])
                {
                    pairAppears[otherColumn][currentColumn][matrixData[offsetRow+otherColumn]][bestSymbol] = true;
                    nbPairsSymbolsResolved++;
                }
            }
        }
        nbRows++;
        if(matrixData.size() < (nbRows+1)*m_nbColumns)
        {
            matrixData.resize((nbRows+1)*m_nbColumns);
        }
    }
    m_currentMat = new CMatrix(m_nbColumns, nbRows);
    memcpy(m_currentMat->m_data, &matrixData[0], m_nbColumns*nbRows*sizeof(int));
    for(i=0; i<m_nbColumns; i++)
    {
        for(j=0; j<m_nbColumns; j++)
        {
            for(k=0; k<m_nbSymbols; k++)
            {
                delete [] pairAppears[i][j][k];
            }
            delete [] pairAppears[i][j];
        }
        delete [] pairAppears[i];
    }
    delete [] pairAppears;
}

void CA_Solution::saveMatrixToFile(const char* filename)
{
    ofstream myfile(filename);
    int nbColumns = m_currentMat->m_nbColumns, nbRows = m_currentMat->m_nbRows;
    int offsetRow, row, column;
    if(myfile.is_open())
    {
        myfile << m_nbSymbols << " " << m_currentMat->m_nbColumns << " " << m_currentMat->m_nbRows << " " << getErrorsNumber() << endl;
        for(row=0; row<nbRows; row++)
        {
            offsetRow = row*nbColumns;
            for(column=0; column<nbColumns; column++)
            {
                myfile << m_currentMat->m_data[offsetRow+column] << " ";
            }
            myfile << endl;
        }
        myfile.close();
    }
}

int CA_Solution::getRowsNumber()
{
    if(m_currentMat)
        return m_currentMat->m_nbRows;
    else
        return 0;
}

int CA_Solution::chooseSymbol(double* symbolsScores, double scoresSum, double alpha)
{
    int currentSymbol, maxScore, bestSymbol, bestScoreSymbol, nbBestScoreSymbols, *bestScoreSymbols = new int[m_nbSymbols];
    double invNbSymbols = 1/(double)m_nbSymbols, cumulatedSum, randomNumber, pk, expSum, *expProbas = new double[m_nbSymbols];
    switch(m_algoType)
    {
    case PURE_GREEDY:
        bestSymbol = 0;
        maxScore = symbolsScores[0];
        for(currentSymbol=1; currentSymbol<m_nbSymbols; currentSymbol++)
        {
            if(symbolsScores[currentSymbol] > maxScore)
            {
                maxScore = symbolsScores[currentSymbol];
                bestSymbol = currentSymbol;
            }
        }
        break;
    case GREEDY_STOCHASTIC:
        bestSymbol = 0;
        maxScore = symbolsScores[0];
        for(currentSymbol=1; currentSymbol<m_nbSymbols; currentSymbol++)
        {
            if(symbolsScores[currentSymbol] > maxScore)
            {
                maxScore = symbolsScores[currentSymbol];
            }
        }
        nbBestScoreSymbols = 0;
        for(currentSymbol=0; currentSymbol<m_nbSymbols; currentSymbol++)
        {
            if(symbolsScores[currentSymbol] == maxScore)
            {
                bestScoreSymbols[nbBestScoreSymbols++] = currentSymbol;
            }
        }
        randomNumber = rand() / (double)RAND_MAX;
        for(currentSymbol=0; currentSymbol<nbBestScoreSymbols; currentSymbol++)
        {
            cumulatedSum += invNbSymbols;
            if(randomNumber <= cumulatedSum)
            {
                bestSymbol = bestScoreSymbols[currentSymbol];
                break;
            }
        }
        break;
    case RANDOM_LINEAR:
        bestSymbol = m_nbSymbols - 1;
        if(scoresSum != 0)
        {
            cumulatedSum = 0;
            randomNumber = rand() / (double)RAND_MAX;
            if(alpha < 0)
            {
                for(currentSymbol=0; currentSymbol<m_nbSymbols; currentSymbol++)
                {
                    pk = symbolsScores[currentSymbol]/(double)scoresSum;
                    cumulatedSum += (pk - invNbSymbols)*alpha + pk;
                    if(randomNumber <= cumulatedSum)
                    {
                        bestSymbol = currentSymbol;
                        break;
                    }
                }
            }
            else
            {
                maxScore = symbolsScores[0];
                bestScoreSymbol = 0;
                for(currentSymbol=1; currentSymbol<m_nbSymbols; currentSymbol++)
                {
                    if(symbolsScores[currentSymbol] > maxScore)
                    {
                        maxScore = symbolsScores[currentSymbol];
                        bestScoreSymbol = currentSymbol;
                    }
                }
                for(currentSymbol=0; currentSymbol<m_nbSymbols; currentSymbol++)
                {
                    pk = symbolsScores[currentSymbol]/(double)scoresSum;
                    cumulatedSum += pk - pk*alpha;
                    if(currentSymbol == bestScoreSymbol)
                    {
                        cumulatedSum += alpha;
                    }
                    if(randomNumber < cumulatedSum)
                    {
                        bestSymbol = currentSymbol;
                        break;
                    }
                }
            }
        }
        break;
    case RANDOM_EXP:
        bestSymbol = m_nbSymbols - 1;
        cumulatedSum = 0;
        expSum = 0;
        for(currentSymbol=0; currentSymbol<m_nbSymbols; currentSymbol++)
        {
            expProbas[currentSymbol] = exp(symbolsScores[currentSymbol]/alpha);
            expSum += expProbas[currentSymbol];
        }
        randomNumber = rand() / (double)RAND_MAX;
        for(currentSymbol=0; currentSymbol<m_nbSymbols; currentSymbol++)
        {
            cumulatedSum += expProbas[currentSymbol]/expSum;
            if(randomNumber <= cumulatedSum)
            {
                bestSymbol = currentSymbol;
                break;
            }
        }
        break;
    }
    return bestSymbol;
}
