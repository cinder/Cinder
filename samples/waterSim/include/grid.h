#ifndef GRID_H
#define GRID_H

#include "array2.h"
#include "util.h"
#include "cinder/Vector.h"

#define AIRCELL 0
#define FLUIDCELL 1
#define SOLIDCELL 2

struct Grid{
	ci::Vec2f gravity;
	float lx, ly;
	float h, overh;

	// active variables
	Array2f u, v; // staggered MAC grid of velocities
	Array2f du, dv; // saved velocities and differences for particle update
	Array2c marker; // identifies what sort of cell we have
	Array2f phi; // decays away from water into air (used for extrapolating velocity)
	Array2d pressure;
	// stuff for the pressure solve
	Array2x3f poisson;
	Array2d preconditioner;
	Array2d m;
	Array2d r, z, s;

	Grid(void)
	{}

	Grid( ci::Vec2f gravity_, int cell_nx, int cell_ny, float lx_)
	{ init( gravity_, cell_nx, cell_ny, lx_); }

	void init( ci::Vec2f gravity_, int cell_nx, int cell_ny, float lx_);
	float CFL(void);
	void save_velocities(void);
	void add_gravity(float dt);
	void compute_distance_to_fluid(void);
	void extend_velocity(void);
	void apply_boundary_conditions(void);
	void make_incompressible(void);
	void get_velocity_update(void);

	void set_gravity( const ci::Vec2f &gravity_ ) { gravity = gravity_; }

	void bary_x(float x, int &i, float &fx)
	{
	  float sx=x*overh;
	  i=(int)sx;
	  //fx = sx - floor( sx );
	  fx = sx - int( sx );
	}

	void bary_x_centre(float x, int &i, float &fx)
	{
		float sx=x*overh-0.5;
		i=(int)sx;
		if(i<0){ i=0; fx=0.0; }
		else if(i>pressure.nx-2){ i=pressure.nx-2; fx=1.0; }
		else {
			//fx = sx - floor( sx );
			fx = sx - int( sx );
		}
	}

	void bary_y(float y, int &j, float &fy)
	{
		float sy=y*overh;
		j=(int)sy;
		//fy = sy - floor( sy );
		fy = sy - int( sy );
	}

	void bary_y_centre(float y, int &j, float &fy)
	{
	  float sy=y*overh-0.5;
	  j=(int)sy;
	  if(j<0){ j=0; fy=0.0; }
	  else if(j>pressure.ny-2){ j=pressure.ny-2; fy=1.0; }
	  else{
		//fy = sy-floor(sy); }
		fy = sy-int(sy); }
	}

	void bilerp_uv(float px, float py, float &pu, float &pv)
	{
	  int i, j;
	  float fx, fy;
	  bary_x(px, i, fx);
	  bary_y_centre(py, j, fy);
	  pu=u.bilerp(i, j, fx, fy);
	  bary_x_centre(px, i, fx);
	  bary_y(py, j, fy);
	  pv=v.bilerp(i, j, fx, fy);
	}

	private:
	void init_phi(void);
	void sweep_phi(void);
	void sweep_u(int i0, int i1, int j0, int j1);
	void sweep_v(int i0, int i1, int j0, int j1);
	void sweep_velocity(void);
	void find_divergence(void);
	void form_poisson(void);
	void form_preconditioner(void);
	void apply_poisson(const Array2d &x, Array2d &y);
	void apply_preconditioner(const Array2d &x, Array2d &y, Array2d &temp);
	void solve_pressure(int maxits, double tolerance);
	void add_gradient(void);
};

#endif

