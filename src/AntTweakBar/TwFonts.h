//  ---------------------------------------------------------------------------
//
//  @file       TwFonts.h
//  @brief      Bitmaps fonts
//  @author     Philippe Decaudin
//  @license    This file is part of the AntTweakBar library.
//              For conditions of distribution and use, see License.txt
//
//  note:       Private header
//
//  ---------------------------------------------------------------------------


#if !defined ANT_TW_FONTS_INCLUDED
#define ANT_TW_FONTS_INCLUDED

//#include <AntTweakBar.h>

/*
A source bitmap includes 224 characters starting from ascii char 32 (i.e. space) 
to ascii char 255 (extended ASCII Latin1/CP1252):
  
 !"#$%&'()*+,-./0123456789:;<=>?
@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_
`abcdefghijklmnopqrstuvwxyz{|}~
��������������������������������
��������������������������������
��������������������������������
��������������������������������

First pixel column of a source bitmap is a delimiter with color=zero at the end of each line of characters.
Last pixel row of a line of characters is a delimiter with color=zero at the last pixel of each character.

*/


struct CTexFont
{
    unsigned char * m_TexBytes;
    int             m_TexWidth;     // power of 2
    int             m_TexHeight;    // power of 2
    float           m_CharU0[256];
    float           m_CharV0[256];
    float           m_CharU1[256];
    float           m_CharV1[256];
    int             m_CharWidth[256];
    int             m_CharHeight;
    int             m_NbCharRead;

    CTexFont();
    ~CTexFont();
};


CTexFont *TwGenerateFont(const unsigned char *_Bitmap, int _BmWidth, int _BmHeight, float _Scaling=1.0f);


extern CTexFont *g_DefaultSmallFont;
extern CTexFont *g_DefaultNormalFont;
extern CTexFont *g_DefaultLargeFont;
extern CTexFont *g_DefaultFixed1Font;

void TwGenerateDefaultFonts(float _Scaling=1.0f);
void TwDeleteDefaultFonts();


#endif  // !defined ANT_TW_FONTS_INCLUDED
