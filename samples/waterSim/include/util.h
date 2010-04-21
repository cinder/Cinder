#ifndef UTIL_H
#define UTIL_H

#include <vector>

// fix Microsoft VC++ oddities
#ifdef min // MS defines min as a macro, making it impossible to use the identifier for functions etc.
#undef min
#endif
#ifdef max // MS defines max as a macro, making it impossible to use the identifier for functions etc.
#undef max
#endif
#ifndef M_PI // MS fails to define M_PI in the standard math header file
#define M_PI 3.14159265358979323846
#endif

template<class T>
inline T sqr(const T &x)
{ return x*x; }

/*
template<class T>
inline T min(const T &a1, const T &a2)
{ if(a1<a2) return a1; else return a2; }
*/

template<class T>
inline T min(const T &a1, const T &a2, const T &a3)
{ if(a1<a2) return min(a1,a3); else return min(a2,a3); }

/*
template<class T>
inline T max(const T &a1, const T &a2)
{ if(a1>a2) return a1; else return a2; }
*/

template<class T>
inline T max(const T &a1, const T &a2, const T &a3)
{ if(a1>a2) return max(a1,a3); else return max(a2,a3); }

template<class T>
inline T clamp(T a, T lower, T upper)
{
   if(a<lower) return lower;
   else if(a>upper) return upper;
   else return a;
}

/*
template<class T>
inline void swap(T &a, T &b)
{ T c=a; a=b; b=c; }
*/

template<class T>
void zero(std::vector<T> &v)
{ for(int i=v.size()-1; i>=0; --i) v[i]=0; }

#endif
