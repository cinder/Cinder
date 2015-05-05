/*
From https://github.com/husl-colors/husl

Copyright (c) 2015 Alexei Boronine

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifdef __cplusplus
extern "C" {
#endif

    //These are the only 2 functions you have to use. Don't care about the ones in husl.c

    //Pass in HUSL values and get back RGB values, H ranges from 0 to 360, S and L from 0 to 100.
    //RGB values will range from 0 to 1.
    void HUSLtoRGB( float *r, float *g, float *b, float h, float s, float l );

    //Pass in RGB values ranging from 0 to 1 and get back HUSL values.
    //H ranges from 0 to 360, S and L from 0 to 100.
    void RGBtoHUSL( float *h, float *s, float *l, float r, float g, float b );


#ifdef __cplusplus
}
#endif