/*
From https://github.com/husl-colors/husl

Changes:
* rename float round(float, int) -> float roundTo(float, int) to avoid redefinition.

Copyright (c) 2015 Alexei Boronine

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#include "husl.h"

#ifdef __cplusplus	//libs if c++ is used
#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#else				//libs if pure c is used
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#define PI 3.1415926535897932384626433832795

float m[3][3] = { 3.2406f, -1.5372f, -0.4986f, -0.9689f, 1.8758f, 0.0415f, 0.0557f, -0.2040f, 1.0570f };
float m_inv[3][3] = { 0.4124f, 0.3576f, 0.1805f, 0.2126f, 0.7152f, 0.0722f, 0.0193f, 0.1192f, 0.9505f };
float refX = 0.95047f;
float refY = 1.00000f;
float refZ = 1.08883f;
float refU = 0.19784f;
float refV = 0.46834f;
float lab_e = 0.008856f;
float lab_k = 903.3f;

char hex[64];
float values[3];


float* XYZ_RGB( float *tuple );
float* RGB_XYZ( float *tuple );
float* XYZ_LUV( float *tuple );
float* LUV_XYZ( float *tuple );
float* LUV_LCH( float *tuple );
float* LCH_LUV( float *tuple );
float* HUSL_LCH( float *tuple );
float* LCH_HUSL( float *tuple );


float maxChroma( float L, float H );
float* rgbPrepare( float *tuple );

float dotProduct( float* a, float * b, int len );
float roundTo( float num, int places );
float f( float t );
float f_inv( float t );
float fromLinear( float c );
float toLinear( float c );


//////////////////////////////////////////////////////////////////////////////////


void HUSLtoRGB( float *r, float *g, float *b, float h, float s, float l )
{
    values[0] = h;
    values[1] = s;
    values[2] = l;

    XYZ_RGB( LUV_XYZ( LCH_LUV( HUSL_LCH( values ) ) ) );

    *r = values[0];
    *g = values[1];
    *b = values[2];

    return;
}

void RGBtoHUSL( float *h, float *s, float *l, float r, float g, float b )
{
    values[0] = r;
    values[1] = g;
    values[2] = b;

    LCH_HUSL( LUV_LCH( XYZ_LUV( RGB_XYZ( values ) ) ) );

    *h = values[0];
    *s = values[1];
    *l = values[2];

    return;
}






float maxChroma( float L, float H ){

    float C, bottom, cosH, hrad, lbottom, m1, m2, m3, rbottom, result, sinH, sub1, sub2, t, top;
    int _i, _j, _len, _len1;
    float *row;
    float _ref[2] = { 0.0f, 1.0f };


    hrad = (float)((H / 360.0f) * 2 * PI);
    sinH = (float)(sin( hrad ));
    cosH = (float)(cos( hrad ));
    sub1 = (float)(pow( L + 16, 3 ) / 1560896.0);
    sub2 = sub1 > 0.008856 ? sub1 : (float)(L / 903.3);
    result = FLT_MAX;
    for ( _i = 0, _len = 3; _i < _len; ++_i ) {
        row = m[_i];
        m1 = row[0], m2 = row[1], m3 = row[2];
        top = (float)((0.99915 * m1 + 1.05122 * m2 + 1.14460 * m3) * sub2);
        rbottom = (float)(0.86330 * m3 - 0.17266 * m2);
        lbottom = (float)(0.12949 * m3 - 0.38848 * m1);
        bottom = (rbottom * sinH + lbottom * cosH) * sub2;

        for ( _j = 0, _len1 = 2; _j < _len1; ++_j ) {
            t = _ref[_j];
            C = (float)(L * (top - 1.05122 * t) / (bottom + 0.17266 * sinH * t));
            if ( (C > 0 && C < result) ) {
                result = C;
            }
        }
    }
    return result;
}

float dotProduct( float* a, float * b, int len ){

    int i, _i, _ref;
    float ret = 0.0f;
    for ( i = _i = 0, _ref = len - 1; 0 <= _ref ? _i <= _ref : _i >= _ref; i = 0 <= _ref ? ++_i : --_i ) {
        ret += a[i] * b[i];
    }
    return ret;

}

float roundTo( float num, int places )
{
    float n;
    n = (float)(pow( 10.0f, places ));
    return (float)(floor( num * n ) / n);
}

float f( float t )
{
    if ( t > lab_e ) {
        return (float)(pow( t, 1.0f / 3.0f ));
    }
    else {
        return (float)(7.787 * t + 16 / 116.0);
    }
}

float f_inv( float t )
{
    if ( pow( t, 3 ) > lab_e ) {
        return (float)(pow( t, 3 ));
    }
    else {
        return (116 * t - 16) / lab_k;
    }
}

float fromLinear( float c )
{
    if ( c <= 0.0031308 ) {
        return 12.92f * c;
    }
    else {
        return (float)(1.055 * pow( c, 1 / 2.4f ) - 0.055);
    }
}

float toLinear( float c )
{
    float a = 0.055f;

    if ( c > 0.04045 ) {
        return (float)(pow( (c + a) / (1 + a), 2.4f ));
    }
    else {
        return (float)(c / 12.92);
    }
}

float* rgbPrepare( float *tuple )
{
    int i;

    for ( i = 0; i < 3; ++i ){
        tuple[i] = roundTo( tuple[i], 3 );

        if ( tuple[i] < 0 || tuple[i] > 1 ) {
            if ( tuple[i] < 0 )
                tuple[i] = 0;
            else
                tuple[i] = 1;
            //printf("Illegal rgb value: %f\n", tuple[i]);
        }

        tuple[i] = roundTo( tuple[i] * 255, 0 );
    }

    return tuple;
}

float* XYZ_RGB( float *tuple )
{
    float B, G, R;
    R = fromLinear( dotProduct( m[0], tuple, 3 ) );
    G = fromLinear( dotProduct( m[1], tuple, 3 ) );
    B = fromLinear( dotProduct( m[2], tuple, 3 ) );

    tuple[0] = R;
    tuple[1] = G;
    tuple[2] = B;

    return tuple;
}

float* RGB_XYZ( float *tuple )
{
    float B, G, R, X, Y, Z;
    float rgbl[3];

    R = tuple[0];
    G = tuple[1];
    B = tuple[2];

    rgbl[0] = toLinear( R );
    rgbl[1] = toLinear( G );
    rgbl[2] = toLinear( B );

    X = dotProduct( m_inv[0], rgbl, 3 );
    Y = dotProduct( m_inv[1], rgbl, 3 );
    Z = dotProduct( m_inv[2], rgbl, 3 );

    tuple[0] = X;
    tuple[1] = Y;
    tuple[2] = Z;

    return tuple;
}

float* XYZ_LUV( float *tuple )
{
    float L, U, V, X, Y, Z, varU, varV;

    X = tuple[0];
    Y = tuple[1];
    Z = tuple[2];

    varU = (4 * X) / (X + (15.0f * Y) + (3 * Z));
    varV = (9 * Y) / (X + (15.0f * Y) + (3 * Z));
    L = 116 * f( Y / refY ) - 16;
    U = 13 * L * (varU - refU);
    V = 13 * L * (varV - refV);

    tuple[0] = L;
    tuple[1] = U;
    tuple[2] = V;

    return tuple;
}

float* LUV_XYZ( float *tuple )
{
    float L, U, V, X, Y, Z, varU, varV, varY;

    L = tuple[0];
    U = tuple[1];
    V = tuple[2];

    if ( L == 0 ) {
        tuple[2] = tuple[1] = tuple[0] = 0.0f;
        return tuple;
    }

    varY = f_inv( (L + 16) / 116.0f );
    varU = U / (13.0f * L) + refU;
    varV = V / (13.0f * L) + refV;
    Y = varY * refY;
    X = 0 - (9 * Y * varU) / ((varU - 4.0f) * varV - varU * varV);
    Z = (9 * Y - (15 * varV * Y) - (varV * X)) / (3.0f * varV);

    tuple[0] = X;
    tuple[1] = Y;
    tuple[2] = Z;

    return tuple;
}

float* LUV_LCH( float *tuple )
{
    float C, H, Hrad, L, U, V;

    L = tuple[0];
    U = tuple[1];
    V = tuple[2];

    C = (float)(pow( pow( U, 2 ) + pow( V, 2 ), (1 / 2.0f) ));
    Hrad = (float)(atan2( V, U ));
    H = (float)(Hrad * 360.0f / 2.0f / PI);
    if ( H < 0 ) {
        H = 360 + H;
    }

    tuple[0] = L;
    tuple[1] = C;
    tuple[2] = H;

    return tuple;
}

float* LCH_LUV( float *tuple )
{
    float C, H, Hrad, L, U, V;

    L = tuple[0];
    C = tuple[1];
    H = tuple[2];

    Hrad = (float)(H / 360.0 * 2.0 * PI);
    U = (float)(cos( Hrad ) * C);
    V = (float)(sin( Hrad ) * C);

    tuple[0] = L;
    tuple[1] = U;
    tuple[2] = V;

    return tuple;
}

float* HUSL_LCH( float *tuple )
{
    float C, H, L, S, max;

    H = tuple[0];
    S = tuple[1];
    L = tuple[2];

    max = maxChroma( L, H );
    C = max / 100.0f * S;

    tuple[0] = L;
    tuple[1] = C;
    tuple[2] = H;

    return tuple;
}

float* LCH_HUSL( float *tuple )
{
    float C, H, L, S, max;

    L = tuple[0];
    C = tuple[1];
    H = tuple[2];

    max = maxChroma( L, H );
    S = C / max * 100;

    tuple[0] = H;
    tuple[1] = S;
    tuple[2] = L;

    return tuple;
}







