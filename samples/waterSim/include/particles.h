#ifndef PARTICLES_H
#define PARTICLES_H

#include <vector>
#include "grid.h"
//#include "vec2.h"
#include "cinder/Vector.h"

struct Particles{
	Grid &grid;
	int np; // number of particles
	std::vector<ci::Vec2f> x, u; // positions and velocities
	// transfer stuff
	Array2f sum;

	Particles(Grid &grid_)
	  :grid(grid_), np(0),
	   sum(grid_.pressure.nx+1, grid_.pressure.ny+1)
	{}

	void add_particle(const ci::Vec2f &px, const ci::Vec2f &pu);
	void clear_particles();
	void transfer_to_grid(void);
	void update_from_grid(void);
	void move_particles_in_grid(float dt);

	private:
	template<class T> void accumulate(T &accum, float q, int i, int j, float fx, float fy);
};

#endif
