#include <cmath>
#include <vector>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "grid.h"
#include "cinder/Rand.h"

using namespace std;
using ci::Vec2f;
using ci::Rand;

void Grid::
init( Vec2f gravity_, int cell_nx, int cell_ny, float lx_)
{
	gravity=gravity_;
	lx=lx_;
	ly=cell_ny*lx/cell_nx;
	h=lx/cell_nx;
	overh=cell_nx/lx;
	// allocate all the grid variables
	u.init(cell_nx+1, cell_ny);
	v.init(cell_nx, cell_ny+1);
	pressure.init(cell_nx, cell_ny);
	marker.init(cell_nx, cell_ny);
	phi.init(cell_nx, cell_ny);
	du.init(cell_nx+1, cell_ny);
	dv.init(cell_nx, cell_ny+1);
	poisson.init(cell_nx, cell_ny);
	preconditioner.init(cell_nx, cell_ny);
	m.init(cell_nx, cell_ny);
	r.init(cell_nx, cell_ny);
	z.init(cell_nx, cell_ny);
	s.init(cell_nx, cell_ny);
}

float Grid::
CFL(void)
{
   float maxv2=max(h*gravity.length(), sqr(u.infnorm())+sqr(v.infnorm()));
   if(maxv2<1e-16) maxv2=1e-16;
   return h/sqrt(maxv2);
}

void Grid::
save_velocities(void)
{
   u.copy_to(du);
   v.copy_to(dv);
}

void Grid::
add_gravity(float dt)
{
   float dtgu = dt * gravity.x;
   float dtgv = dt * gravity.y;
   for( int i = 0; i < u.size; ++i )
      u.data[i] -= dtgu;
   for( int i = 0; i < v.size; ++i )
      v.data[i] -= dtgv;
}

void Grid::
compute_distance_to_fluid(void)
{
   init_phi();
   for(int i=0; i<2; ++i)
      sweep_phi();
}

void Grid::
extend_velocity(void)
{
   for(int i=0; i<4; ++i)
      sweep_velocity();
}

void Grid::
apply_boundary_conditions(void)
{
   int i, j;
   // first mark where solid is
   for(j=0; j<marker.ny; ++j)
      marker(0,j)=marker(marker.nx-1,j)=SOLIDCELL;
   for(i=0; i<marker.nx; ++i)
      marker(i,0)=marker(i,marker.ny-1)=SOLIDCELL;
	
	
   // now makre sure nothing leaves the domain
   for(j=0; j<u.ny; ++j)
      u(0,j)=u(1,j)=u(u.nx-1,j)=u(u.nx-2,j)=0;
   for(i=0; i<v.nx; ++i)
      v(i,0)=v(i,1)=v(i,v.ny-1)=v(i,v.ny-2)=0;
}

void Grid::
make_incompressible(void)
{
   find_divergence();
   form_poisson();
   form_preconditioner();
   solve_pressure(100, 1e-5);
   add_gradient();
}

void Grid::
get_velocity_update(void)
{
   int i;
   for(i=0; i<u.size; ++i)
      du.data[i]=u.data[i]-du.data[i];
   for(i=0; i<v.size; ++i)
      dv.data[i]=v.data[i]-dv.data[i];
}

//====================================== private helper functions ============================

void Grid::
init_phi(void)
{
   int i, j;
   // start off with indicator inside the fluid and overestimates of distance outside
   float large_distance=phi.nx+phi.ny+2;
   for(i=0; i<phi.size; ++i)
      phi.data[i]=large_distance;
   for(j=1; j<phi.ny-1; ++j) for(i=1; i<phi.nx-1; ++i){
      if(marker(i,j)==FLUIDCELL){
         phi(i,j)=-0.5;
      }
   }
}

static inline void solve_distance(float p, float q, float &r)
{
   float d=std::min(p,q)+1;
   if(d>std::max(p,q)) 
      d=(p+q+sqrt(2-sqr(p-q)))/2;
   if(d<r) r=d;
}

void Grid::
sweep_phi(void)
{
   // fast sweeping outside the fluid in all four sweep directions
   int i, j;
   for(j=1; j<phi.ny; ++j) for(i=1; i<phi.nx; ++i)
      if(marker(i,j)!=FLUIDCELL)
         solve_distance(phi(i-1,j), phi(i,j-1), phi(i,j));
   for(j=phi.ny-2; j>=0; --j) for(i=1; i<phi.nx; ++i)
      if(marker(i,j)!=FLUIDCELL)
         solve_distance(phi(i-1,j), phi(i,j+1), phi(i,j));
   for(j=1; j<phi.ny; ++j) for(i=phi.nx-2; i>=0; --i)
      if(marker(i,j)!=FLUIDCELL)
         solve_distance(phi(i+1,j), phi(i,j-1), phi(i,j));
   for(j=phi.ny-2; j>=0; --j) for(i=phi.nx-2; i>=0; --i)
      if(marker(i,j)!=FLUIDCELL)
         solve_distance(phi(i+1,j), phi(i,j+1), phi(i,j));
}

void Grid::
sweep_u(int i0, int i1, int j0, int j1)
{
   int di=(i0<i1) ? 1 : -1, dj=(j0<j1) ? 1 : -1;
   float dp, dq, alpha;
   for(int j=j0; j!=j1; j+=dj) for(int i=i0; i!=i1; i+=di)
      if(marker(i-1,j)==AIRCELL && marker(i,j)==AIRCELL){
         dp=di*(phi(i,j)-phi(i-1,j));
         if(dp<0) continue; // not useful on this sweep direction
         dq=0.5*(phi(i-1,j)+phi(i,j)-phi(i-1,j-dj)-phi(i,j-dj));
         if(dq<0) continue; // not useful on this sweep direction
         if(dp+dq==0) alpha=0.5;
         else alpha=dp/(dp+dq);
         u(i,j)=alpha*u(i-di,j)+(1-alpha)*u(i,j-dj);
      }
}

void Grid::
sweep_v(int i0, int i1, int j0, int j1)
{
   int di=(i0<i1) ? 1 : -1, dj=(j0<j1) ? 1 : -1;
   float dp, dq, alpha;
   for(int j=j0; j!=j1; j+=dj) for(int i=i0; i!=i1; i+=di)
      if(marker(i,j-1)==AIRCELL && marker(i,j)==AIRCELL){
         dq=dj*(phi(i,j)-phi(i,j-1));
         if(dq<0) continue; // not useful on this sweep direction
         dp=0.5*(phi(i,j-1)+phi(i,j)-phi(i-di,j-1)-phi(i-di,j));
         if(dp<0) continue; // not useful on this sweep direction
         if(dp+dq==0) alpha=0.5;
         else alpha=dp/(dp+dq);
         v(i,j)=alpha*v(i-di,j)+(1-alpha)*v(i,j-dj);
      }
}

void Grid::
sweep_velocity(void)
{
   int i, j;
   // sweep u, only into the air
   sweep_u(1, u.nx-1, 1, u.ny-1);
   sweep_u(1, u.nx-1, u.ny-2, 0);
   sweep_u(u.nx-2, 0, 1, u.ny-1);
   sweep_u(u.nx-2, 0, u.ny-2, 0);
   for(i=0; i<u.nx; ++i){
      u(i,0)=u(i,1); u(i,u.ny-1)=u(i,u.ny-2);
   }
   for(j=0; j<u.ny; ++j){
       u(0,j)=u(1,j); u(u.nx-1,j)=u(u.nx-2,j);
   }
   // now the same for v
   sweep_v(1, v.nx-1, 1, v.ny-1);
   sweep_v(1, v.nx-1, v.ny-2, 0);
   sweep_v(v.nx-2, 0, 1, v.ny-1);
   sweep_v(v.nx-2, 0, v.ny-2, 0);
   for(i=0; i<v.nx; ++i){
      v(i,0)=v(i,1); v(i,v.ny-1)=v(i,v.ny-2);
   }
   for(j=0; j<v.ny; ++j){
      v(0,j)=v(1,j); v(v.nx-1,j)=v(v.nx-2,j);
   }
}

void Grid::
find_divergence(void)
{
   r.zero();
   for(int j=0; j<r.ny; ++j) for(int i=0; i<r.nx; ++i){
      if(marker(i,j)==FLUIDCELL)
         r(i,j)=u(i+1,j)-u(i,j)+v(i,j+1)-v(i,j);
   }
}

void Grid::
form_poisson(void)
{
   poisson.zero();
   for(int j=1; j<poisson.ny-1; ++j) for(int i=1; i<poisson.nx-1; ++i){
      if(marker(i,j)==FLUIDCELL){
         if(marker(i-1,j)!=SOLIDCELL)
            poisson(i,j,0)+=1;
         if(marker(i+1,j)!=SOLIDCELL){
            poisson(i,j,0)+=1;
            if(marker(i+1,j)==FLUIDCELL)
               poisson(i,j,1)=-1;
         }
         if(marker(i,j-1)!=SOLIDCELL)
            poisson(i,j,0)+=1;
         if(marker(i,j+1)!=SOLIDCELL){
            poisson(i,j,0)+=1;
            if(marker(i,j+1)==FLUIDCELL)
               poisson(i,j,2)=-1;
         }
      }
   }
}

void Grid::
apply_poisson(const Array2d &x, Array2d &y)
{
   y.zero();
   for(int j=1; j<poisson.ny-1; ++j) for(int i=1; i<poisson.nx-1; ++i){
      if(marker(i,j)==FLUIDCELL){
         y(i,j)=poisson(i,j,0)*x(i,j) + poisson(i-1,j,1)*x(i-1,j)
                                      + poisson(i,j,1)*x(i+1,j)
                                      + poisson(i,j-1,2)*x(i,j-1)
                                      + poisson(i,j,2)*x(i,j+1);
      }
   }
}

void Grid::
form_preconditioner()
{
   const double mic_parameter=0.99;
   double d;
   preconditioner.zero();
   for(int j=1; j<preconditioner.ny-1; ++j) for(int i=1; i<preconditioner.nx-1; ++i){
      if(marker(i,j)==FLUIDCELL){
         d=poisson(i,j,0) - sqr( poisson(i-1,j,1)*preconditioner(i-1,j) )
                          - sqr( poisson(i,j-1,2)*preconditioner(i,j-1) )
                          - mic_parameter*( poisson(i-1,j,1)*poisson(i-1,j,2)*sqr(preconditioner(i-1,j))
                                           +poisson(i,j-1,2)*poisson(i,j-1,1)*sqr(preconditioner(i,j-1)) );
         preconditioner(i,j)=1/sqrt(d+1e-6);
      }
   }
}

void Grid::
apply_preconditioner(const Array2d &x, Array2d &y, Array2d &m)
{
   int i, j;
   float d;
   m.zero();
   // solve L*m=x
   for(j=1; j<x.ny-1; ++j) for(i=1; i<x.nx-1; ++i)
      if(marker(i,j)==FLUIDCELL){
         d=x(i,j) - poisson(i-1,j,1)*preconditioner(i-1,j)*m(i-1,j)
                  - poisson(i,j-1,2)*preconditioner(i,j-1)*m(i,j-1);
         m(i,j)=preconditioner(i,j)*d;
      }
   // solve L'*y=m
   y.zero();
   for(j=x.ny-2; j>0; --j) for(i=x.nx-2; i>0; --i)
      if(marker(i,j)==FLUIDCELL){
         d=m(i,j) - poisson(i,j,1)*preconditioner(i,j)*y(i+1,j)
                  - poisson(i,j,2)*preconditioner(i,j)*y(i,j+1);
         y(i,j)=preconditioner(i,j)*d;
      }
}

void Grid::
solve_pressure(int maxits, double tolerance)
{
   int its;
   double tol=tolerance*r.infnorm();
   pressure.zero();
   if(r.infnorm()==0)
      return;
   apply_preconditioner(r, z, m);
   z.copy_to(s);
   double rho=z.dot(r);
   if(rho==0)
      return;
   for(its=0; its<maxits; ++its){
      apply_poisson(s, z);
      double alpha=rho/s.dot(z);
      pressure.increment(alpha, s);
      r.increment(-alpha, z);
      if(r.infnorm()<=tol){
         return;
      }
      apply_preconditioner(r, z, m);
      double rhonew=z.dot(r);
      double beta=rhonew/rho;
      s.scale_and_increment(beta, z);
      rho=rhonew;
   }
   printf("Didn't converge in pressure solve (its=%d, tol=%g, |r|=%g)\n", its, tol, r.infnorm());
}

void Grid::
add_gradient(void)
{
   int i, j;
   for(j=1; j<u.ny-1; ++j) for(i=2; i<u.nx-2; ++i){
      if(marker(i-1,j)|marker(i,j)==FLUIDCELL){ // if at least one is FLUID, neither is SOLID
         u(i,j)+=pressure(i,j)-pressure(i-1,j);
      }
   }
   for(j=2; j<v.ny-2; ++j) for(i=1; i<v.nx-1; ++i){
      if(marker(i,j-1)|marker(i,j)==FLUIDCELL){ // if at least one is FLUID, neither is SOLID
         v(i,j)+=pressure(i,j)-pressure(i,j-1);
      }
   }
}

