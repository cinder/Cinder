#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include "particles.h"
#include "util.h"

using namespace std;
using ci::Vec2f;

void Particles::
add_particle(const Vec2f &px, const Vec2f &pu)
{
   x.push_back(px);
   u.push_back(pu);
   ++np;
}

void Particles::
clear_particles()
{
	x.clear();
	u.clear();
	np = 0;
}

template<class T>
void Particles::
accumulate(T &accum, float q, int i, int j, float fx, float fy)
{
   float weight;

   weight=(1-fx)*(1-fy);
   accum(i,j)+=weight*q;
   sum(i,j)+=weight;

   weight=fx*(1-fy);
   accum(i+1,j)+=weight*q;
   sum(i+1,j)+=weight;

   weight=(1-fx)*fy;
   accum(i,j+1)+=weight*q;
   sum(i,j+1)+=weight;

   weight=fx*fy;
   accum(i+1,j+1)+=weight*q;
   sum(i+1,j+1)+=weight;
}

void Particles::
transfer_to_grid(void)
{
   int p, i, ui, j, vj;
   float fx, ufx, fy, vfy;

   grid.u.zero();
   sum.zero();
   for(p=0; p<np; ++p){
      grid.bary_x(x[p][0], ui, ufx);
      grid.bary_y_centre(x[p][1], j, fy);
      accumulate(grid.u, u[p][0], ui, j, ufx, fy);
   }
   for(j=0; j<grid.u.ny; ++j) for(i=0; i<grid.u.nx; ++i){
      if(sum(i,j)!=0) grid.u(i,j)/=sum(i,j);
   }

   grid.v.zero();
   sum.zero();
   for(p=0; p<np; ++p){
      grid.bary_x_centre(x[p][0], i, fx);
      grid.bary_y(x[p][1], vj, vfy);
      accumulate(grid.v, u[p][1], i, vj, fx, vfy);
   }
   for(j=0; j<grid.v.ny; ++j) for(i=0; i<grid.v.nx; ++i){
      if(sum(i,j)!=0) grid.v(i,j)/=sum(i,j);
   }

   // identify where particles are in grid
   grid.marker.zero();
   for(p=0; p<np; ++p){
      grid.bary_x(x[p][0], i, fx);
      grid.bary_y(x[p][1], j, fy);
      grid.marker(i,j)=FLUIDCELL;
   }
}

void Particles::
update_from_grid(void)
{
   int p;
   int i, ui, j, vj;
   float fx, ufx, fy, vfy;
   for(p=0; p<np; ++p){
      grid.bary_x(x[p][0], ui, ufx);
      grid.bary_x_centre(x[p][0], i, fx);
      grid.bary_y(x[p][1], vj, vfy);
      grid.bary_y_centre(x[p][1], j, fy);
      //u[p]+=Vec2f(grid.du.bilerp(ui, j, ufx, fy), grid.dv.bilerp(i, vj, fx, vfy)); // FLIP
      u[p]=Vec2f(grid.u.bilerp(ui, j, ufx, fy), grid.v.bilerp(i, vj, fx, vfy)); // PIC
   }
}

void Particles::
move_particles_in_grid(float dt)
{
	Vec2f midx, gu;
	float xmin=1.001*grid.h, xmax=grid.lx-1.001*grid.h;
	float ymin=1.001*grid.h, ymax=grid.ly-1.001*grid.h;
	for( int p = 0; p < np; ++p ) {
		// first stage of Runge-Kutta 2 (do a half Euler step)
		grid.bilerp_uv(x[p][0], x[p][1], gu[0], gu[1]);
		midx=x[p]+gu*(0.5f*dt);
		clamp(midx[0], xmin, xmax);
		clamp(midx[1], ymin, ymax);
		// second stage of Runge-Kutta 2
		grid.bilerp_uv(midx[0], midx[1], gu[0], gu[1]);
		x[p]+=dt*gu;
		clamp(x[p][0], xmin, xmax);
		clamp(x[p][1], ymin, ymax);
	}
}
