#ifndef ARRAY2
#define ARRAY2

#include <cstdio>
#include <cmath>
#include <cstring>

template<class T>
struct Array2{
   int nx, ny;
   int size;
   T *data;

   Array2()
      :nx(0), ny(0), size(0), data(0)
   {}

   Array2(int nx_, int ny_)
      :nx(0), ny(0), size(0), data(0)
   { init(nx_, ny_); }

   void init(int nx_, int ny_)
   {
      delete_memory();
      nx=nx_;
      ny=ny_;
      size=nx*ny;
      data=new T[size];
      zero();
   }

   ~Array2()
   { delete_memory(); }

   void delete_memory()
   {
      delete[] data; data=0;
      nx=ny=size=0;
   }

   const T &operator() (int i, int j) const
   { return data[i+nx*j]; }

   T &operator() (int i, int j)
   { return data[i+nx*j]; }

   T bilerp(int i, int j, T fx, T fy)
   { return (1-fx)*((1-fy)*(*this)(i,j)+fy*(*this)(i,j+1))+fx*((1-fy)*(*this)(i+1,j)+fy*(*this)(i+1,j+1)); }

   void copy_to(Array2 &a) const
   { std::memcpy(a.data, data, size*sizeof(T)); }

   T infnorm() const
   { 
      T r=0;
      for(int i=0; i<size; ++i)
         if(!(std::fabs(data[i])<=r)) r=std::fabs(data[i]);
      return r;
   }

   void zero()
   { std::memset(data, 0, size*sizeof(T)); }

   double dot(const Array2 &a) const
   {
      double r=0;
      for(int i=0; i<size; ++i)
         r+=data[i]*a.data[i];
      return r;
   }

   void increment(double scale, const Array2 &a)
   { for(int i=0; i<size; ++i) data[i]+=scale*a.data[i]; }

   void scale_and_increment(double scale, const Array2 &a)
   { for(int i=0; i<size; ++i) data[i]=scale*data[i]+a.data[i]; }

   void write_matlab(FILE *fp, const char *variable_name)
   {
      fprintf(fp, "%s=[", variable_name);
      for(int i=0; i<nx; ++i){
	 for(int j=0; j<ny; ++j)
	    fprintf(fp, "%lg ", (double)(*this)(i,j));
	 fprintf(fp, ";\n");
      }
      fprintf(fp,"];\n");
   }
};

typedef Array2<float> Array2f;
typedef Array2<double> Array2d;
typedef Array2<char> Array2c;

template<class T>
struct Array2x3{
   int nx, ny;
   int size;
   T *data;

   Array2x3()
      :nx(0), ny(0), size(0), data(0)
   {}

   Array2x3(int nx_, int ny_)
      :nx(0), ny(0), size(0), data(0)
   { init(nx_, ny_); }

   void init(int nx_, int ny_)
   {
      delete_memory();
      nx=nx_;
      ny=ny_;
      size=3*nx*ny;
      data=new T[size];
      zero();
   }

   ~Array2x3()
   { delete_memory(); }

   void delete_memory()
   {
      delete[] data; data=0;
      nx=ny=size=0;
   }

   const T &operator() (int i, int j, int k) const
   { return data[(i+nx*j)*3+k]; }

   T &operator() (int i, int j, int k)
   { return data[(i+nx*j)*3+k]; }

   void zero()
   { std::memset(data, 0, size*sizeof(T)); }

   T infnorm() const
   { 
      T r=0;
      for(int i=0; i<size; ++i)
         if(!(std::fabs(data[i])<=r)) r=std::fabs(data[i]);
      return r;
   }

   void write_matlab(FILE *fp, const char *variable_name)
   {
      fprintf(fp, "%s=reshape([", variable_name);
      for(int i=0; i<size; ++i)
         fprintf(fp, "%lg ", (double)data[i]);
      fprintf(fp,"],[3 %d %d]);\n", nx, ny);
   }
};

typedef Array2x3<float> Array2x3f;
typedef Array2x3<double> Array2x3d;

#endif
