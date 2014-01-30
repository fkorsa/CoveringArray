#ifndef MATRIX_H
#define MATRIX_H

#include <cstddef>
#include <cstring>

class CMatrix
{
public:
    CMatrix(const int nbColumns = 1, const int nbRows = 1);
    CMatrix(const CMatrix& mat);
    ~CMatrix();
    int *m_data;
    int m_nbColumns, m_nbRows;
};

#endif // MATRIX_H
