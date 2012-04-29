//
//  qbPolyTriangulated.cpp
//
//  Created by Roger Sodre on 07/02/2012
//  Copyright 2012 Studio Avante. All rights reserved.
//

#import "cinder/Cinder.h"
#import "qbPoly.h"



namespace cinder { namespace qb {
	
	typedef struct {
		int p1,p2,p3;
	} ITRIANGLE;
	typedef struct {
		int p1,p2;
	} IEDGE;
	
	//
	// TRIANGULATION
	//
	class qbPolyTriangulated : public qb::qbPolyGroup {
	public:
		
		qbPolyTriangulated() {};
		
		int triangulateFromPolyGroup( qb::qbPolyGroup & svg );
		int triangulateFromPoints( std::vector<Vec3f> & points );
		
		
	private:
		
		int Triangulate(int nv, Vec3f *pxyz,ITRIANGLE *v,int *ntri);
		int CircumCircle(double xp,double yp,
						 double x1,double y1,
						 double x2,double y2,
						 double x3,double y3,
						 double *xc,double *yc,double *rsqr);
		
	};
	
	
} } // cinder::qb
