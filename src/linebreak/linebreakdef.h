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
 * @file	linebreakdef.h
 *
 * Definitions of internal data structures, declarations of global
 * variables, and function prototypes for the line breaking algorithm.
 *
 * @version	2.1, 2011/05/07
 * @author	Wu Yongwei
 */

/**
 * Constant value to mark the end of string.  It is not a valid Unicode
 * character.
 */
#define EOS 0xFFFF

/**
 * Line break classes.  This is a direct mapping of Table 1 of Unicode
 * Standard Annex 14, Revision 26.
 */
enum LineBreakClass
{
	/* This is used to signal an error condition. */
	LBP_Undefined,	/**< Undefined */

	/* The following break classes are treated in the pair table. */
	LBP_OP,			/**< Opening punctuation */
	LBP_CL,			/**< Closing punctuation */
	LBP_CP,			/**< Closing parenthesis */
	LBP_QU,			/**< Ambiguous quotation */
	LBP_GL,			/**< Glue */
	LBP_NS,			/**< Non-starters */
	LBP_EX,			/**< Exclamation/Interrogation */
	LBP_SY,			/**< Symbols allowing break after */
	LBP_IS,			/**< Infix separator */
	LBP_PR,			/**< Prefix */
	LBP_PO,			/**< Postfix */
	LBP_NU,			/**< Numeric */
	LBP_AL,			/**< Alphabetic */
	LBP_ID,			/**< Ideographic */
	LBP_IN,			/**< Inseparable characters */
	LBP_HY,			/**< Hyphen */
	LBP_BA,			/**< Break after */
	LBP_BB,			/**< Break before */
	LBP_B2,			/**< Break on either side (but not pair) */
	LBP_ZW,			/**< Zero-width space */
	LBP_CM,			/**< Combining marks */
	LBP_WJ,			/**< Word joiner */
	LBP_H2,			/**< Hangul LV */
	LBP_H3,			/**< Hangul LVT */
	LBP_JL,			/**< Hangul L Jamo */
	LBP_JV,			/**< Hangul V Jamo */
	LBP_JT,			/**< Hangul T Jamo */

	/* The following break classes are not treated in the pair table */
	LBP_AI,			/**< Ambiguous (alphabetic or ideograph) */
	LBP_BK,			/**< Break (mandatory) */
	LBP_CB,			/**< Contingent break */
	LBP_CR,			/**< Carriage return */
	LBP_LF,			/**< Line feed */
	LBP_NL,			/**< Next line */
	LBP_SA,			/**< South-East Asian */
	LBP_SG,			/**< Surrogates */
	LBP_SP,			/**< Space */
	LBP_XX			/**< Unknown */
};

/**
 * Struct for entries of line break properties.  The array of the
 * entries \e must be sorted.
 */
struct LineBreakProperties
{
	utf32_t start;				/**< Starting coding point */
	utf32_t end;				/**< End coding point */
	enum LineBreakClass prop;	/**< The line breaking property */
};

/**
 * Struct for association of language-specific line breaking properties
 * with language names.
 */
struct LineBreakPropertiesLang
{
	const char *lang;					/**< Language name */
	size_t namelen;						/**< Length of name to match */
	struct LineBreakProperties *lbp;	/**< Pointer to associated data */
};

/**
 * Abstract function interface for #lb_get_next_char_utf8,
 * #lb_get_next_char_utf16, and #lb_get_next_char_utf32.
 */
typedef utf32_t (*get_next_char_t)(const void *, size_t, size_t *);

/* Declarations */
extern struct LineBreakProperties lb_prop_default[];
extern struct LineBreakPropertiesLang lb_prop_lang_map[];

/* Function Prototype */
utf32_t lb_get_next_char_utf8(const utf8_t *s, size_t len, size_t *ip);
utf32_t lb_get_next_char_utf16(const utf16_t *s, size_t len, size_t *ip);
utf32_t lb_get_next_char_utf32(const utf32_t *s, size_t len, size_t *ip);
void set_linebreaks(
		const void *s,
		size_t len,
		const char *lang,
		char *brks,
		get_next_char_t get_next_char);
