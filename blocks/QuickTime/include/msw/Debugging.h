/*
     File:       Debugging.h
 
     Contains:   Macros to handle exceptions and assertions.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1989-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __DEBUGGING__
#define __DEBUGGING__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MIXEDMODE__
#include <MixedMode.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#include <Events.h>
#ifndef __GESTALT__
#include <Gestalt.h>
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

/*
    This file supplies standard debugging routines and macros to Carbon and
    Classic Mac OS programs. Other C programs which wish to use the
    exception handling and assertion macros should include AssertMacros.h
    instead of this file.

    To activate debugger breaks, #define DEBUG to 1 (one) before including this
    file. Five further levels of debugging are available, selected by #defining
    one of the following conditionals to 1 after DEBUG is defined to 1.

        DEBUG_INTERNAL      the default; asserts include file and line number
                            information

        DEBUG_EXTERNAL      used for code which must ship to developers outside
                            your organization; no file or line number
                            information is included in asserts

        DEBUG_BREAK_ONLY    where an assertion string would normally be sent to
                            the debugger; call Debugger() instead.

        PRODUCTION          used for shipping code; no debugger breaks are
                            emitted

        PERFORMANCE         same as PRODUCTION

    #defining DEBUG to 0 is equivalent to #defining PRODUCTION 1 when
    DEBUG is 1. (No code for debugger breaks is emitted in either case.)
    
    #defining DEBUG to 1 without specifying a level is equivalent to #defining
    DEBUG_INTERNAL 1.

    In addition, these macros should also be #defined (they are described in detail below):
        kComponentSignatureString
        COMPONENT_SIGNATURE
*/
/*
 *  Before including this file, #define kComponentSignatureString to a C-string
 *  containing the name of your client and #define COMPONENT_SIGNATURE to your
 *  client's unique signature (i.e., your program's registered creator type).
 *  For example:
 *
 *      #define kComponentSignatureString "SurfWriter"
 *      #define COMPONENT_SIGNATURE 'WAVE'
 *
 *  If you don't define kComponentSignatureString and COMPONENT_SIGNATURE, the
 *  default kComponentSignatureString and COMPONENT_SIGNATURE values will be
 *  used by the DEBUGASSERTMSG macros below.
 */

#ifndef kComponentSignatureString
 #define kComponentSignatureString "Third Party Client"
#endif
#ifndef COMPONENT_SIGNATURE
  #define COMPONENT_SIGNATURE '?*?*'
#endif
#define QuoteExceptionString(x) #x


/*
 *  The DEBUGLEVEL's
 */
#define DEBUG_LEVEL_PRODUCTION 0
#define DEBUG_LEVEL_BREAK_ONLY    1
#define DEBUG_LEVEL_EXTERNAL  3
#define DEBUG_LEVEL_INTERNAL  4
#define DEBUGFULL             DEBUG_LEVEL_INTERNAL

/*
 * define DEBUGLEVEL
 */
#if DEBUG
    #if PRODUCTION
     #define DEBUGLEVEL DEBUG_LEVEL_PRODUCTION
  #elif PERFORMANCE
      #define DEBUGLEVEL DEBUG_LEVEL_PRODUCTION
  #elif DEBUG_BREAK_ONLY
     #define DEBUGLEVEL DEBUG_LEVEL_BREAK_ONLY
  #elif DEBUG_EXTERNAL
       #define DEBUGLEVEL DEBUG_LEVEL_EXTERNAL
    #elif DEBUG_INTERNAL
       #define DEBUGLEVEL DEBUG_LEVEL_INTERNAL
    #else
      /* default to DEBUG_LEVEL_INTERNAL */
      #define DEBUGLEVEL DEBUG_LEVEL_INTERNAL
    #endif
#endif
#ifndef DEBUGLEVEL
#define DEBUGLEVEL DEBUG_LEVEL_PRODUCTION
#endif



/*
 *  The options parameter to DebugAssert and DEBUGASSERTMSG is currently reserved (must be zero).
 */
#define DEBUG_NO_OPTIONS      0

/*
 *  DEBUGASSERTMSG()
 *
 *  Summary:
 *    All error reporting is routed through this macro, which calls the system
 *    routine DebugAssert(). If you wish to use your own assertion break
 *    routine, you can override DEBUGASSERTMSG by defining it before including
 *    this file.
 *
 *  Parameters:
 *
 *    componentSignature:
 *      The unique signature of component causing the assertion.
 *    
 *    options:
 *      reserved.
 *    
 *    assertionString:
 *      A pointer to a string constant containing the assertion.
 *      This must be a string constant (and not a string variable or
 *      NULL) because the Proeprocessor concatenates it with other
 *      string constants
 *    
 *    exceptionLabelString:
 *      A pointer to a string containing the exceptionLabel, or NULL.
 *    
 *    errorString:
 *      A pointer to the error string, or NULL. DEBUGASSERTMSG macros
 *      must not attempt to concatenate this string with constant
 *      character strings.
 *    
 *    fileName:
 *      A pointer to the fileName or pathname (generated by the
 *      preprocessor __FILE__ identifier), or NULL.
 *    
 *    lineNumber:
 *      The line number in the file (generated by the preprocessor
 *      __LINE__ identifier), or 0 (zero).
 *    
 *    value:
 *      A value associated with the assertion, or NULL.
 */

#ifndef DEBUGASSERTMSG
    #if DEBUGLEVEL == DEBUG_LEVEL_PRODUCTION
       /* no debugger message */
      #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value)
   #elif DEBUGLEVEL == DEBUG_LEVEL_BREAK_ONLY
     /* call Debugger() if it is available */
       #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
         Debugger()
 #elif DEBUGLEVEL == DEBUG_LEVEL_EXTERNAL
       /* exclude fileName and lineNumber */
      #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
         DebugAssert(componentSignature, options, assertionString, exceptionLabelString, errorString, 0, 0, (void*)value)
   #else
      /* include all info */
     #define DEBUGASSERTMSG(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
         DebugAssert(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, (void*)value)
   #endif
#endif

/*
 *  Define the three inputs to AssertMacros.h
 */

#ifndef DEBUG_ASSERT_COMPONENT_NAME_STRING
    #define DEBUG_ASSERT_COMPONENT_NAME_STRING kComponentSignatureString
#endif
#ifndef DEBUG_ASSERT_PRODUCTION_CODE
   #define DEBUG_ASSERT_PRODUCTION_CODE (DEBUGLEVEL==0)
#endif
#ifndef DEBUG_ASSERT_MESSAGE
   #define DEBUG_ASSERT_MESSAGE(componentNameString, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value) \
       DEBUGASSERTMSG(COMPONENT_SIGNATURE, DEBUG_NO_OPTIONS, componentNameString ": " assertionString, exceptionLabelString, errorString, fileName, lineNumber, value)
#endif

/*
 *  Include AssertMacros.h where all of the check, verify, and require macros are defined
 */
#include <AssertMacros.h>
/*
 *  The following check, verify, and require macros assert that TaskLevel is 0.
 */

#define ATTASKLEVEL0()                                                         \
  (TaskLevel() == 0)


#define check_tasklevel0()                                                     \
  check(ATTASKLEVEL0())


#define check_tasklevel0_string(cstring)                                       \
  check_string(ATTASKLEVEL0(), cstring)


#define verify_tasklevel0()                                                        \
  verify(ATTASKLEVEL0())


#define verify_tasklevel0_string(cstring)                                      \
  verify_string(ATTASKLEVEL0(), cstring)


#define require_tasklevel0(exceptionLabel)                                     \
  require(ATTASKLEVEL0(), exceptionLabel)


#define require_tasklevel0_action(exceptionLabel, action)                      \
  require_action(ATTASKLEVEL0(), exceptionLabel, action)


#define require_tasklevel0_quiet(exceptionLabel)                               \
  require_quiet(ATTASKLEVEL0(), exceptionLabel)


#define require_tasklevel0_action_quiet(exceptionLabel, action)                    \
  require_action_quiet(ATTASKLEVEL0(), exceptionLabel, action)


#define require_tasklevel0_string(exceptionLabel, cstring)                     \
  require_string(ATTASKLEVEL0(), exceptionLabel, cstring)


#define require_tasklevel0_action_string(exceptionLabel, action, cstring)      \
  require_action_string(ATTASKLEVEL0(), exceptionLabel, action, cstring)

/*
 *  SafeDebugger and SafeDebugStr work like Debugger and DebugStr in both
 *  production and non-preduction builds, but only call the debugger
 *  if the debugger is installed.
 */

#if CALL_NOT_IN_CARBON
    /* Make sure a low level debugger is installed on Mac OS 8 or 9 */
 #define SafeDebugger()                                                      \
      do                                                                      \
      {                                                                       \
          if ( ((*((UInt8 *)0x0BFF)) != (UInt8)0xff) &&                       \
               (((*((UInt8 *)0x0BFF)) & (UInt8)0xe0) == (UInt8)0x60) &&       \
               ((*((unsigned long *)0x0120)) != 0) &&                         \
               ((*((unsigned long *)0x0120)) != (unsigned long)0xffffffff) )  \
          {                                                                   \
              Debugger();                                                     \
          }                                                                   \
      } while ( false )
#else
    /* Debugger() is always safe on Carbon under Mac OS X */
   /* Otherwise we must make sure a low level debugger is installed */
    #define SafeDebugger()                                                      \
      do                                                                      \
      {                                                                       \
          long response;                                                      \
          if ( (Gestalt(gestaltSystemVersion, &response) == noErr) &&         \
               (response >= 0x1000) )                                         \
          {                                                                   \
              Debugger();                                                     \
          }                                                                   \
          else                                                                \
          {                                                                   \
              if ( ((*((UInt8 *)0x0BFF)) != (UInt8)0xff) &&                   \
                   (((*((UInt8 *)0x0BFF)) & (UInt8)0xe0) == (UInt8)0x60) &&   \
                   ((*((unsigned long *)0x0120)) != 0) &&                     \
                   ((*((unsigned long *)0x0120)) != (unsigned long)0xffffffff) ) \
               {                                                               \
                  Debugger();                                                 \
              }                                                               \
          }                                                                   \
      } while ( false )
#endif


#if CALL_NOT_IN_CARBON
    /* Make sure low level debugger is installed on Mac OS 8 or 9 */
   #define SafeDebugStr(str)                                                   \
      do                                                                      \
      {                                                                       \
          if ( ((*((UInt8 *)0x0BFF)) != (UInt8)0xff) &&                       \
               (((*((UInt8 *)0x0BFF)) & (UInt8)0xe0) == (UInt8)0x60) &&       \
               ((*((unsigned long *)0x0120)) != 0) &&                         \
               ((*((unsigned long *)0x0120)) != (unsigned long)0xffffffff) )  \
          {                                                                   \
              DebugStr(str);                                                  \
          }                                                                   \
      } while ( false )
#else
    /* DebugStr() is always safe on Carbon under Mac OS X */
   /* Otherwise we must make sure a low level debugger is installed */
    #define SafeDebugStr(str)                                                   \
      do                                                                      \
      {                                                                       \
          long response;                                                      \
          if ( (Gestalt(gestaltSystemVersion, &response) == noErr) &&         \
               (response >= 0x1000) )                                         \
          {                                                                   \
              DebugStr(str);                                                  \
          }                                                                   \
          else                                                                \
          {                                                                   \
              if ( ((*((UInt8 *)0x0BFF)) != (UInt8)0xff) &&                   \
                   (((*((UInt8 *)0x0BFF)) & (UInt8)0xe0) == (UInt8)0x60) &&   \
                   ((*((unsigned long *)0x0120)) != 0) &&                     \
                   ((*((unsigned long *)0x0120)) != (unsigned long)0xffffffff) ) \
               {                                                               \
                  DebugStr(str);                                              \
              }                                                               \
          }                                                                   \
      } while ( false )
#endif

/*
 *  DEBUGGER and DEBUGSTR call SafeDebugger and SafeDebugStr only
 *  in DEBUG builds.
 *
 *  DEBUGGER and DEBUGSTR are defined only if they are not already defined to
 *  prevent conflicts with developer code.
 */

#ifndef DEBUGGER
  #if DEBUG
      #define DEBUGGER()                                                      \
          SafeDebugger()
 #else
      #define DEBUGGER()
 #endif
#endif

#ifndef DEBUGSTR
    #if DEBUG
      #define DEBUGSTR(str)                                                       \
          SafeDebugStr(str)
  #else
      #define DEBUGSTR(str)
  #endif
#endif

/*
 *  CapsLockDebugger and CapsLockDebugStr call SafeDebugger and SafeDebugStr
 *  only if the Caps Lock key is down.
 */

#define CapsLockDebugger()                                                     \
  do                                                                          \
  {                                                                           \
      KeyMap theKeys;                                                         \
      GetKeys(theKeys);                                                       \
      if ( ((theKeys[1] >> 1) & 0x01) != 0 )                                  \
      {                                                                       \
          SafeDebugger();                                                     \
      }                                                                       \
  } while ( false )


#define CapsLockDebugStr(str)                                                  \
  do                                                                          \
  {                                                                           \
      KeyMap theKeys;                                                         \
      GetKeys(theKeys);                                                       \
      if ( ((theKeys[1] >> 1) & 0x01) != 0 )                                  \
      {                                                                       \
          SafeDebugStr(str);                                                  \
      }                                                                       \
  } while ( false )

/*
 *  ISCAPSLOCKKEYDOWN and ISLOWLEVELDEBUGGERCALLABLE are convenience macros
 *  which are only defined for non-Carbon builds because they access
 *  low memory addresses directly. They have been left in this file for
 *  use by existing non-Carbon code.
 *
 *  ISCAPSLOCKKEYDOWN and ISLOWLEVELDEBUGGERCALLABLE are defined only
 *  if they are not already defined to prevent conflicts with developer code.
 */

#if CALL_NOT_IN_CARBON
    #ifndef ISCAPSLOCKKEYDOWN
      #define ISCAPSLOCKKEYDOWN()                                             \
          ((((UInt16 *)(0x0174))[3] & 0x0002) == 0x0002)
 #endif
 
   #ifndef ISLOWLEVELDEBUGGERCALLABLE
     #define ISLOWLEVELDEBUGGERCALLABLE()                                    \
          ( ((*((UInt8 *)0x0BFF)) != (UInt8)0xff) &&                          \
            (((*((UInt8 *)0x0BFF)) & (UInt8)0xe0) == (UInt8)0x60) &&          \
            ((*((unsigned long *)0x0120)) != 0) &&                            \
            ((*((unsigned long *)0x0120)) != (unsigned long)0xffffffff) )
    #endif
#endif

/*
 *  You can use DPRINTF as a dprintf which goes away in production builds.
 *  DPRINTF is not supported by Carbon because dprintf
 *  is not supported by Carbon.
 *
 *  To use it, double-parenthesize the argument - i.e., use:
 *
 *      DPRINTF(("formatString %d", 5 ));
 *
 *  This is sadly necessary because a macro can not have a variable number
 *  of arguments.
 *
 *  DPRINTF is defined only if it is not already defined to
 *  prevent conflicts with developer code.
 */

#if DEBUG
    #define DPRINTF(x)   dprintf x
#else
    #define DPRINTF(x)  { }
#endif


/*
 *  kBlessedBusErrorBait is an address that will never be mapped by
 *  Mac OS 8 or 9. It is close to the middle of the 64K range from 0x68F10000
 *  to 0x68F1FFFF that is unmapped and cannot be accessed without causing an
 *  exception. Thus, it's a good value to use for filling uninitialized
 *  pointers, etc.
 *
 *  Mac OS X programs should NOT use kBlessedBusErrorBait and should use
 *  zero (0) instead, since by default, page 0 is read and write protected
 *  for user code.
 */
enum {
  kBlessedBusErrorBait          = 0x68F168F1
};

/*
 *  DebugAssert()
 *  
 *  Summary:
 *    DebugAssert is the system routine that the DEBUGASSERTMSG macro
 *    calls (by default) to display assertion messsages. The output
 *    from DebugAssert can be redirected by installing a
 *    DebugAssertOutputHandler with InstallDebugAssertOutputHandler.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of component causing the assertion.
 *    
 *    options:
 *      reserved.
 *    
 *    assertionString:
 *      A pointer to a string containing the assertion, or NULL.
 *    
 *    exceptionLabelString:
 *      A pointer to a string containing the exceptionLabel, or NULL.
 *    
 *    errorString:
 *      A pointer to the error string, or NULL.
 *    
 *    fileName:
 *      A pointer to the fileName or pathname (generated by the
 *      preprocessor __FILE__ identifier), or NULL.
 *    
 *    lineNumber:
 *      The line number in the file (generated by the preprocessor
 *      __LINE__ identifier), or 0 (zero).
 *    
 *    value:
 *      A value associated with the assertion, or NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DebugAssert(
  OSType        componentSignature,
  UInt32        options,
  const char *  assertionString,
  const char *  exceptionLabelString,
  const char *  errorString,
  const char *  fileName,
  long          lineNumber,
  void *        value)                                        TWOWORDINLINE(0x7000, 0xAA7E);


/*
 *  TaskLevel masks
 */
enum {
  k68kInterruptLevelMask        = 0x00000007, /* 68K interrupt levels 0 through 7 */
  kInVBLTaskMask                = 0x00000010, /* VBLs are executing */
  kInDeferredTaskMask           = 0x00000020, /* Deferred tasks are executing */
  kInSecondaryIntHandlerMask    = 0x00000040, /* Secondary interrupt handlers are executing */
  kInNestedInterruptMask        = 0x00000080 /* The system is handling an interrupt */
};

/*
 *  TaskLevel()
 *  
 *  Summary:
 *    TaskLevel returns 0 if we're (probably) running at non-interrupt
 *    time. There's no way to make this perfect, but this is as close
 *    as we can get. If TaskLevel doesn't return 0, then one of the
 *    TaskLevel masks can be used to learn more.
 *  
 *  Result:
 *    The current task level.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
TaskLevel(void)                                               TWOWORDINLINE(0x7001, 0xAA7E);


/*
 *  Constants used by the DebugComponent functions
 */
enum {
  kComponentDebugOption         = 0     /* optionSelectorNum to turn breaks for component On or Off*/
};

enum {
  kGetDebugOption               = 1,    /* get current debug option setting*/
  kSetDebugOption               = 2     /* set debug option*/
};


/*
 *  DebugComponentCallbackProcPtr
 *  
 *  Discussion:
 *    DebugComponentCallback is the callback into a component that
 *    registers with DebugLib. It is called to get the debug option
 *    setting, or to turn a debug option on or off.
 *  
 *  Parameters:
 *    
 *    optionSelectorNum:
 *      The component debug option to set.
 *    
 *    command:
 *      The command the DebugComponentCallbackProc must handle:
 *       kGetDebugOption - get current debug option setting
 *       kSetDebugOption - set debug option
 *    
 *    optionSetting:
 *      A pointer to a Boolean where the DebugComponentCallbackProc
 *      must return the option setting: the current setting if command
 *      is kGetDebugOption; the new debug option if command is 
 *      kSetDebugOption
 */
typedef CALLBACK_API( void , DebugComponentCallbackProcPtr )(SInt32 optionSelectorNum, UInt32 command, Boolean *optionSetting);
typedef STACK_UPP_TYPE(DebugComponentCallbackProcPtr)           DebugComponentCallbackUPP;
/*
 *  NewDebugComponent()
 *  
 *  Summary:
 *    NewDebugComponent registers a component with DebugLib.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of component.
 *    
 *    componentName:
 *      The displayable string naming the component.
 *    
 *    componentCallback:
 *      The callback into component for working with options.
 *  
 *  Result:
 *    An operating system result code: noErr, memFullErr,
 *    debuggingExecutionContextErr, debuggingDuplicateSignatureErr, or
 *    debuggingInvalidNameErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
NewDebugComponent(
  OSType                      componentSignature,
  ConstStr255Param            componentName,
  DebugComponentCallbackUPP   componentCallback)              TWOWORDINLINE(0x7002, 0xAA7E);


/*
 *  NewDebugOption()
 *  
 *  Summary:
 *    NewDebugOption registers a debug option with DebugLib.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The signature of component to register a debug option for.
 *    
 *    optionSelectorNum:
 *      The selector number of this debug option.
 *    
 *    optionName:
 *      The displayable string naming this debug option.
 *  
 *  Result:
 *    An operating system result code: noErr, memFullErr,
 *    debuggingExecutionContextErr, debuggingDuplicateOptionErr,
 *    debuggingInvalidSignatureErr, debuggingInvalidNameErr, or
 *    debuggingNoCallbackErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
NewDebugOption(
  OSType             componentSignature,
  SInt32             optionSelectorNum,
  ConstStr255Param   optionName)                              TWOWORDINLINE(0x7003, 0xAA7E);


/*
 *  DisposeDebugComponent()
 *  
 *  Summary:
 *    DisposeDebugComponent removes a component registration and all
 *    related debug options from DebugLib.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of a component.
 *  
 *  Result:
 *    An operating system result code: noErr,
 *    debuggingExecutionContextErr, or debuggingInvalidSignatureErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DisposeDebugComponent(OSType componentSignature)              TWOWORDINLINE(0x7004, 0xAA7E);


/*
 *  GetDebugComponentInfo()
 *  
 *  Summary:
 *    GetDebugComponentInfo returns a component registered with
 *    DebugLib.
 *  
 *  Parameters:
 *    
 *    index:
 *      The index into the list of registered components (1-based).
 *    
 *    componentSignature:
 *      A pointer to an OSType where the unique signature of a
 *      component is returned.
 *    
 *    componentName:
 *      A pointer to an Str255 where the displayable string naming a
 *      component is returned.
 *  
 *  Result:
 *    An operating system result code: noErr or debuggingNoMatchErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDebugComponentInfo(
  UInt32    index,
  OSType *  componentSignature,
  Str255    componentName)                                    TWOWORDINLINE(0x7005, 0xAA7E);


/*
 *  GetDebugOptionInfo()
 *  
 *  Summary:
 *    GetDebugOptionInfo returns a debug option registered with
 *    DebugLib.
 *  
 *  Parameters:
 *    
 *    index:
 *      The index into the list of registered debug options (0-based);
 *      0 = kComponentDebugOption.
 *    
 *    componentSignature:
 *      The unique signature of a component.
 *    
 *    optionSelectorNum:
 *      A pointer to an SInt32 where the selector number of this debug
 *      option is returned.
 *    
 *    optionName:
 *      A pointer to an Str255 where the displayable string naming this
 *      debug option is returned.
 *    
 *    optionSetting:
 *      A pointer to an Boolean where the current debug option setting
 *      is returned.
 *  
 *  Result:
 *    An operating system result code: noErr,
 *    debuggingInvalidSignatureErr, or debuggingNoMatchErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetDebugOptionInfo(
  UInt32     index,
  OSType     componentSignature,
  SInt32 *   optionSelectorNum,
  Str255     optionName,
  Boolean *  optionSetting)                                   TWOWORDINLINE(0x7006, 0xAA7E);


/*
 *  SetDebugOptionValue()
 *  
 *  Summary:
 *    SetDebugOptionValue sets a debug option registered with DebugLib.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of a component.
 *    
 *    optionSelectorNum:
 *      The selector number of this debug option.
 *    
 *    newOptionSetting:
 *      The new debug option setting.
 *  
 *  Result:
 *    An operating system result code: noErr,
 *    debuggingInvalidSignatureErr, or debuggingInvalidOptionErr.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetDebugOptionValue(
  OSType    componentSignature,
  SInt32    optionSelectorNum,
  Boolean   newOptionSetting)                                 TWOWORDINLINE(0x7007, 0xAA7E);



/*
 *  DebugAssertOutputHandlerProcPtr
 *  
 *  Discussion:
 *    DebugAssertOutputHandler is the callback that registers with
 *    DebugLib to handle the output from DebugAssert. The
 *    "componentSignature" through "value" parameters are the raw
 *    values passed to DebugAssert when an exception occurs.
 *  
 *  Parameters:
 *    
 *    componentSignature:
 *      The unique signature of component causing the assertion.
 *    
 *    options:
 *      reserved.
 *    
 *    assertionString:
 *      A pointer to a string containing the assertion, or NULL.
 *    
 *    exceptionLabelString:
 *      A pointer to a string containing the exceptionLabel, or NULL.
 *    
 *    errorString:
 *      A pointer to the error string, or NULL.
 *    
 *    fileName:
 *      A pointer to the fileName or pathname (generated by the
 *      preprocessor __FILE__ identifier), or NULL.
 *    
 *    lineNumber:
 *      The line number in the file (generated by the preprocessor
 *      __LINE__ identifier), or 0 (zero).
 *    
 *    value:
 *      A value associated with the assertion, or NULL.
 *    
 *    outputMsg:
 *      The string DebugAssert build which would normally be passed to
 *      DebugStr if a DebugAssertOutputHandler isn't installed.
 */
typedef CALLBACK_API( void , DebugAssertOutputHandlerProcPtr )(OSType componentSignature, UInt32 options, const char *assertionString, const char *exceptionLabelString, const char *errorString, const char *fileName, long lineNumber, void *value, ConstStr255Param outputMsg);
typedef STACK_UPP_TYPE(DebugAssertOutputHandlerProcPtr)         DebugAssertOutputHandlerUPP;
/*
 *  InstallDebugAssertOutputHandler()
 *  
 *  Summary:
 *    InstallDebugAssertOutputHandler installs a
 *    DebugAssertOutputHandler which DebugAssert calls instead of
 *    DebugStr.
 *  
 *  Parameters:
 *    
 *    handler:
 *      The DebugAssertOutputHandler to install or NULL to switch back
 *      to the default handler (DebugStr).
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InstallDebugAssertOutputHandler(DebugAssertOutputHandlerUPP handler) TWOWORDINLINE(0x7008, 0xAA7E);


#if CALL_NOT_IN_CARBON
/*
 *  dprintf()
 *  
 *  Summary:
 *    printf takes a variable argument list and 'prints' that to the
 *    debugging output handler.  Calling dprintf() from anything but C
 *    or C++ is tricky.
 *  
 *  Parameters:
 *    
 *    format:
 *      The format string.
 *    
 *    ...:
 *      The arguments to the format string.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
dprintf(const char * format, ...)                             SIXWORDINLINE(0x2057, 0x43EF, 0x0004, 0x303C, 0x000A, 0xAA7E);


/*
 *  vdprintf()
 *  
 *  Summary:
 *    vdprintf takes a va_args list and 'prints' that to the debugging
 *    output handler.
 *  
 *  Parameters:
 *    
 *    format:
 *      The format string.
 *    
 *    va_args_list:
 *      The va_args list.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in DebugLib 1.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
vdprintf(
  const char *  format,
  char *        va_args_list)                                 TWOWORDINLINE(0x7009, 0xAA7E);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  NewDebugComponentCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DebugComponentCallbackUPP )
NewDebugComponentCallbackUPP(DebugComponentCallbackProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDebugComponentCallbackProcInfo = 0x00000FC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DebugComponentCallbackUPP) NewDebugComponentCallbackUPP(DebugComponentCallbackProcPtr userRoutine) { return (DebugComponentCallbackUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDebugComponentCallbackProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDebugComponentCallbackUPP(userRoutine) (DebugComponentCallbackUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDebugComponentCallbackProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewDebugAssertOutputHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( DebugAssertOutputHandlerUPP )
NewDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppDebugAssertOutputHandlerProcInfo = 0x00FFFFC0 };  /* pascal no_return_value Func(4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(DebugAssertOutputHandlerUPP) NewDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerProcPtr userRoutine) { return (DebugAssertOutputHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDebugAssertOutputHandlerProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewDebugAssertOutputHandlerUPP(userRoutine) (DebugAssertOutputHandlerUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppDebugAssertOutputHandlerProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeDebugComponentCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDebugComponentCallbackUPP(DebugComponentCallbackUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDebugComponentCallbackUPP(DebugComponentCallbackUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDebugComponentCallbackUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeDebugAssertOutputHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeDebugAssertOutputHandlerUPP(DebugAssertOutputHandlerUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeDebugAssertOutputHandlerUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeDebugComponentCallbackUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDebugComponentCallbackUPP(
  SInt32                     optionSelectorNum,
  UInt32                     command,
  Boolean *                  optionSetting,
  DebugComponentCallbackUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDebugComponentCallbackUPP(SInt32 optionSelectorNum, UInt32 command, Boolean * optionSetting, DebugComponentCallbackUPP userUPP) { CALL_THREE_PARAMETER_UPP(userUPP, uppDebugComponentCallbackProcInfo, optionSelectorNum, command, optionSetting); }
  #else
    #define InvokeDebugComponentCallbackUPP(optionSelectorNum, command, optionSetting, userUPP) CALL_THREE_PARAMETER_UPP((userUPP), uppDebugComponentCallbackProcInfo, (optionSelectorNum), (command), (optionSetting))
  #endif
#endif

/*
 *  InvokeDebugAssertOutputHandlerUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeDebugAssertOutputHandlerUPP(
  OSType                       componentSignature,
  UInt32                       options,
  const char *                 assertionString,
  const char *                 exceptionLabelString,
  const char *                 errorString,
  const char *                 fileName,
  long                         lineNumber,
  void *                       value,
  ConstStr255Param             outputMsg,
  DebugAssertOutputHandlerUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeDebugAssertOutputHandlerUPP(OSType componentSignature, UInt32 options, const char * assertionString, const char * exceptionLabelString, const char * errorString, const char * fileName, long lineNumber, void * value, ConstStr255Param outputMsg, DebugAssertOutputHandlerUPP userUPP) { CALL_NINE_PARAMETER_UPP(userUPP, uppDebugAssertOutputHandlerProcInfo, componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value, outputMsg); }
  #else
    #define InvokeDebugAssertOutputHandlerUPP(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value, outputMsg, userUPP) CALL_NINE_PARAMETER_UPP((userUPP), uppDebugAssertOutputHandlerProcInfo, (componentSignature), (options), (assertionString), (exceptionLabelString), (errorString), (fileName), (lineNumber), (value), (outputMsg))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewDebugComponentCallbackProc(userRoutine)          NewDebugComponentCallbackUPP(userRoutine)
    #define NewDebugAssertOutputHandlerProc(userRoutine)        NewDebugAssertOutputHandlerUPP(userRoutine)
    #define CallDebugComponentCallbackProc(userRoutine, optionSelectorNum, command, optionSetting) InvokeDebugComponentCallbackUPP(optionSelectorNum, command, optionSetting, userRoutine)
    #define CallDebugAssertOutputHandlerProc(userRoutine, componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value, outputMsg) InvokeDebugAssertOutputHandlerUPP(componentSignature, options, assertionString, exceptionLabelString, errorString, fileName, lineNumber, value, outputMsg, userRoutine)
#endif /* CALL_NOT_IN_CARBON */


#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DEBUGGING__ */

