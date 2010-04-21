#include "PolyBoolean.h"

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <ctype.h>
#include <math.h>
#include <list>
using std::vector;
using std::list;
using namespace fli;
#include "flint/Flint.h"

bool lineIntersection( const Vec2f &p1, const Vec2f &p2, const Vec2f &q1, const Vec2f &q2, float *alpha_p, float *alpha_q, Vec2f *intersection );

struct node
{
	node( const Vec2f &aPos )
		: pos( aPos ), next( 0 ), prev( 0 ), neighbor( 0 )
	{
		isIntersection = entry = visited = false;
		alpha = 0.0f;
	}

	node( const Vec2f &aPos, float aAlpha )
		: pos( aPos ), alpha( aAlpha ), next( 0 ), prev( 0 ), neighbor( 0 )
	{
		isIntersection = true;
		entry = visited = false;
	}
	
	Vec2f	pos;
	node	*next, *prev;
	node	*neighbor;   // the coresponding intersection point
	bool	isIntersection;
	bool	entry;
	bool	visited;
	float	alpha;              // intersection point placemet relative to adjacent non-intersection points
};

class PolyContour {
 public:
	PolyContour( const vector<Vec2f> &points )
		: head( 0 )
	{
		for( vector<Vec2f>::const_iterator ptIt = points.begin(); ptIt != points.end(); ++ptIt )
			append( new node( *ptIt ) );
	}

	~PolyContour()
	{
		node *it = head;
		while( true ) {
			node *next = it->next;
			delete it;
			if( next != head )
				it = next;
			else
				break;
		}
	}
	
	node* front() const { return head; }
	
	void append( node *newNode )
	{
		if( ! head ) {
			head = newNode;
		}
		else {
			node *it = head;
			while( it->next )
				it = it->next;
			it->next = newNode;
			newNode->prev = it;
		}
	}
	
	int testInsideCrossings( const Vec2f &point )
	{
		int crossings = 0;
		for( node* it = head; it->next; it = it->next ) {
			float dummy; Vec2f dummyP;
			if( lineIntersection( Vec2f( 0, point.y ), point, it->pos, it->next->pos, &dummy, &dummy, &dummyP ) )
				crossings++;
		}
		return crossings;
	}
	
	void close()
	{
		node *it = head;
		while( it->next )
			it = it->next;
		head->prev = it;
		it->next = head;
	}
	
	node* firstUnvisitedIntersection()
	{
		node *it = head->next;
		while( it && ( it != head ) && ( ( ! it->isIntersection ) || ( it->visited ) ) )
			it = it->next;
		
		if( it == head )
			return 0;
		else
			return it;
	}
	
	void insertIntersection( node *after, node *before, node *ins )
	{
		node *it = after;
		while( ( it != before ) && ( it->alpha < ins->alpha ) )
			it = it->next;
		ins->next = it;
		ins->prev = it->prev;
		ins->prev->next = ins;
		ins->next->prev = ins;	
	}		

	node* nextNonIntersection( node *startIt )
	{
		node* it = startIt->next;
		while( it && it->isIntersection )
			it = it->next;
		return it;
	}

 private:
	node	*head;	
};

class Poly {
 public:
	Poly( const vector<vector<Vec2f> > &contours )
	{
		for( vector<vector<Vec2f> >::const_iterator contourIt = contours.begin(); contourIt != contours.end(); ++contourIt ) {
			shared_ptr<PolyContour> cont( new PolyContour( *contourIt ) );
			mContours.push_back( cont );
		}
	}
	
	vector<shared_ptr<PolyContour> >::iterator contoursBegin() { return mContours.begin(); }
	vector<shared_ptr<PolyContour> >::iterator contoursEnd() { return mContours.end(); }	
	
	void loopContours()
	{
		for( vector<shared_ptr<PolyContour> >::iterator ctIt = mContours.begin(); ctIt != mContours.end(); ++ctIt )
			(*ctIt)->append( new node( (*ctIt)->front()->pos ) );
	}
	
	bool testInside( const Vec2f &point )
	{
		int crossings = 0;
		for( vector<shared_ptr<PolyContour> >::iterator ctIt = mContours.begin(); ctIt != mContours.end(); ++ctIt )
			crossings += (*ctIt)->testInsideCrossings( point );
		return crossings % 2;
	}
	
	void closeContours()
	{
		for( vector<shared_ptr<PolyContour> >::iterator ctIt = mContours.begin(); ctIt != mContours.end(); ++ctIt )
			(*ctIt)->close();
	}
	
 private:
	vector<shared_ptr<PolyContour> >		mContours;
};


bool lineIntersection( const Vec2f &p1, const Vec2f &p2, const Vec2f &q1, const Vec2f &q2, float *alpha_p, float *alpha_q, Vec2f *intersection )
{
	float tp, tq, par;
	Vec2f pos;

	par = ( p2.x - p1.x ) * ( q2.y - q1.y ) - ( p2.y - p1.y ) * ( q2.x - q1.x );
 
	if( par == 0.0f )
		return false;                               // parallel lines

	float invPar = 1.0f / par;
	tp = ((q1.x - p1.x)*(q2.y - q1.y) - (q1.y - p1.y)*(q2.x - q1.x)) * invPar;
	tq = ((p2.y - p1.y)*(q1.x - p1.x) - (p2.x - p1.x)*(q1.y - p1.y)) * invPar;

	if( tp < 0 || tp > 1 || tq < 0 || tq > 1 )
		return false;

	pos = p1 + tp * ( p2 - p1 );

	float invDistP = 1.0f / p1.distance( p2 );
	float invDistQ = 1.0f / q1.distance( q2 );
	*alpha_p = p1.distance( pos ) * invDistP;
	*alpha_q = q1.distance( pos ) * invDistQ;
	*intersection = pos;
	
	return true;
}

void processIntersections( PolyContour &subject, PolyContour &clip )
{
	Vec2f iPos;
	float alpha_s, alpha_c;

	for( node *subjectIt = subject.front(); subjectIt; subjectIt = subjectIt->next ) {
		if( ! subjectIt->isIntersection ) {
			for( node *clipIt = clip.front(); clipIt; clipIt = clipIt->next ) {
				if( ! clipIt->isIntersection ) {
					node *nextSubject = subject.nextNonIntersection( subjectIt );
					node *nextClip = clip.nextNonIntersection( clipIt );
					if( nextSubject && nextClip && lineIntersection( subjectIt->pos, nextSubject->pos, clipIt->pos, nextClip->pos, &alpha_s, &alpha_c, &iPos ) ) {
						node *is = new node( iPos, alpha_s );
						node *ic = new node( iPos, alpha_c );
						is->neighbor = ic;
						ic->neighbor = is;
						subject.insertIntersection( subjectIt, nextSubject, is );
						clip.insertIntersection( clipIt, nextClip, ic );
					}					
				}
			}
		}
	}
}

vector<vector<Vec2f> > clipPolys( Poly &subject, Poly &clip, int pS, int pC )
{
	bool e;

	// close the loop of both polys by adding a terminal node that connects to the first
	subject.loopContours();
	clip.loopContours();
	
	for( vector<shared_ptr<PolyContour> >::iterator subjectContourIt = subject.contoursBegin(); subjectContourIt != subject.contoursEnd(); ++subjectContourIt ) {
		for( vector<shared_ptr<PolyContour> >::iterator clipContourIt = clip.contoursBegin(); clipContourIt != clip.contoursEnd(); ++clipContourIt ) {
			processIntersections( **subjectContourIt, **clipContourIt );
		}
	}
	
	for( vector<shared_ptr<PolyContour> >::iterator subjectContourIt = subject.contoursBegin(); subjectContourIt != subject.contoursEnd(); ++subjectContourIt ) {
		e = clip.testInside( (*subjectContourIt)->front()->pos );
		if( pS )
			e = ! e;
		for( node *subjectIt = (*subjectContourIt)->front(); subjectIt; subjectIt = subjectIt->next ) {
			if( subjectIt->isIntersection ) {
				subjectIt->entry = e;
				e = ! e;
			}
		}
	}

	for( vector<shared_ptr<PolyContour> >::iterator clipContourIt = clip.contoursBegin(); clipContourIt != clip.contoursEnd(); ++clipContourIt ) {
		e = subject.testInside( (*clipContourIt)->front()->pos );
		if( pC )
			e = ! e;
		for( node *clipIt = (*clipContourIt)->front(); clipIt; clipIt = clipIt->next ) {
			if( clipIt->isIntersection ) {
				clipIt->entry = e;
				e = ! e;
			}
		}
	}

	vector<vector<Vec2f> > result;

	subject.closeContours();
	clip.closeContours();

	node *it;
	for( vector<shared_ptr<PolyContour> >::iterator subjectContourIt = subject.contoursBegin(); subjectContourIt != subject.contoursEnd(); ++subjectContourIt ) {	
		while( ( it = (*subjectContourIt)->firstUnvisitedIntersection() ) != 0 ) {
			vector<Vec2f> resultPoly;		
			while( ! it->visited ) {
				bool forward = it->entry;
				while( true ) {
					resultPoly.push_back( it->pos );
					it->visited = true;
					it = forward ? it->next : it->prev;
					if( it->isIntersection ) {
						it->visited = true;
						break;
					}
				}
				it = it->neighbor;
			}
			
			result.push_back( resultPoly );
		}
	}
	
	return result;
}

vector<vector<Vec2f> > calculateIntersection( const vector<vector<Vec2f> > &subject, const vector<vector<Vec2f> > &clip, int mode )
{
	Poly subjectPoly( subject ), clipPoly( clip );
	if( mode == 0 ) std::cout << "intersection" << std::endl;
	else if( mode == 1 ) std::cout << "union" << std::endl;
	else if( mode == 2 ) std::cout << "subject - clip" << std::endl;		
	else				std::cout << "clip - subject" << std::endl;		
	
	return clipPolys( subjectPoly, clipPoly, mode & 1, mode & 2 );
}