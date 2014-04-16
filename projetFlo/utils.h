#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <climits>

typedef chrono::time_point<chrono::system_clock> Date;

template<typename T>
inline T* Allocate1DArray(int size, T initValue = T())
{
    T *t = new T[size];
    for(int i = 0; i < size; i++)
    {
        t[i] = initValue;
    }
    return t;
}

template<typename T>
inline T** Allocate2DArray(int size1, int size2, T initValue = T())
{
    T **t = new T*[size1];
    for(int i = 0; i < size1; i++)
    {
        t[i] = Allocate1DArray(size2, initValue);
    }
    return t;
}

template<typename T>
inline T*** Allocate3DArray(int size1, int size2, int size3, T initValue = T())
{
    T ***t = new T**[size1];
    for(int i = 0; i < size1; i++)
    {
        t[i] = Allocate2DArray(size2, size3, initValue);
    }
    return t;
}

template<typename T>
inline T**** Allocate4DArray(int size1, int size2, int size3, int size4, T initValue = T())
{
    T ****t = new T***[size1];
    for(int i = 0; i < size1; i++)
    {
        t[i] = Allocate3DArray(size2, size3, size4, initValue);
    }
    return t;
}

template<typename T>
inline void Delete2DArray(T **t, int size1)
{
    for(int i = 0; i < size1; i++)
    {
        delete[] t[i];
    }
    delete[] t;
}

template<typename T>
inline void Delete3DArray(T ***t, int size1, int size2)
{
    for(int i1 = 0; i1 < size1; i1++)
    {
        Delete2DArray(t[i1], size2);
    }
    delete[] t;
}

template<typename T>
inline void Delete4DArray(T ****t, int size1, int size2, int size3)
{
    for(int i1 = 0; i1 < size1; i1++)
    {
        Delete3DArray(t[i1], size2, size3);
    }
    delete[] t;
}

#endif // UTILS_H
