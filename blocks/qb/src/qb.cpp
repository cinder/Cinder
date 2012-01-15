//
//  qb.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#include "qb.h"
#include "qbCube.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace cinder { namespace qb {
	
	
	///////////////////////////////////
	//
	// MISC LIB
	// 
	void qbLog(std::string s)
	{
		if ( QB_VERBOSE )
			printf("%s\n",s.c_str());
	}
	void qbCheckGLError(const char *file, int line, const char *func)
	{
		int err=0;
		while( (err=glGetError())!=0 )
		{
			char msg[256];
			switch(err)
			{
				case GL_INVALID_ENUM:
					sprintf(msg, "GL_INVALID_ENUM");
					break;
				case GL_INVALID_VALUE:
					sprintf(msg, "GL_INVALID_VALUE");
					break;
				case GL_INVALID_OPERATION:
					sprintf(msg, "GL_INVALID_OPERATION");
					break;
				case GL_STACK_OVERFLOW:
					sprintf(msg, "GL_STACK_OVERFLOW");
					break;
				case GL_STACK_UNDERFLOW:
					sprintf(msg, "GL_STACK_UNDERFLOW");
					break;
				case GL_OUT_OF_MEMORY:
					sprintf(msg, "GL_OUT_OF_MEMORY");
					break;
				case 1286:		// 0x0506
					sprintf(msg, "INVALID_FRAMEBUFFER_OPERATION_OES");
					break;
				default:
					sprintf(msg, "UNKNOWN ERROR");
			}
			printf(">>> GL ERROR !!!!! %s(%d) : [%s] GL_ERROR=%d/0x%x (%s)\n", file, line, func, err, err, msg);
		}
	}
	
	

	///////////////////////////////////////////
	//
	// TEXT
	//
	void writeFPS()
	{
		float currFPS = math<float>::clamp( App::get()->getAverageFps(), 0, 1000 );
		char str[20];
		sprintf(str, "%.1f", currFPS);
		int x = getWindowWidth() - strlen(str)*8;
		int y = getWindowHeight() - 10;
		qb::writeStuff(str, x, y);
	}
	void writeStuff(std::string text, int x, int y)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		gl::color( Color::white() );
		gl::drawString( toString(text), Vec2f( x, y ), Color( 1, 1, 1 ), _qb.mFontNormal );
		glDisable(GL_BLEND);
	}
	
	
	///////////////////////////////////////////////////////////
	//
	// TRANSFORMATIONS
	//
	// Fit src inside dst
	Rectf rectToFit( Rectf src, Rectf dst )
	{
		float sw = src.getWidth();
		float sh = src.getHeight();
		float sa = (sw / sh);	// aspect ratio
		float dw = dst.getWidth();
		float dh = dst.getHeight();
		float da = (dw / dh);	// aspect ratio
		// different ratio
		if (da > sa)
		{
			float scale = (dh / sh);
			float gap = (dw - (sw * scale)) / 2.0f;
			return Rectf( gap, 0, dw-gap, dh);
		}
		else if (da < sa)
		{
			float scale = (dw / sw);
			float gap = (dh - (sh * scale)) / 2.0f;
			return Rectf( 0, gap, dw, dh-gap );
		}
		// Same ratio
		else
			return dst;
	}
	void transformToFit( Area src, Area dst )
	{
		qb::transformToFit( Vec2i(src.getWidth(), src.getHeight()), Vec2i(dst.getWidth(), dst.getHeight()) );
	}
	void transformToFit( Vec2i src, Vec2i dst )
	{
		// Scale
		float scaleX = ( dst.x / src.x );
		float scaleY = ( dst.y / src.y );
		float sc = math<float>::clamp( ( scaleX < scaleY ? scaleX : scaleY ), 0.0, 1.0 );
		// Position (scaled)
		int gapX = floorf( (src.x * sc) < dst.x ? ( dst.x - (src.x * sc) ) / 2.0 : 0.0 ) ;
		int gapY = floorf( (src.y * sc) < dst.y ? ( dst.y - (src.y * sc) ) / 2.0 : 0.0 ) ;
		// Transform!!
		glTranslatef( gapX, gapY, 0 );
		glScalef( sc, sc, sc );
	}
	
	
	
	
	
	
	
	///////////////////////////////////////////////////////////
	//
	// POST-FX
	//
	// Super Fast Blur v1.1
	// by Mario Klingemann <http://incubator.quasimondo.com>
	// converted to C++ by Mehmet Akten, <http://www.memo.tv>
	//
	// Tip: Multiple invovations of this filter with a small
	// radius will approximate a gaussian blur quite well.
	//
	//#include ImageFilters.h
	void superFastBlur(unsigned char *pix, int w, int h, int radius)
	{
		if (radius<1) return;
		int wm=w-1;
		int hm=h-1;
		int wh=w*h;
		int div=radius+radius+1;
		unsigned char *r=new unsigned char[wh];
		unsigned char *g=new unsigned char[wh];
		unsigned char *b=new unsigned char[wh];
		int rsum,gsum,bsum,x,y,i,p,p1,p2,yp,yi,yw;
		int *vMIN = new int[max(w,h)];
		int *vMAX = new int[max(w,h)];
		
		unsigned char *dv=new unsigned char[256*div];
		for (i=0;i<256*div;i++) dv[i]=(i/div);
		
		yw=yi=0;
		
		for (y=0;y<h;y++){
			rsum=gsum=bsum=0;
			for(i=-radius;i<=radius;i++){
				p = (yi + min(wm, max(i,0))) * 3;
				rsum += pix[p];
				gsum += pix[p+1];
				bsum += pix[p+2];
			}
			for (x=0;x<w;x++){
				
				r[yi]=dv[rsum];
				g[yi]=dv[gsum];
				b[yi]=dv[bsum];
				
				if(y==0){
					vMIN[x]=min(x+radius+1,wm);
					vMAX[x]=max(x-radius,0);
				}
				p1 = (yw+vMIN[x])*3;
				p2 = (yw+vMAX[x])*3;
				
				rsum += pix[p1] - pix[p2];
				gsum += pix[p1+1] - pix[p2+1];
				bsum += pix[p1+2] - pix[p2+2];
				
				yi++;
			}
			yw+=w;
		}
		
		for (x=0;x<w;x++){
			rsum=gsum=bsum=0;
			yp=-radius*w;
			for(i=-radius;i<=radius;i++){
				yi=max(0,yp)+x;
				rsum+=r[yi];
				gsum+=g[yi];
				bsum+=b[yi];
				yp+=w;
			}
			yi=x;
			for (y=0;y<h;y++){
				pix[yi*3] = dv[rsum];
				pix[yi*3 + 1] = dv[gsum];
				pix[yi*3 + 2] = dv[bsum];
				if(x==0){
					vMIN[y]=min(y+radius+1,hm)*w;
					vMAX[y]=max(y-radius,0)*w;
				}
				p1=x+vMIN[y];
				p2=x+vMAX[y];
				
				rsum+=r[p1]-r[p2];
				gsum+=g[p1]-g[p2];
				bsum+=b[p1]-b[p2];
				
				yi+=w;
			}
		}
		
		delete r;
		delete g;
		delete b;
		
		delete vMIN;
		delete vMAX;
		delete dv;
	}
	
	
} } // cinder::qb

