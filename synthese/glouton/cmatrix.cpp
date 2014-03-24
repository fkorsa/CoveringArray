#include "cmatrix.h"

CMatrix::CMatrix(const int nbColumns, const int nbRows) :
    m_data(NULL),
    m_nbColumns(nbColumns),
    m_nbRows(nbRows)
{
    m_data = new int[m_nbColumns*m_nbRows];
}


CMatrix::CMatrix(const CMatrix& mat) :
    m_data(NULL),
    m_nbColumns(mat.m_nbColumns),
    m_nbRows(mat.m_nbRows)
{
    m_data = new int[m_nbColumns*m_nbRows];
    memcpy(m_data, mat.m_data, m_nbColumns*m_nbRows*sizeof(int));
}

CMatrix::~CMatrix()
{
    delete [] m_data;
}
