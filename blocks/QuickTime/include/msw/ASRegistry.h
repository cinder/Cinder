/*
     File:       ASRegistry.h
 
     Contains:   AppleScript Registry constants.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1991-2001 by Apple Computer, Inc., all rights reserved
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __ASREGISTRY__
#define __ASREGISTRY__

#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

#ifndef __AEOBJECTS__
#include <AEObjects.h>
#endif



#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif


enum {
  keyAETarget                   = FOUR_CHAR_CODE('targ'),
  keySubjectAttr                = FOUR_CHAR_CODE('subj'), /* Magic 'returning' parameter: */
  keyASReturning                = FOUR_CHAR_CODE('Krtn'), /* AppleScript Specific Codes: */
  kASAppleScriptSuite           = FOUR_CHAR_CODE('ascr'),
  kASScriptEditorSuite          = FOUR_CHAR_CODE('ToyS'),
  kASTypeNamesSuite             = FOUR_CHAR_CODE('tpnm'), /* dynamic terminologies */
  typeAETE                      = FOUR_CHAR_CODE('aete'),
  typeAEUT                      = FOUR_CHAR_CODE('aeut'),
  kGetAETE                      = FOUR_CHAR_CODE('gdte'),
  kGetAEUT                      = FOUR_CHAR_CODE('gdut'),
  kUpdateAEUT                   = FOUR_CHAR_CODE('udut'),
  kUpdateAETE                   = FOUR_CHAR_CODE('udte'),
  kCleanUpAEUT                  = FOUR_CHAR_CODE('cdut'),
  kASComment                    = FOUR_CHAR_CODE('cmnt'),
  kASLaunchEvent                = FOUR_CHAR_CODE('noop'),
  keyScszResource               = FOUR_CHAR_CODE('scsz'),
  typeScszResource              = FOUR_CHAR_CODE('scsz'), /* subroutine calls */
  kASSubroutineEvent            = FOUR_CHAR_CODE('psbr'),
  keyASSubroutineName           = FOUR_CHAR_CODE('snam'),
  kASPrepositionalSubroutine    = FOUR_CHAR_CODE('psbr'),
  keyASPositionalArgs           = FOUR_CHAR_CODE('parg')
};

enum {
                                        /* Add this parameter to a Get Data result if your app handled the 'as' parameter */
  keyAppHandledCoercion         = FOUR_CHAR_CODE('idas')
};

enum {
                                        /* Miscellaneous AppleScript commands */
  kASStartLogEvent              = FOUR_CHAR_CODE('log1'),
  kASStopLogEvent               = FOUR_CHAR_CODE('log0'),
  kASCommentEvent               = FOUR_CHAR_CODE('cmnt')
};


/* Operator Events: */
enum {
                                        /* Binary: */
  kASAdd                        = FOUR_CHAR_CODE('+   '),
  kASSubtract                   = FOUR_CHAR_CODE('-   '),
  kASMultiply                   = FOUR_CHAR_CODE('*   '),
  kASDivide                     = FOUR_CHAR_CODE('/   '),
  kASQuotient                   = FOUR_CHAR_CODE('div '),
  kASRemainder                  = FOUR_CHAR_CODE('mod '),
  kASPower                      = FOUR_CHAR_CODE('^   '),
  kASEqual                      = kAEEquals,
  kASNotEqual                   = 0xAD202020,
  kASGreaterThan                = kAEGreaterThan,
  kASGreaterThanOrEqual         = kAEGreaterThanEquals,
  kASLessThan                   = kAELessThan,
  kASLessThanOrEqual            = kAELessThanEquals,
  kASComesBefore                = FOUR_CHAR_CODE('cbfr'),
  kASComesAfter                 = FOUR_CHAR_CODE('cafr'),
  kASConcatenate                = FOUR_CHAR_CODE('ccat'),
  kASStartsWith                 = kAEBeginsWith,
  kASEndsWith                   = kAEEndsWith,
  kASContains                   = kAEContains
};

enum {
  kASAnd                        = kAEAND,
  kASOr                         = kAEOR, /* Unary: */
  kASNot                        = kAENOT,
  kASNegate                     = FOUR_CHAR_CODE('neg '),
  keyASArg                      = FOUR_CHAR_CODE('arg ')
};

enum {
                                        /* event code for the 'error' statement */
  kASErrorEventCode             = FOUR_CHAR_CODE('err '),
  kOSAErrorArgs                 = FOUR_CHAR_CODE('erra'),
  keyAEErrorObject              = FOUR_CHAR_CODE('erob'), /* Properties: */
  pLength                       = FOUR_CHAR_CODE('leng'),
  pReverse                      = FOUR_CHAR_CODE('rvse'),
  pRest                         = FOUR_CHAR_CODE('rest'),
  pInherits                     = FOUR_CHAR_CODE('c@#^'),
  pProperties                   = FOUR_CHAR_CODE('pALL'), /* User-Defined Record Fields: */
  keyASUserRecordFields         = FOUR_CHAR_CODE('usrf'),
  typeUserRecordFields          = typeAEList
};

/* Prepositions: */
enum {
  keyASPrepositionAt            = FOUR_CHAR_CODE('at  '),
  keyASPrepositionIn            = FOUR_CHAR_CODE('in  '),
  keyASPrepositionFrom          = FOUR_CHAR_CODE('from'),
  keyASPrepositionFor           = FOUR_CHAR_CODE('for '),
  keyASPrepositionTo            = FOUR_CHAR_CODE('to  '),
  keyASPrepositionThru          = FOUR_CHAR_CODE('thru'),
  keyASPrepositionThrough       = FOUR_CHAR_CODE('thgh'),
  keyASPrepositionBy            = FOUR_CHAR_CODE('by  '),
  keyASPrepositionOn            = FOUR_CHAR_CODE('on  '),
  keyASPrepositionInto          = FOUR_CHAR_CODE('into'),
  keyASPrepositionOnto          = FOUR_CHAR_CODE('onto'),
  keyASPrepositionBetween       = FOUR_CHAR_CODE('btwn'),
  keyASPrepositionAgainst       = FOUR_CHAR_CODE('agst'),
  keyASPrepositionOutOf         = FOUR_CHAR_CODE('outo'),
  keyASPrepositionInsteadOf     = FOUR_CHAR_CODE('isto'),
  keyASPrepositionAsideFrom     = FOUR_CHAR_CODE('asdf'),
  keyASPrepositionAround        = FOUR_CHAR_CODE('arnd'),
  keyASPrepositionBeside        = FOUR_CHAR_CODE('bsid'),
  keyASPrepositionBeneath       = FOUR_CHAR_CODE('bnth'),
  keyASPrepositionUnder         = FOUR_CHAR_CODE('undr')
};

enum {
  keyASPrepositionOver          = FOUR_CHAR_CODE('over'),
  keyASPrepositionAbove         = FOUR_CHAR_CODE('abve'),
  keyASPrepositionBelow         = FOUR_CHAR_CODE('belw'),
  keyASPrepositionApartFrom     = FOUR_CHAR_CODE('aprt'),
  keyASPrepositionGiven         = FOUR_CHAR_CODE('givn'),
  keyASPrepositionWith          = FOUR_CHAR_CODE('with'),
  keyASPrepositionWithout       = FOUR_CHAR_CODE('wout'),
  keyASPrepositionAbout         = FOUR_CHAR_CODE('abou'),
  keyASPrepositionSince         = FOUR_CHAR_CODE('snce'),
  keyASPrepositionUntil         = FOUR_CHAR_CODE('till')
};

enum {
                                        /* Terminology & Dialect things: */
  kDialectBundleResType         = FOUR_CHAR_CODE('Dbdl'), /* AppleScript Classes and Enums: */
  cConstant                     = typeEnumerated,
  cClassIdentifier              = pClass,
  cObjectBeingExamined          = typeObjectBeingExamined,
  cList                         = typeAEList,
  cSmallReal                    = typeSMFloat,
  cReal                         = typeFloat,
  cRecord                       = typeAERecord,
  cReference                    = cObjectSpecifier,
  cUndefined                    = FOUR_CHAR_CODE('undf'),
  cMissingValue                 = FOUR_CHAR_CODE('msng'),
  cSymbol                       = FOUR_CHAR_CODE('symb'),
  cLinkedList                   = FOUR_CHAR_CODE('llst'),
  cVector                       = FOUR_CHAR_CODE('vect'),
  cEventIdentifier              = FOUR_CHAR_CODE('evnt'),
  cKeyIdentifier                = FOUR_CHAR_CODE('kyid'),
  cUserIdentifier               = FOUR_CHAR_CODE('uid '),
  cPreposition                  = FOUR_CHAR_CODE('prep'),
  cKeyForm                      = enumKeyForm,
  cScript                       = FOUR_CHAR_CODE('scpt'),
  cHandler                      = FOUR_CHAR_CODE('hand'),
  cProcedure                    = FOUR_CHAR_CODE('proc')
};

enum {
  cHandleBreakpoint             = FOUR_CHAR_CODE('brak')
};

enum {
  cClosure                      = FOUR_CHAR_CODE('clsr'),
  cRawData                      = FOUR_CHAR_CODE('rdat'),
  cStringClass                  = typeChar,
  cNumber                       = FOUR_CHAR_CODE('nmbr'),
  cListElement                  = FOUR_CHAR_CODE('celm'),
  cListOrRecord                 = FOUR_CHAR_CODE('lr  '),
  cListOrString                 = FOUR_CHAR_CODE('ls  '),
  cListRecordOrString           = FOUR_CHAR_CODE('lrs '),
  cNumberOrString               = FOUR_CHAR_CODE('ns  '),
  cNumberOrDateTime             = FOUR_CHAR_CODE('nd  '),
  cNumberDateTimeOrString       = FOUR_CHAR_CODE('nds '),
  cAliasOrString                = FOUR_CHAR_CODE('sf  '),
  cSeconds                      = FOUR_CHAR_CODE('scnd'),
  typeSound                     = FOUR_CHAR_CODE('snd '),
  enumBooleanValues             = FOUR_CHAR_CODE('boov'), /*  Use this instead of typeBoolean to avoid with/without conversion  */
  kAETrue                       = typeTrue,
  kAEFalse                      = typeFalse,
  enumMiscValues                = FOUR_CHAR_CODE('misc'),
  kASCurrentApplication         = FOUR_CHAR_CODE('cura'), /* User-defined property ospecs: */
  formUserPropertyID            = FOUR_CHAR_CODE('usrp')
};

enum {
  cString                       = cStringClass /* old name for cStringClass - can't be used in .r files*/
};

enum {
                                        /* Global properties: */
  pASIt                         = FOUR_CHAR_CODE('it  '),
  pASMe                         = FOUR_CHAR_CODE('me  '),
  pASResult                     = FOUR_CHAR_CODE('rslt'),
  pASSpace                      = FOUR_CHAR_CODE('spac'),
  pASReturn                     = FOUR_CHAR_CODE('ret '),
  pASTab                        = FOUR_CHAR_CODE('tab '),
  pASPi                         = FOUR_CHAR_CODE('pi  '),
  pASParent                     = FOUR_CHAR_CODE('pare'),
  kASInitializeEventCode        = FOUR_CHAR_CODE('init'),
  pASPrintLength                = FOUR_CHAR_CODE('prln'),
  pASPrintDepth                 = FOUR_CHAR_CODE('prdp'),
  pASTopLevelScript             = FOUR_CHAR_CODE('ascr')
};

enum {
                                        /* Considerations */
  kAECase                       = FOUR_CHAR_CODE('case'),
  kAEDiacritic                  = FOUR_CHAR_CODE('diac'),
  kAEWhiteSpace                 = FOUR_CHAR_CODE('whit'),
  kAEHyphens                    = FOUR_CHAR_CODE('hyph'),
  kAEExpansion                  = FOUR_CHAR_CODE('expa'),
  kAEPunctuation                = FOUR_CHAR_CODE('punc'),
  kAEZenkakuHankaku             = FOUR_CHAR_CODE('zkhk'),
  kAESmallKana                  = FOUR_CHAR_CODE('skna'),
  kAEKataHiragana               = FOUR_CHAR_CODE('hika'),
  kASConsiderReplies            = FOUR_CHAR_CODE('rmte'),
  enumConsiderations            = FOUR_CHAR_CODE('cons')
};

/* Considerations bit masks */
enum {
  kAECaseConsiderMask           = 0x00000001,
  kAEDiacriticConsiderMask      = 0x00000002,
  kAEWhiteSpaceConsiderMask     = 0x00000004,
  kAEHyphensConsiderMask        = 0x00000008,
  kAEExpansionConsiderMask      = 0x00000010,
  kAEPunctuationConsiderMask    = 0x00000020,
  kASConsiderRepliesConsiderMask = 0x00000040,
  kAECaseIgnoreMask             = 0x00010000,
  kAEDiacriticIgnoreMask        = 0x00020000,
  kAEWhiteSpaceIgnoreMask       = 0x00040000,
  kAEHyphensIgnoreMask          = 0x00080000,
  kAEExpansionIgnoreMask        = 0x00100000,
  kAEPunctuationIgnoreMask      = 0x00200000,
  kASConsiderRepliesIgnoreMask  = 0x00400000,
  enumConsidsAndIgnores         = FOUR_CHAR_CODE('csig')
};

enum {
  cCoercion                     = FOUR_CHAR_CODE('coec'),
  cCoerceUpperCase              = FOUR_CHAR_CODE('txup'),
  cCoerceLowerCase              = FOUR_CHAR_CODE('txlo'),
  cCoerceRemoveDiacriticals     = FOUR_CHAR_CODE('txdc'),
  cCoerceRemovePunctuation      = FOUR_CHAR_CODE('txpc'),
  cCoerceRemoveHyphens          = FOUR_CHAR_CODE('txhy'),
  cCoerceOneByteToTwoByte       = FOUR_CHAR_CODE('txex'),
  cCoerceRemoveWhiteSpace       = FOUR_CHAR_CODE('txws'),
  cCoerceSmallKana              = FOUR_CHAR_CODE('txsk'),
  cCoerceZenkakuhankaku         = FOUR_CHAR_CODE('txze'),
  cCoerceKataHiragana           = FOUR_CHAR_CODE('txkh'), /* Lorax things: */
  cZone                         = FOUR_CHAR_CODE('zone'),
  cMachine                      = FOUR_CHAR_CODE('mach'),
  cAddress                      = FOUR_CHAR_CODE('addr'),
  cRunningAddress               = FOUR_CHAR_CODE('radd'),
  cStorage                      = FOUR_CHAR_CODE('stor')
};

enum {
                                        /* DateTime things: */
  pASWeekday                    = FOUR_CHAR_CODE('wkdy'),
  pASMonth                      = FOUR_CHAR_CODE('mnth'),
  pASDay                        = FOUR_CHAR_CODE('day '),
  pASYear                       = FOUR_CHAR_CODE('year'),
  pASTime                       = FOUR_CHAR_CODE('time'),
  pASDateString                 = FOUR_CHAR_CODE('dstr'),
  pASTimeString                 = FOUR_CHAR_CODE('tstr'), /* Months */
  cMonth                        = pASMonth,
  cJanuary                      = FOUR_CHAR_CODE('jan '),
  cFebruary                     = FOUR_CHAR_CODE('feb '),
  cMarch                        = FOUR_CHAR_CODE('mar '),
  cApril                        = FOUR_CHAR_CODE('apr '),
  cMay                          = FOUR_CHAR_CODE('may '),
  cJune                         = FOUR_CHAR_CODE('jun '),
  cJuly                         = FOUR_CHAR_CODE('jul '),
  cAugust                       = FOUR_CHAR_CODE('aug '),
  cSeptember                    = FOUR_CHAR_CODE('sep '),
  cOctober                      = FOUR_CHAR_CODE('oct '),
  cNovember                     = FOUR_CHAR_CODE('nov '),
  cDecember                     = FOUR_CHAR_CODE('dec ')
};

enum {
                                        /* Weekdays */
  cWeekday                      = pASWeekday,
  cSunday                       = FOUR_CHAR_CODE('sun '),
  cMonday                       = FOUR_CHAR_CODE('mon '),
  cTuesday                      = FOUR_CHAR_CODE('tue '),
  cWednesday                    = FOUR_CHAR_CODE('wed '),
  cThursday                     = FOUR_CHAR_CODE('thu '),
  cFriday                       = FOUR_CHAR_CODE('fri '),
  cSaturday                     = FOUR_CHAR_CODE('sat '), /* AS 1.1 Globals: */
  pASQuote                      = FOUR_CHAR_CODE('quot'),
  pASSeconds                    = FOUR_CHAR_CODE('secs'),
  pASMinutes                    = FOUR_CHAR_CODE('min '),
  pASHours                      = FOUR_CHAR_CODE('hour'),
  pASDays                       = FOUR_CHAR_CODE('days'),
  pASWeeks                      = FOUR_CHAR_CODE('week'), /* Writing Code things: */
  cWritingCodeInfo              = FOUR_CHAR_CODE('citl'),
  pScriptCode                   = FOUR_CHAR_CODE('pscd'),
  pLangCode                     = FOUR_CHAR_CODE('plcd'), /* Magic Tell and End Tell events for logging: */
  kASMagicTellEvent             = FOUR_CHAR_CODE('tell'),
  kASMagicEndTellEvent          = FOUR_CHAR_CODE('tend')
};


#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ASREGISTRY__ */

