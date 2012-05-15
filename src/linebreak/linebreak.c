/* vim: set tabstop=4 shiftwidth=4: */

/*
 * Line breaking in a Unicode sequence.  Designed to be used in a
 * generic text renderer.
 *
 * Copyright (C) 2008-2011 Wu Yongwei <wuyongwei at gmail dot com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the author be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software.  If you use this
 *    software in a product, an acknowledgement in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must
 *    not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 * The main reference is Unicode Standard Annex 14 (UAX #14):
 *		<URL:http://www.unicode.org/reports/tr14/>
 *
 * When this library was designed, this annex was at Revision 19, for
 * Unicode 5.0.0:
 *		<URL:http://www.unicode.org/reports/tr14/tr14-19.html>
 *
 * This library has been updated according to Revision 26, for
 * Unicode 6.0.0:
 *		<URL:http://www.unicode.org/reports/tr14/tr14-26.html>
 *
 * The Unicode Terms of Use are available at
 *		<URL:http://www.unicode.org/copyright.html>
 */

/**
 * @file	linebreak.c
 *
 * Implementation of the line breaking algorithm as described in Unicode
 * Standard Annex 14.
 *
 * @version	2.1, 2011/05/07
 * @author	Wu Yongwei
 */

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "linebreak.h"
#include "linebreakdef.h"

/**
 * Size of the second-level index to the line breaking properties.
 */
#define LINEBREAK_INDEX_SIZE 40

/**
 * Version number of the library.
 */
const int linebreak_version = LINEBREAK_VERSION;

/**
 * Enumeration of break actions.  They are used in the break action
 * pair table below.
 */
enum BreakAction
{
	DIR_BRK,		/**< Direct break opportunity */
	IND_BRK,		/**< Indirect break opportunity */
	CMI_BRK,		/**< Indirect break opportunity for combining marks */
	CMP_BRK,		/**< Prohibited break for combining marks */
	PRH_BRK			/**< Prohibited break */
};

/**
 * Break action pair table.  This is a direct mapping of Table 2 of
 * Unicode Standard Annex 14, Revision 24.
 */
static enum BreakAction baTable[LBP_JT][LBP_JT] = {
	{	/* OP */
		PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, CMP_BRK,
		PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK, PRH_BRK },
	{	/* CL */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, PRH_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* CP */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, PRH_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* QU */
		PRH_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK,
		IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK },
	{	/* GL */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK,
		IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK },
	{	/* NS */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* EX */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* SY */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* IS */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, IND_BRK, IND_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* PR */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, IND_BRK, IND_BRK, IND_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK },
	{	/* PO */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, IND_BRK, IND_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* NU */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* AL */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, IND_BRK, IND_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* ID */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* IN */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* HY */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, DIR_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* BA */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, DIR_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* BB */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK,
		IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK },
	{	/* B2 */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, IND_BRK, IND_BRK, DIR_BRK, PRH_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* ZW */
		DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, PRH_BRK, DIR_BRK,
		DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* CM */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, DIR_BRK, IND_BRK, IND_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK },
	{	/* WJ */
		IND_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK,
		IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK },
	{	/* H2 */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, IND_BRK, IND_BRK },
	{	/* H3 */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, IND_BRK },
	{	/* JL */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, IND_BRK, IND_BRK, IND_BRK, IND_BRK, DIR_BRK },
	{	/* JV */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, IND_BRK, IND_BRK },
	{	/* JT */
		DIR_BRK, PRH_BRK, PRH_BRK, IND_BRK, IND_BRK, IND_BRK, PRH_BRK,
		PRH_BRK, PRH_BRK, DIR_BRK, IND_BRK, DIR_BRK, DIR_BRK, DIR_BRK,
		IND_BRK, IND_BRK, IND_BRK, DIR_BRK, DIR_BRK, PRH_BRK, CMI_BRK,
		PRH_BRK, DIR_BRK, DIR_BRK, DIR_BRK, DIR_BRK, IND_BRK }
};

/**
 * Struct for the second-level index to the line breaking properties.
 */
struct LineBreakPropertiesIndex
{
	utf32_t end;					/**< End coding point */
	struct LineBreakProperties *lbp;/**< Pointer to line breaking properties */
};

/**
 * Second-level index to the line breaking properties.
 */
static struct LineBreakPropertiesIndex lb_prop_index[LINEBREAK_INDEX_SIZE] =
{
	{ 0xFFFFFFFF, lb_prop_default }
};

/**
 * Initializes the second-level index to the line breaking properties.
 * If it is not called, the performance of #get_char_lb_class_lang (and
 * thus the main functionality) can be pretty bad, especially for big
 * code points like those of Chinese.
 */
void init_linebreak(void)
{
	size_t i;
	size_t iPropDefault;
	size_t len;
	size_t step;

	len = 0;
	while (lb_prop_default[len].prop != LBP_Undefined)
		++len;
	step = len / LINEBREAK_INDEX_SIZE;
	iPropDefault = 0;
	for (i = 0; i < LINEBREAK_INDEX_SIZE; ++i)
	{
		lb_prop_index[i].lbp = lb_prop_default + iPropDefault;
		iPropDefault += step;
		lb_prop_index[i].end = lb_prop_default[iPropDefault].start - 1;
	}
	lb_prop_index[--i].end = 0xFFFFFFFF;
}

/**
 * Gets the language-specific line breaking properties.
 *
 * @param lang	language of the text
 * @return		pointer to the language-specific line breaking
 *				properties array if found; \c NULL otherwise
 */
static struct LineBreakProperties *get_lb_prop_lang(const char *lang)
{
	struct LineBreakPropertiesLang *lbplIter;
	if (lang != NULL)
	{
		for (lbplIter = lb_prop_lang_map; lbplIter->lang != NULL; ++lbplIter)
		{
			if (strncmp(lang, lbplIter->lang, lbplIter->namelen) == 0)
			{
				return lbplIter->lbp;
			}
		}
	}
	return NULL;
}

/**
 * Gets the line breaking class of a character from a line breaking
 * properties array.
 *
 * @param ch	character to check
 * @param lbp	pointer to the line breaking properties array
 * @return		the line breaking class if found; \c LBP_XX otherwise
 */
static enum LineBreakClass get_char_lb_class(
		utf32_t ch,
		struct LineBreakProperties *lbp)
{
	while (lbp->prop != LBP_Undefined && ch >= lbp->start)
	{
		if (ch <= lbp->end)
			return lbp->prop;
		++lbp;
	}
	return LBP_XX;
}

/**
 * Gets the line breaking class of a character from the default line
 * breaking properties array.
 *
 * @param ch	character to check
 * @return		the line breaking class if found; \c LBP_XX otherwise
 */
static enum LineBreakClass get_char_lb_class_default(
		utf32_t ch)
{
	size_t i = 0;
	while (ch > lb_prop_index[i].end)
		++i;
	assert(i < LINEBREAK_INDEX_SIZE);
	return get_char_lb_class(ch, lb_prop_index[i].lbp);
}

/**
 * Gets the line breaking class of a character for a specific
 * language.  This function will check the language-specific data first,
 * and then the default data if there is no language-specific property
 * available for the character.
 *
 * @param ch		character to check
 * @param lbpLang	pointer to the language-specific line breaking
 *					properties array
 * @return			the line breaking class if found; \c LBP_XX
 *					otherwise
 */
static enum LineBreakClass get_char_lb_class_lang(
		utf32_t ch,
		struct LineBreakProperties *lbpLang)
{
	enum LineBreakClass lbcResult;

	/* Find the language-specific line breaking class for a character */
	if (lbpLang)
	{
		lbcResult = get_char_lb_class(ch, lbpLang);
		if (lbcResult != LBP_XX)
			return lbcResult;
	}

	/* Find the generic language-specific line breaking class, if no
	 * language context is provided, or language-specific data are not
	 * available for the specific character in the specified language */
	return get_char_lb_class_default(ch);
}

/**
 * Resolves the line breaking class for certain ambiguous or complicated
 * characters.  They are treated in a simplistic way in this
 * implementation.
 *
 * @param lbc	line breaking class to resolve
 * @param lang	language of the text
 * @return		the resolved line breaking class
 */
static enum LineBreakClass resolve_lb_class(
		enum LineBreakClass lbc,
		const char *lang)
{
	switch (lbc)
	{
	case LBP_AI:
		if (lang != NULL &&
				(strncmp(lang, "zh", 2) == 0 ||	/* Chinese */
				 strncmp(lang, "ja", 2) == 0 ||	/* Japanese */
				 strncmp(lang, "ko", 2) == 0))	/* Korean */
		{
			return LBP_ID;
		}
		/* Fall through */
	case LBP_SA:
	case LBP_SG:
	case LBP_XX:
		return LBP_AL;
	default:
		return lbc;
	}
}

/**
 * Gets the next Unicode character in a UTF-8 sequence.  The index will
 * be advanced to the next complete character, unless the end of string
 * is reached in the middle of a UTF-8 sequence.
 *
 * @param[in]     s		input UTF-8 string
 * @param[in]     len	length of the string in bytes
 * @param[in,out] ip	pointer to the index
 * @return				the Unicode character beginning at the index; or
 *						#EOS if end of input is encountered
 */
utf32_t lb_get_next_char_utf8(
		const utf8_t *s,
		size_t len,
		size_t *ip)
{
	utf8_t ch;
	utf32_t res;

	assert(*ip <= len);
	if (*ip == len)
		return EOS;
	ch = s[*ip];

	if (ch < 0xC2 || ch > 0xF4)
	{	/* One-byte sequence, tail (should not occur), or invalid */
		*ip += 1;
		return ch;
	}
	else if (ch < 0xE0)
	{	/* Two-byte sequence */
		if (*ip + 2 > len)
			return EOS;
		res = ((ch & 0x1F) << 6) + (s[*ip + 1] & 0x3F);
		*ip += 2;
		return res;
	}
	else if (ch < 0xF0)
	{	/* Three-byte sequence */
		if (*ip + 3 > len)
			return EOS;
		res = ((ch & 0x0F) << 12) +
			  ((s[*ip + 1] & 0x3F) << 6) +
			  ((s[*ip + 2] & 0x3F));
		*ip += 3;
		return res;
	}
	else
	{	/* Four-byte sequence */
		if (*ip + 4 > len)
			return EOS;
		res = ((ch & 0x07) << 18) +
			  ((s[*ip + 1] & 0x3F) << 12) +
			  ((s[*ip + 2] & 0x3F) << 6) +
			  ((s[*ip + 3] & 0x3F));
		*ip += 4;
		return res;
	}
}

/**
 * Gets the next Unicode character in a UTF-16 sequence.  The index will
 * be advanced to the next complete character, unless the end of string
 * is reached in the middle of a UTF-16 surrogate pair.
 *
 * @param[in]     s		input UTF-16 string
 * @param[in]     len	length of the string in words
 * @param[in,out] ip	pointer to the index
 * @return				the Unicode character beginning at the index; or
 *						#EOS if end of input is encountered
 */
utf32_t lb_get_next_char_utf16(
		const utf16_t *s,
		size_t len,
		size_t *ip)
{
	utf16_t ch;

	assert(*ip <= len);
	if (*ip == len)
		return EOS;
	ch = s[(*ip)++];

	if (ch < 0xD800 || ch > 0xDBFF)
	{	/* If the character is not a high surrogate */
		return ch;
	}
	if (*ip == len)
	{	/* If the input ends here (an error) */
		--(*ip);
		return EOS;
	}
	if (s[*ip] < 0xDC00 || s[*ip] > 0xDFFF)
	{	/* If the next character is not the low surrogate (an error) */
		return ch;
	}
	/* Return the constructed character and advance the index again */
	return (((utf32_t)ch & 0x3FF) << 10) + (s[(*ip)++] & 0x3FF) + 0x10000;
}

/**
 * Gets the next Unicode character in a UTF-32 sequence.  The index will
 * be advanced to the next character.
 *
 * @param[in]     s		input UTF-32 string
 * @param[in]     len	length of the string in dwords
 * @param[in,out] ip	pointer to the index
 * @return				the Unicode character beginning at the index; or
 *						#EOS if end of input is encountered
 */
utf32_t lb_get_next_char_utf32(
		const utf32_t *s,
		size_t len,
		size_t *ip)
{
	assert(*ip <= len);
	if (*ip == len)
		return EOS;
	return s[(*ip)++];
}

/**
 * Sets the line breaking information for a generic input string.
 *
 * @param[in]  s			input string
 * @param[in]  len			length of the input
 * @param[in]  lang			language of the input
 * @param[out] brks			pointer to the output breaking data,
 *							containing #LINEBREAK_MUSTBREAK,
 *							#LINEBREAK_ALLOWBREAK, #LINEBREAK_NOBREAK,
 *							or #LINEBREAK_INSIDEACHAR
 * @param[in] get_next_char	function to get the next UTF-32 character
 */
void set_linebreaks(
		const void *s,
		size_t len,
		const char *lang,
		char *brks,
		get_next_char_t get_next_char)
{
	utf32_t ch;
	enum LineBreakClass lbcCur;
	enum LineBreakClass lbcNew;
	enum LineBreakClass lbcLast;
	struct LineBreakProperties *lbpLang;
	size_t posCur = 0;
	size_t posLast = 0;

	--posLast;	/* To be ++'d later */
	ch = get_next_char(s, len, &posCur);
	if (ch == EOS)
		return;
	lbpLang = get_lb_prop_lang(lang);
	lbcCur = resolve_lb_class(get_char_lb_class_lang(ch, lbpLang), lang);
	lbcNew = LBP_Undefined;

nextline:

	/* Special treatment for the first character */
	switch (lbcCur)
	{
	case LBP_LF:
	case LBP_NL:
		lbcCur = LBP_BK;
		break;
	case LBP_CB:
		lbcCur = LBP_BA;
		break;
	case LBP_SP:
		lbcCur = LBP_WJ;
		break;
	default:
		break;
	}

	/* Process a line till an explicit break or end of string */
	for (;;)
	{
		for (++posLast; posLast < posCur - 1; ++posLast)
		{
			brks[posLast] = LINEBREAK_INSIDEACHAR;
		}
		assert(posLast == posCur - 1);
		lbcLast = lbcNew;
		ch = get_next_char(s, len, &posCur);
		if (ch == EOS)
			break;
		lbcNew = get_char_lb_class_lang(ch, lbpLang);
		if (lbcCur == LBP_BK || (lbcCur == LBP_CR && lbcNew != LBP_LF))
		{
			brks[posLast] = LINEBREAK_MUSTBREAK;
			lbcCur = resolve_lb_class(lbcNew, lang);
			goto nextline;
		}

		switch (lbcNew)
		{
		case LBP_SP:
			brks[posLast] = LINEBREAK_NOBREAK;
			continue;
		case LBP_BK:
		case LBP_LF:
		case LBP_NL:
			brks[posLast] = LINEBREAK_NOBREAK;
			lbcCur = LBP_BK;
			continue;
		case LBP_CR:
			brks[posLast] = LINEBREAK_NOBREAK;
			lbcCur = LBP_CR;
			continue;
		case LBP_CB:
			brks[posLast] = LINEBREAK_ALLOWBREAK;
			lbcCur = LBP_BA;
			continue;
		default:
			break;
		}

		lbcNew = resolve_lb_class(lbcNew, lang);

		assert(lbcCur <= LBP_JT);
		assert(lbcNew <= LBP_JT);
		switch (baTable[lbcCur - 1][lbcNew - 1])
		{
		case DIR_BRK:
			brks[posLast] = LINEBREAK_ALLOWBREAK;
			break;
		case CMI_BRK:
		case IND_BRK:
			if (lbcLast == LBP_SP)
			{
				brks[posLast] = LINEBREAK_ALLOWBREAK;
			}
			else
			{
				brks[posLast] = LINEBREAK_NOBREAK;
			}
			break;
		case CMP_BRK:
			brks[posLast] = LINEBREAK_NOBREAK;
			if (lbcLast != LBP_SP)
				continue;
			break;
		case PRH_BRK:
			brks[posLast] = LINEBREAK_NOBREAK;
			break;
		}

		lbcCur = lbcNew;
	}

	assert(posLast == posCur - 1 && posCur <= len);
	/* Break after the last character */
	brks[posLast] = LINEBREAK_MUSTBREAK;
	/* When the input contains incomplete sequences */
	while (posCur < len)
	{
		brks[posCur++] = LINEBREAK_INSIDEACHAR;
	}
}

/**
 * Sets the line breaking information for a UTF-8 input string.
 *
 * @param[in]  s	input UTF-8 string
 * @param[in]  len	length of the input
 * @param[in]  lang	language of the input
 * @param[out] brks	pointer to the output breaking data, containing
 *					#LINEBREAK_MUSTBREAK, #LINEBREAK_ALLOWBREAK,
 *					#LINEBREAK_NOBREAK, or #LINEBREAK_INSIDEACHAR
 */
void set_linebreaks_utf8(
		const utf8_t *s,
		size_t len,
		const char *lang,
		char *brks)
{
	set_linebreaks(s, len, lang, brks,
				   (get_next_char_t)lb_get_next_char_utf8);
}

/**
 * Sets the line breaking information for a UTF-16 input string.
 *
 * @param[in]  s	input UTF-16 string
 * @param[in]  len	length of the input
 * @param[in]  lang	language of the input
 * @param[out] brks	pointer to the output breaking data, containing
 *					#LINEBREAK_MUSTBREAK, #LINEBREAK_ALLOWBREAK,
 *					#LINEBREAK_NOBREAK, or #LINEBREAK_INSIDEACHAR
 */
void set_linebreaks_utf16(
		const utf16_t *s,
		size_t len,
		const char *lang,
		char *brks)
{
	set_linebreaks(s, len, lang, brks,
				   (get_next_char_t)lb_get_next_char_utf16);
}

/**
 * Sets the line breaking information for a UTF-32 input string.
 *
 * @param[in]  s	input UTF-32 string
 * @param[in]  len	length of the input
 * @param[in]  lang	language of the input
 * @param[out] brks	pointer to the output breaking data, containing
 *					#LINEBREAK_MUSTBREAK, #LINEBREAK_ALLOWBREAK,
 *					#LINEBREAK_NOBREAK, or #LINEBREAK_INSIDEACHAR
 */
void set_linebreaks_utf32(
		const utf32_t *s,
		size_t len,
		const char *lang,
		char *brks)
{
	set_linebreaks(s, len, lang, brks,
				   (get_next_char_t)lb_get_next_char_utf32);
}

/**
 * Tells whether a line break can occur between two Unicode characters.
 * This is a wrapper function to expose a simple interface.  Generally
 * speaking, it is better to use #set_linebreaks_utf32 instead, since
 * complicated cases involving combining marks, spaces, etc. cannot be
 * correctly processed.
 *
 * @param char1 the first Unicode character
 * @param char2 the second Unicode character
 * @param lang  language of the input
 * @return      one of #LINEBREAK_MUSTBREAK, #LINEBREAK_ALLOWBREAK,
 *				#LINEBREAK_NOBREAK, or #LINEBREAK_INSIDEACHAR
 */
int is_line_breakable(
		utf32_t char1,
		utf32_t char2,
		const char* lang)
{
	utf32_t s[2];
	char brks[2];
	s[0] = char1;
	s[1] = char2;
	set_linebreaks_utf32(s, 2, lang, brks);
	return brks[0];
}
