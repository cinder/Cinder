#include "flint/Vector.h"
using fli::Vec2d;
using fli::Vec2f;

#include <list>
#include <vector>
#include <deque>
#include <queue>
#include <set>
#include <fstream>
using namespace std;

double signedArea (const Vec2d& p1, const Vec2d& p2);
double signedArea (const Vec2d& p0, const Vec2d& p1, const Vec2d& p2);

struct SweepEventComp : public binary_function<class SweepEvent *, class SweepEvent *, bool> {
	bool operator()(class SweepEvent *e1, class SweepEvent *e2);
};

struct SweepEventCompInv : public binary_function<class SweepEvent *, class SweepEvent *, bool> {
	bool operator()(class SweepEvent *e1, class SweepEvent *e2) { SweepEventComp comp; return ! comp( e1, e2 ); }
};

#if 1
template<typename _Tp, typename _Sequence = vector<_Tp>,
	   typename _Compare  = less<typename _Sequence::value_type> >
class my_priority_queue : public std::priority_queue<_Tp, _Sequence,_Compare> {
public:
	_Sequence& getC() { return priority_queue<_Tp, _Sequence,_Compare>::c; }
};

typedef std::list<class SweepEvent*> SweepEventList;

/*typedef std::priority_queue<class SweepEvent*, vector<class SweepEvent*>, SweepEventComp> PriorityQueue;

class SweepEvent* priorityQueuePop( PriorityQueue &pq )
{
	SweepEvent *e = pq.top(); pq.pop(); return e;
}

void priorityQueuePush( PriorityQueue &pq, class SweepEvent *e )
{
	pq.push( e );
}*/

typedef set<class SweepEvent*, class SweepEventCompInv> PriorityQueue;

SweepEvent* priorityQueuePop( PriorityQueue &pq )
{
	SweepEvent *e = *pq.begin();
	pq.erase( pq.begin() );
	return e;
}

void priorityQueuePush( PriorityQueue &pq, class SweepEvent *e )
{
	pq.insert( e );
}



template<typename _RandomAccessIterator, typename _Distance,
	   typename _StrictWeakOrdering, typename W>
bool regular_my_is_heap(_RandomAccessIterator __first, _StrictWeakOrdering __comp, _Distance __n, W w )
{
	_Distance __parent = 0;
	for (_Distance __child = 1; __child < __n; ++__child) {
		if (__comp(__first[__parent], __first[__child])) {
			std::cout << "Comparing to " << *(__first[__child]) << std::endl;
			for (_Distance __child2 = 0; __child2 < __child; ++__child2) {
				std::cout << *(__first[__child2]) << " : " << __comp(__first[__child], __first[__child2]) << std::endl;
			}
			
			// this should have returned false
			__comp(__first[__parent], __first[__child]);
			// instead, this will return false
			__comp(__first[__child], __first[__parent] );
			return false;
		}
		if ((__child & 1) == 0)
			++__parent;
	}
	return true;
}

template<typename _RandomAccessIterator, typename _Distance,
	   typename _StrictWeakOrdering>
bool ensureGreater(_RandomAccessIterator __first, _Distance __parent, _Distance __child, _StrictWeakOrdering __comp, _Distance n )
{
	if( __child >= n )
		return true;
		
	if( __comp( __first[__parent], __first[__child] ) ) {
		__comp( __first[__parent], __first[__child] );
		// this should have returned false
		return false;
	}
	if( ! ensureGreater( __first, __parent, __child * 2 + 1, __comp, n ) )
		return false;
	if( ! ensureGreater( __first, __parent, __child * 2 + 2, __comp, n ) )
		return false;
	
	return true;
}

template<typename _RandomAccessIterator, typename _Distance,
	   typename _StrictWeakOrdering, typename W>
bool my_is_heap(_RandomAccessIterator __first, _StrictWeakOrdering __comp, _Distance __n, W w )
{
	for( _Distance __parent = 0; __parent < __n; ++__parent ) {
		_Distance leftChild = __parent * 2 + 1;
		if( leftChild < __n )
			ensureGreater( __first, __parent, leftChild, __comp, __n );
		_Distance rightChild = __parent * 2 + 2;
		if( rightChild < __n )
			ensureGreater( __first, __parent, rightChild, __comp, __n );	
	}
	return true;
}

#endif

//#define _DEBUG_

const double MR_EPSILON = 0.000001;
bool epsilonEqual( const double &v1, const double &v2 ) 
{
	return fabs( v1 - v2 ) < MR_EPSILON;
}

bool epsilonEqual( const fli::Vec2d &v1, const fli::Vec2d &v2 )
{
	return epsilonEqual( v1.x, v2.x ) && epsilonEqual( v1.y, v2.y );
}

double distanceSquared( const fli::Vec2d &v1, const fli::Vec2d &v2 )
{
	return ( v2.x - v1.x ) * ( v2.x - v1.x ) + ( v2.y - v1.y ) * ( v2.y - v1.y );
}

class Segment {
public:
	/** Segment endVec2ds */
	Vec2d p1, p2;

public:
	/** Constructor from two Vec2ds **/
	Segment(const Vec2d &ap1, const Vec2d &ap2) : p1 (ap1), p2 (ap2) {}

	/** Get the beginning Vec2d */
	const Vec2d &begin() const { return p1; }
	/** Get the end Vec2d */
	const Vec2d &end() const { return p2; }
};

class Vec2dChain {
 public:
	typedef list<Vec2d>::iterator iterator;
	Vec2dChain () : l (), _closed (false) {}
	void init (const Segment& s);
	bool LinkSegment (const Segment& s);
	bool LinkVec2dChain (Vec2dChain& chain);
	bool closed () const { return _closed; }
	iterator begin () { return l.begin (); }
	iterator end () { return l.end (); }
	void clear () { l.clear (); }
	unsigned int size () const { return l.size (); }
 private:
	/** Cadena de puntos enlazados */
	list<Vec2d> l;
	bool _closed;
};

class Connector {
public:
	typedef list<Vec2dChain>::iterator iterator;
	Connector() : openPolygons (), closedPolygons () {}

	void add (const Segment& s);
	iterator begin () { return closedPolygons.begin (); }
	iterator end () { return closedPolygons.end (); }
	void clear () { closedPolygons.clear (); openPolygons.clear (); }
	size_t size () const { return closedPolygons.size (); }
	void toPolygon ( class Polygon& p);
private:
	/** @brief lista de polígonos abiertos */
	list<Vec2dChain> openPolygons;
	/** @brief lista de polígonos solución (cerrados) */
	list<Vec2dChain> closedPolygons;
};

class Contour {
	/** Set of segments conforming the contour */
	vector<Vec2d> Vec2ds;

public:
	typedef vector<Vec2d>::iterator iterator;

	/** Get the p-th vertex */
	Vec2d& vertex (unsigned p) { return Vec2ds[p]; }
	Segment segment (unsigned p) { return (p == nvertices () - 1) ? Segment (Vec2ds.back (), Vec2ds.front ()) : Segment (Vec2ds[p], Vec2ds[p+1]); }
	/** Number of vertices */
	size_t nvertices () { return Vec2ds.size (); }
	/** Get the bounding box */
	void boundingbox (Vec2d& min, Vec2d& max);

	void add (const Vec2d& s) { Vec2ds.push_back (s); }
	void pop_back() { Vec2ds.pop_back(); }
	void erase (iterator i) { Vec2ds.erase (i); }
	void clear () { Vec2ds.clear (); }
	const Vec2d& front() const { return Vec2ds.front(); }
	const Vec2d& back() const { return Vec2ds.back(); }	

	iterator begin () { return Vec2ds.begin (); }
	iterator end () { return Vec2ds.end (); }
};

class Polygon {
	/** Set of contours conforming the polygon */
	vector<Contour> contours;

public:
	typedef vector<Contour>::iterator iterator;
	
	Polygon () : contours () {}

	/** Get the p-th contour */
	Contour& contour (unsigned p) { return contours[p]; }
	/** Number of contours */
	size_t ncontours () const { return contours.size (); }
	/** Number of vertices */
	unsigned nvertices ();
	/** Get the bounding box */
	void boundingbox (Vec2d& min, Vec2d& max);

	Contour& pushbackContour () { contours.push_back (Contour ()); return contours.back (); }
	void deletebackContour () { contours.pop_back (); }
	void erase (iterator i) { contours.erase (i); }
	void clear () { contours.clear (); }

	iterator begin () { return contours.begin (); }
	iterator end () { return contours.end (); }
};

enum EdgeType { NORMAL, NON_CONTRIBUTING, SAME_SIGN, DIFFERENT_SIGN };
enum PolygonType { SUBJECT, CLIPPING };

struct SweepEvent {
	Vec2d p;    // Vec2d associated with the event
	bool left;  // is the Vec2d the left endVec2d of the segment (p, other->p)?
	PolygonType pl; // Polygon to which the associated segment belongs to
	SweepEvent *other; // Event associated to the other endVec2d of the segment
	/**  Does the segment (p, other->p) represent an inside-outside transition in the polygon for a ray from (p.x, -infinite) that crosses the segment at p? */
	bool inOut;
	EdgeType type;
	bool inside; // Only used in "left" events. Is the segment (p, other->p) inside the other polygon ?
	SweepEventList::iterator pos; // Only used in "left" events. Position of the event (segment) in S
	unsigned int uniqueId;

	/** Class constructor */
	SweepEvent (Vec2d pp, bool b, PolygonType apl, SweepEvent *o, EdgeType t = NORMAL) : p (pp), left (b), pl (apl), other (o), type (t) {}
	SweepEvent( const SweepEvent &a ) : p( a.p ), left( a.left ), pl( a.pl ), other( a.other ), inOut( a.inOut ), type( a.type ), inside( a.inside ) {}

	/** Return the segment associated to the SweepEvent */
	Segment segment() { return Segment (p, other->p); }

	/** Is the segment (p, other->p) below Vec2d x */
	bool below (const Vec2d& x) const { return (left) ? signedArea (p, other->p, x) > 0 : signedArea (other->p, p, x) > 0; }
	/** Is the segment (p, other->p) above Vec2d x */
	bool above (const Vec2d& x) const { return !below (x); }
};

struct SegmentCompList : public binary_function<SweepEvent *, SweepEvent *, bool> {
	bool operator()(SweepEvent *e1, SweepEvent *e2);
};

class MartinezRueda {
public:
	enum BoolOpType { INTERSECTION, UNION, DIFFERENCE };
	/** Class constructor */
	MartinezRueda( Polygon& sp, Polygon& cp, vector<Vec2f> *aRawIntersections ) : subject (sp), clipping (cp), nint (0), rawIntersections( aRawIntersections ) {}
	/** Compute the boolean operation */
	void computeList (BoolOpType op, Polygon& result);
	/** Number of intersections found (for statistics) */
	int nInt () const { return nint; }

	/** @brief Event Queue */
	PriorityQueue eq;
	/** @brief It holds the events generated during the computation of the boolean operation */
	deque<SweepEvent> eventHolder;
	/** @brief Polygon 1 */
	Polygon& subject;
	/** @brief Polygon 2 */
	Polygon& clipping;
	/** To compare events */
	SweepEventCompInv sec;
std::vector<fli::Vec2f> *rawIntersections;	
	/** @brief Number of intersections (for statistics) */
	int nint;
	/** @brief Compute the events associated to segment s, and insert them into pq */
	void processSegment (const Segment& s, PolygonType pl);
	/** @brief Process a posible intersection between the segment associated to the left events e1 and e2 */
	void possibleIntersection (SweepEvent *e1, SweepEvent *e2);
	/** @brief Divide the segment associated to left event e, updating pq and (implicitly) the status line */
	void divideSegment (SweepEvent *e, const Vec2d& p);
	static unsigned int nextUniqueId;
	/** @brief Store the SweepEvent e into the event holder, returning the address of e */
	SweepEvent *storeSweepEvent( const SweepEvent &e ) { eventHolder.push_back( e ); SweepEvent* result = &eventHolder.back(); result->uniqueId = nextUniqueId++; return result; }
};
unsigned int MartinezRueda::nextUniqueId = 0;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Vec2dChain::init (const Segment& s)
{
	l.push_back (s.begin ());
	l.push_back (s.end ());
}

bool Vec2dChain::LinkSegment (const Segment& s)
{
	if (s.begin () == l.front ()) {
		if (s.end () == l.back ())
			_closed = true;
		else
			l.push_front (s.end ());
		return true;
	}
	if (s.end () == l.back ()) {
		if (s.begin () == l.front ())
			_closed = true;
		else
			l.push_back (s.begin ());
		return true;
	}
	if (s.end () == l.front ()) {
		if (s.begin () == l.back ())
			_closed = true;
		else
			l.push_front (s.begin ());
		return true;
	}
	if (s.begin () == l.back ()) {
		if (s.end () == l.front ())
			_closed = true;
		else
			l.push_back (s.end ());
		return true;
	}
	return false;
}

bool Vec2dChain::LinkVec2dChain (Vec2dChain& chain)
{
	if (chain.l.front () == l.back ()) {
		chain.l.pop_front ();
		l.splice (l.end (), chain.l);
		return true;
	}
	if (chain.l.back () == l.front ()) {
		l.pop_front ();
		l.splice (l.begin (), chain.l);
		return true;
	}
	if (chain.l.front () == l.front ()) {
		l.pop_front ();
		reverse (chain.l.begin (), chain.l.end ());
		l.splice (l.begin (), chain.l);
		return true;
	}
	if (chain.l.back () == l.back ()) {
		l.pop_back ();
		reverse (chain.l.begin (), chain.l.end ());
		l.splice (l.end (), chain.l);
		return true;
	}
	return false;
}

void Connector::add(const Segment& s)
{
	iterator j = openPolygons.begin ();
	while (j != openPolygons.end ()) {
		if (j->LinkSegment (s)) {
			if (j->closed ())
				closedPolygons.splice (closedPolygons.end (), openPolygons, j);
			else {
				list<Vec2dChain>::iterator k = j;
				for (++k; k != openPolygons.end (); k++) {
					if (j->LinkVec2dChain (*k)) {
						openPolygons.erase (k);
						break;
					}
				}
			}
			return;
		}
		j++;
	}
	// The segment cannot be connected with any open polygon
	openPolygons.push_back (Vec2dChain ());
	openPolygons.back ().init (s);
}

void Connector::toPolygon (Polygon& p)
{
	for (iterator it = begin (); it != end (); it++) {
		Contour& contour = p.pushbackContour ();
		for (Vec2dChain::iterator it2 = it->begin (); it2 != it->end (); it2++)
			contour.add (*it2);
	}
}

void Contour::boundingbox (Vec2d& min, Vec2d& max)
{
	min.x = min.y = numeric_limits<double>::max ();
	max.x = max.y = -numeric_limits<double>::max ();
	Contour::iterator i = begin();
	while (i != end()) {
		if (i->x < min.x)
			min.x = i->x;
		if (i->x > max.x)
			max.x = i->x;
		if (i->y < min.y)
			min.y = i->y;
		if (i->y > max.y)
			max.y = i->y;
		++i;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned Polygon::nvertices ()
{
	unsigned int nv = 0;
	for (unsigned int i = 0; i < ncontours (); i++)
		nv += contours[i].nvertices ();
	return nv;
}

void Polygon::boundingbox (Vec2d& min, Vec2d& max)
{
	min.x = min.y = numeric_limits<double>::max ();
	max.x = max.y = -numeric_limits<double>::max ();
	Vec2d mintmp;
	Vec2d maxtmp;
	for (unsigned int i = 0; i < ncontours (); i++) {
		contours[i].boundingbox (mintmp, maxtmp);
		if (mintmp.x < min.x)
			min.x = mintmp.x;
		if (maxtmp.x > max.x)
			max.x = maxtmp.x;
		if (mintmp.y < min.y)
			min.y = mintmp.y;
		if (maxtmp.y > max.y)
			max.y = maxtmp.y;
	}
}

inline double signedArea (const Vec2d& p0, const Vec2d& p1, const Vec2d& p2)
{ 
	return (p0.x - p2.x)*(p1.y - p2.y) - (p1.x - p2.x) * (p0.y - p2.y);
}

/** Signed area of the triangle ( (0,0), p1, p2) */
inline double signedArea (const Vec2d& p1, const Vec2d& p2)
{ 
	return -p2.x*(p1.y - p2.y) - -p2.y*(p1.x - p2.x);
}

/** Sign of triangle (p1, p2, o) */
inline int sign (const Vec2d& p1, const Vec2d& p2, const Vec2d& o)
{
	double det = (p1.x - o.x) * (p2.y - o.y) - (p2.x - o.x) * (p1.y - o.y);
	return (det < 0 ? -1 : (det > 0 ? +1 : 0));
}

inline bool Vec2dInTriangle (Segment& s, Vec2d& o, Vec2d& p)
{
	int x = sign (s.begin (), s.end (), p);
	return ((x == sign (s.end (), o, p)) && (x == sign (o, s.begin (), p)));
}

static int findIntersection (double u0, double u1, double v0, double v1, double w[2])
{
	if ((u1 < v0) || (u0 > v1))
		return 0;
	if (u1 > v0) {
		if (u0 < v1) {
			w[0] = (u0 < v0) ? v0 : u0;
			w[1] = (u1 > v1) ? v1 : u1;
			return 2;
		} else {
			// u0 == v1
			w[0] = u0;
			return 1;
		}
	} else {
		// u1 == v0
		w[0] = u1;
		return 1;
	}
}

int findIntersection (const Segment &seg0, const Segment &seg1, Vec2d &pi0, Vec2d &pi1)
{
	const double EPSILON = 0.00000001;
	const Vec2d& p0 = seg0.begin ();
	Vec2d d0 (seg0.end ().x - p0.x, seg0.end ().y - p0.y);
	const Vec2d& p1 = seg1.begin ();
	Vec2d d1 (seg1.end ().x - p1.x, seg1.end ().y - p1.y);
	double sqrEpsilon = EPSILON * 0.1; // Antes 0.001
	Vec2d E (p1.x - p0.x, p1.y - p0.y);
	double kross = d0.x * d1.y - d0.y * d1.x;
	double sqrKross = kross * kross;
	double sqrLen0 = d0.x * d0.x + d0.y * d0.y;
	double sqrLen1 = d1.x * d1.x + d1.y * d1.y;

	if (sqrKross > sqrEpsilon * sqrLen0 * sqrLen1) {
		// lines of the segments are not parallel
		double s = (E.x * d1.y - E.y * d1.x) / kross;
		if ((s < 0) || (s > 1)) {
			return 0;
		}
		double t = (E.x * d0.y - E.y * d0.x) / kross;
		if ((t < 0) || (t > 1)) {
			return 0;
		}
		// intersection of lines is a Vec2d an each segment
		pi0.x = p0.x + s * d0.x;
		pi0.y = p0.y + s * d0.y;
		if (pi0.distance(seg0.begin ()) < EPSILON) pi0 = seg0.begin ();
		if (pi0.distance(seg0.end ()) < EPSILON) pi0 = seg0.end ();
		if (pi0.distance(seg1.begin ()) < EPSILON) pi0 = seg1.begin ();
		if (pi0.distance(seg1.end ()) < EPSILON) pi0 = seg1.end ();
		return 1;
	}

	// lines of the segments are parallel
	double sqrLenE = E.x * E.x + E.y * E.y;
	kross = E.x * d0.y - E.y * d0.x;
	sqrKross = kross * kross;
	if (sqrKross > sqrEpsilon * sqrLen0 * sqrLenE) {
		// lines of the segment are different
		return 0;
	}

	// Lines of the segments are the same. Need to test for overlap of segments.
	double s0 = (d0.x * E.x + d0.y * E.y) / sqrLen0;  // so = Dot (D0, E) * sqrLen0
	double s1 = s0 + (d0.x * d1.x + d0.y * d1.y) / sqrLen0;  // s1 = s0 + Dot (D0, D1) * sqrLen0
	double smin = min (s0, s1);
	double smax = max (s0, s1);
	double w[2];
	int imax = findIntersection (0.0, 1.0, smin, smax, w);

	if (imax > 0) {
		pi0.x = p0.x + w[0] * d0.x;
		pi0.y = p0.y + w[0] * d0.y;
		if( pi0.distance(seg0.begin ()) < EPSILON ) pi0 = seg0.begin();
		if( pi0.distance(seg0.end ()) < EPSILON ) pi0 = seg0.end();
		if( pi0.distance(seg1.begin ()) < EPSILON ) pi0 = seg1.begin();
		if( pi0.distance(seg1.end ()) < EPSILON ) pi0 = seg1.end();
		if( imax > 1) {
			pi1.x = p0.x + w[1] * d0.x;
			pi1.y = p0.y + w[1] * d0.y;
		}
	}

	return imax;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// #define _DEBUG_ // uncomment this if you want to debug the computation of the boolean operation, or just to see how the algorithm proceeds

ostream &operator<<(ostream &o, SweepEvent &e)
{
/*	char *namesEventTypes[] = { " (NORMAL) ", " (NON_CONTRIBUTING) ", " (SAME_SIGN) ", " (DIFFERENT_SIGN) " };
	return o << " Vec2d: " << e.p << " Other Vec2d: " << e.other->p << (e.left ? " (Left) " : " (Right) ")
            << (e.inside ? " (Inside) " : " (Outside) ") <<  (e.inOut ? " (In-Out) " : " (Out-In) ") << "Type: "
            << namesEventTypes[e.type] << " Polygon: " << (e.pl == SUBJECT ? " (SUBJECT)" : " (CLIPPING)");*/
	return o << e.p << "(" << e.uniqueId << ")" << ((e.left)?"->":"<-") << e.other->p << "(" << e.other->uniqueId << ")";
}

// Compare two sweep events
// Return true means that e1 is placed at the event queue after e2, i.e,, e1 is processed by the algorithm after e2
bool SweepEventComp::operator()( SweepEvent *e1, SweepEvent *e2 ) {
	if (e1->p.x > e2->p.x) // Different x coordinate
		return true;
	else if (e2->p.x > e1->p.x) // Different x coordinate
		return false;
	else if( e1->p.y > e2->p.y )
		return true;
	else if( e2->p.y > e1->p.y )
		return false;

	if( e1->left != e2->left ) // Same Vec2d, but one is a left endVec2d and the other a right one. The right one is processed first
		return e1->left;

	// Same Vec2d, both events are left endVec2ds or both are right endVec2ds. The event associate to the bottom segment is processed first
	double sa = ( e1->left) ? signedArea( e1->p, e1->other->p, e2->other->p ) : signedArea( e1->other->p, e1->p, e2->other->p );
	if( sa < 0 )
		return true;
	else if( sa > 0 )
		return false;
	else
		return e1->uniqueId > e2->uniqueId;
}

bool SegmentCompList::operator()(SweepEvent *e1, SweepEvent *e2) {
	if (signedArea (e1->p, e1->other->p, e2->other->p) != 0 || signedArea (e1->p, e1->other->p, e2->p) != 0) {
		// Segments are not collinear
		// If they share their left endVec2d use the right endVec2d to sort
		if (e1->p == e2->p)
			return e1->below (e2->other->p);
		
		// Different Vec2ds
		// The segment associated to e2 has been inserted into S after the segment associated to e1
		return e1->below (e2->p);
	}
	// Segments are collinear
	if (e1->p == e2->p)
		return e1 < e2;
	SweepEventComp comp;
	return comp (e1, e2);
}

void MartinezRueda::computeList (BoolOpType op, Polygon& result)
{
	// Test for trivial result cases
	if (subject.ncontours () * clipping.ncontours () == 0) { // At least one of the polygons is empty
		if (op == DIFFERENCE)
			result = subject;
		if (op == UNION)
			result = (subject.ncontours () == 0) ? clipping : subject;
		return;
	}
	Vec2d minsubj, maxsubj, minclip, maxclip;
	subject.boundingbox (minsubj, maxsubj);
	clipping.boundingbox (minclip, maxclip);
	if (minsubj.x > maxclip.x || minclip.x > maxsubj.x || minsubj.y > maxclip.y || minclip.y > maxsubj.y) {
		// the bounding boxes do not overlap
		if (op == DIFFERENCE)
			result = subject;
		if (op == UNION) {
			result = subject;
			for (unsigned int i = 0; i < clipping.ncontours (); i++) {
				Contour& c = result.pushbackContour ();
				c = clipping.contour (i);
			}
		}
		return;
	}

	// Boolean operation is not trivial

	// Insert all the endVec2ds associated to the segments into the event queue
	for (unsigned int i = 0; i < subject.ncontours (); i++)
		for (unsigned int j = 0; j < subject.contour (i).nvertices (); j++)
			processSegment(subject.contour (i).segment (j), SUBJECT);
	for (unsigned int i = 0; i < clipping.ncontours (); i++)
		for (unsigned int j = 0; j < clipping.contour (i).nvertices (); j++)
			processSegment(clipping.contour (i).segment (j), CLIPPING);

	Connector connector;  // to connect the edge solutions
	SegmentCompList s;    // to compare the segments, and insert them into S
	SweepEvent *e;        // the event to be processed
	list<SweepEvent *> S; // Status line
	list<SweepEvent *>::iterator it, sli, prev, next;

	double minmaxx = std::min (maxsubj.x, maxclip.x); // for optimization 1

	while( ! eq.empty() ) {
		e = priorityQueuePop( eq );
		
		#ifdef _DEBUG_
		cout << "Process event: " << *e << endl;
		#endif

		if ((op == INTERSECTION && (e->p.x > minmaxx)) || (op == DIFFERENCE && e->p.x > maxsubj.x)) { // optimization 1
			connector.toPolygon (result);
			return;
		}
		if ((op == UNION && (e->p.x > minmaxx))) { // optimization 1
			// add all the non-processed segments to the result
			if (!e->left)
				connector.add (e->segment ());
			while( ! eq.empty() ) {
				e = priorityQueuePop( eq );
				if (!e->left)
					connector.add (e->segment ());
			}
			connector.toPolygon (result);
			return;
		}

		if (e->left) { // the segment associated to e is a left endVec2d and must be inserted into S
			e->inside = e->inOut = 0;
			for (it = S.begin (); it != S.end () && s(*it, e); it++)
				if ((*it)->pl != e->pl)
					e->inside ^= 1;
				else
					e->inOut ^= 1;
			it = S.insert (it, e);
			e->pos = it;
			#ifdef _DEBUG_
			cout << "After insertion: " << endl;
			for (list<SweepEvent *>::const_iterator it2 = S.begin(); it2 != S.end(); it2++)
				cout << **it2 << endl;
			#endif

			// Process a possible intersection with the next segment in S (the segment above the segment associated to e)
			next = prev = it;
			if ((++next) != S.end())
				possibleIntersection(e, *next);

			// Process a possible intersection with the previous segment in S (the segment below the segment associated to e)
			if (prev != S.begin())
				possibleIntersection(e, *(--prev));
		}
		else { // the segment associated to e is a right endVec2d and must be removed from S
			next = prev = sli = e->other->pos;

			// Get the next and previous segments to e in S
			++next;
			(prev != S.begin()) ? --prev : prev = S.end();

			// Compute if the segment associated t e belongs to the boolean opearation
			switch (e->type) {
				case (NORMAL):
					switch (op) {
						case (INTERSECTION):
							if (e->other->inside)
								connector.add (e->segment ());
							break;
						case (UNION):
							if (!e->other->inside)
								connector.add (e->segment ());
							break;
						case (DIFFERENCE):
							if (((e->pl == SUBJECT) && (!e->other->inside)) || (e->pl == CLIPPING && e->other->inside))
								connector.add (e->segment ());
							break;
					}
					break;
				case (SAME_SIGN):
					if (op == INTERSECTION || op == UNION)
						connector.add (e->segment ());
					break;
				case (DIFFERENT_SIGN):
					if (op == DIFFERENCE)
						connector.add (e->segment ());
					break;
			}

			// The segment associated to e is erased from S
			assert (sli != S.end());
			S.erase (sli);

			// Check for possible intersection between the "neighbors" of the deleted segment in S
			if (next != S.end() && prev != S.end())
				possibleIntersection (*next, *prev);
		}

		#ifdef _DEBUG_
		cout << "After processing: " << endl;
		for (list<SweepEvent *>::const_iterator it2 = S.begin(); it2 != S.end(); it2++)
			cout << **it2 << endl;
		cout << endl;
		#endif
	}
	connector.toPolygon (result);
}

void MartinezRueda::processSegment (const Segment& s, PolygonType pl)
{
	SweepEvent *e1 = storeSweepEvent(SweepEvent(s.begin(), true, pl, 0));
	SweepEvent *e2 = storeSweepEvent(SweepEvent(s.end(), true, pl, e1));
	e1->other = e2;

	if (e1->p.x < e2->p.x) {
		e2->left = false;
	} else if (e1->p.x > e2->p.x) {
		e1->left = false;
	} else if (e1->p.y < e2->p.y) { // the segment is vertical. The bottom endVec2d is the left endVec2d 
		e2->left = false;
	} else {
		e1->left = false;
	}
	priorityQueuePush( eq, e1 );
	priorityQueuePush( eq, e2 );
}

void MartinezRueda::possibleIntersection (SweepEvent *e1, SweepEvent *e2)
{
	if ((e1->pl == e2->pl) ) // you can uncomment these two lines if self-intersecting polygons are not allowed
		return;

	Vec2d ip1, ip2;  // intersection Vec2ds
	int nintersections;

	if (!(nintersections = findIntersection(e1->segment (), e2->segment (), ip1, ip2)))
		return;

/// TODO
//	if ((nintersections == 1) && ( (e1->p == e2->p) || (e1->other->p == e2->other->p)))
	if( ( nintersections == 1 ) && ( epsilonEqual( e1->p, e2->p ) || ( epsilonEqual( e1->other->p, e2->other->p ) ) ) )
		return; // the segments only intersect at an endVec2d


	if( ( nintersections == 2 ) && ( e1->pl == e2->pl ) )
		return; // the segments overlap, but belong to the same polygon

	// The segments associated to e1 and e2 intersect
	nint += nintersections;

	if (nintersections == 1) {
		if( e1->p != ip1 && e1->other->p != ip1 ) {  // if ip1 is not an endVec2d of the segment associated to e1 then divide "e1"
			divideSegment (e1, ip1);
rawIntersections->push_back( ip1 );			
		}
		if( e2->p != ip1 && e2->other->p != ip1 ) { // if ip1 is not an endVec2d of the segment associated to e2 then divide "e2"
			divideSegment (e2, ip1);
rawIntersections->push_back( ip1 );		
		}
		return;
	}

	// The segments overlap
	vector<SweepEvent *> sortedEvents;
	if (e1->p == e2->p) {
		sortedEvents.push_back (0);
	} else if( sec( e1, e2 ) ) {
		sortedEvents.push_back (e2);
		sortedEvents.push_back (e1);
	} else {
		sortedEvents.push_back (e1);
		sortedEvents.push_back (e2);
	}
	if (e1->other->p == e2->other->p) {
		sortedEvents.push_back (0);
	} else if (sec (e1->other, e2->other)) {
		sortedEvents.push_back (e2->other);
		sortedEvents.push_back (e1->other);
	} else {
		sortedEvents.push_back (e1->other);
		sortedEvents.push_back (e2->other);
	}

	if (sortedEvents.size () == 2) { // are both segments equal?
		e1->type = e1->other->type = NON_CONTRIBUTING;
		e2->type = e2->other->type = (e1->inOut == e2->inOut) ? SAME_SIGN : DIFFERENT_SIGN;
		return;
	}
	if (sortedEvents.size () == 3) { // The segments share an endVec2d
		sortedEvents[1]->type = sortedEvents[1]->other->type = NON_CONTRIBUTING;
		if (sortedEvents[0])         // is the right endVec2d the shared Vec2d?
			sortedEvents[0]->other->type = (e1->inOut == e2->inOut) ? SAME_SIGN : DIFFERENT_SIGN;
		 else 								// the shared Vec2d is the left endVec2d
			sortedEvents[2]->other->type = (e1->inOut == e2->inOut) ? SAME_SIGN : DIFFERENT_SIGN;
		divideSegment (sortedEvents[0] ? sortedEvents[0] : sortedEvents[2]->other, sortedEvents[1]->p);
rawIntersections->push_back( sortedEvents[1]->p );
		return;
	}
	if (sortedEvents[0] != sortedEvents[3]->other) { // no segment includes totally the other one
		sortedEvents[1]->type = NON_CONTRIBUTING;
		sortedEvents[2]->type = (e1->inOut == e2->inOut) ? SAME_SIGN : DIFFERENT_SIGN;
		divideSegment (sortedEvents[0], sortedEvents[1]->p);
rawIntersections->push_back( sortedEvents[1]->p );
		divideSegment (sortedEvents[1], sortedEvents[2]->p);
rawIntersections->push_back( sortedEvents[2]->p );		
		return;
	}
	 // one segment includes the other one
	sortedEvents[1]->type = sortedEvents[1]->other->type = NON_CONTRIBUTING;
	divideSegment (sortedEvents[0], sortedEvents[1]->p);
rawIntersections->push_back( sortedEvents[1]->p );
	sortedEvents[3]->other->type = (e1->inOut == e2->inOut) ? SAME_SIGN : DIFFERENT_SIGN;
	divideSegment (sortedEvents[3]->other, sortedEvents[2]->p);
rawIntersections->push_back( sortedEvents[2]->p );
}

void MartinezRueda::divideSegment( SweepEvent *e, const Vec2d& p )
{
	// "Right event" of the "left segment" resulting from dividing e (the segments associated to e)
	SweepEvent *r = storeSweepEvent(SweepEvent(p, false, e->pl, e, e->type));
	// "Left event" of the "right segment" resulting from dividing e (the segments associated to e)
	SweepEvent *l = storeSweepEvent(SweepEvent(p, true, e->pl, e->other, e->other->type));
	SweepEventComp comp;
	if (comp (l, e->other)) { // avoid a rounding error. The left event would be processed after the right event
		e->other->left = true;
		l->left = false;
	}
	e->other->other = l;
	e->other = r;
	priorityQueuePush( eq, l );
	priorityQueuePush( eq, r );
}

vector<vector<Vec2f> > calculateIntersection( const vector<vector<Vec2f> > &subject, const vector<vector<Vec2f> > &clip, int mode, std::vector<fli::Vec2f> *rawIntersections )
{
	Polygon subjectPoly, clipPoly, resultPoly;
	Vec2f lastPt;

	MartinezRueda::BoolOpType op;
	mode %= 3;
	if( mode == 0 ) op = MartinezRueda::INTERSECTION;
	else if( mode == 1 ) op = MartinezRueda::UNION;
	else op = MartinezRueda::DIFFERENCE;
	
	for( vector<vector<Vec2f> >::const_iterator contourIt = subject.begin(); contourIt != subject.end(); ++contourIt ) {
		if( contourIt->size() <= 2 )
			continue;
		Contour& contour = subjectPoly.pushbackContour ();
		for( vector<Vec2f>::const_iterator ptIt = contourIt->begin(); ptIt != contourIt->end(); ++ptIt ) {
			if( ( ptIt == contourIt->begin() ) || ( *ptIt != lastPt ) ) {
				contour.add( Vec2d( *ptIt ) );
				lastPt = *ptIt;
			}
			else
				std::cout << "Skipping " << *ptIt;
		}
		if( contour.front() == contour.back() ) {
			std::cout << "Popping " << contour.back();
			contour.pop_back();
		}
	}

	for( vector<vector<Vec2f> >::const_iterator contourIt = clip.begin(); contourIt != clip.end(); ++contourIt ) {
		if( contourIt->size() <= 2 )
			continue;
		Contour& contour = clipPoly.pushbackContour ();
		for( vector<Vec2f>::const_iterator ptIt = contourIt->begin(); ptIt != contourIt->end(); ++ptIt ) {
			if( ( ptIt == contourIt->begin() ) || ( *ptIt != lastPt ) ) {
				contour.add( Vec2d( *ptIt ) );
				lastPt = *ptIt;
			}
			else
				std::cout << "Skipping " << *ptIt;
		}
		if( contour.front() == contour.back() ) {
			std::cout << "Popping " << contour.back();
			contour.pop_back();
		}
	}
	
	MartinezRueda mr( subjectPoly, clipPoly, rawIntersections );
	mr.computeList( op, resultPoly );
std::cout << "Int: " << mr.nint << std::endl;	
	vector<vector<Vec2f> > result;
	for( Polygon::iterator contourIt = resultPoly.begin(); contourIt != resultPoly.end(); ++contourIt ) {
		vector<Vec2f> resultChain;
		for( Contour::iterator ptIt = contourIt->begin(); ptIt != contourIt->end(); ++ptIt ) {
			resultChain.push_back( Vec2f( *ptIt ) );
		}
		result.push_back( resultChain );
	}
	
	return result;
}

std::vector<std::vector<fli::Vec2f> > loadPolygon( const std::string &path, float scale )
{
	std::ifstream f( path.c_str() );
	double px, py;

	vector<vector<Vec2f> > result;
	assert (f != NULL);
	int ncontours;
	f >> ncontours;
	for( int i = 0; i < ncontours; i++ ) {
		int npoints, level;
		f >> npoints >> level;
		vector<Vec2f> contour;
		for (int j = 0; j < npoints; j++) {
			f >> px; f >> py;
			contour.push_back( Vec2f( px, py ) * scale );
		}
		
		if( contour.size() >= 3 )
			result.push_back( contour );
	}	
	
	return result;
}
