/*
     File:       HIObject.h
 
     Contains:   Base object for HIToolbox
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __HIOBJECT__
#define __HIOBJECT__

#ifndef __CORESERVICES__
#include <CoreServices.h>
#endif

#ifndef __COREFOUNDATION__
#include <CoreFoundation.h>
#endif

#ifndef __CARBONEVENTSCORE__
#include <CarbonEventsCore.h>
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
 *  HIObject
 *  
 *  Discussion:
 *    HIObject is the HIToolbox's base class for various objects. Over
 *    time, all of our common objects (controls, windows, menus, etc.)
 *    will be derived from HIObject. Code which is external to
 *    HIToolbox can also create its own subclasses of our objects using
 *    the routines contained in this file. There are also polymorphic
 *    functions one can use on any HIObject for getting the class ID,
 *    etc. 
 *    
 *    HIObjects are actually CF types under the hood. This means that
 *    they can be put into CF collections and retain/release can be
 *    called on them. 
 *    
 *    An HIObject is essentially a very basic building-block object
 *    which contains an event target. You can create these objects to
 *    use as your own Carbon Event receptors in your application, or
 *    you can subclass existing Toolbox object to suit your needs.
 *    
 *    
 *    You register your subclasses with HIObjectRegisterSubclass,
 *    passing your class ID, the parent class, and an event handler.
 *    You also pass a list of events the handler is interested in.
 *    
 *    
 *    To create an object of your subclass, you call HIObjectCreate,
 *    passing the class ref you registered, as well as an
 *    initialization event. 
 *    
 *    Construction is two-phase: first the basic construction of the
 *    object is done, then initialization is performed. The toolbox
 *    sends construction events bottom-up, as you would expect in C++
 *    or the like. Here is the list of what goes on to create an
 *    object: 
 *    
 *    1)  The Toolbox creates the base HIObject 
 *    
 *    2)  It then installs the event handler you specified when you
 *    registered your subclass. Your handler must listen for
 *    kEventHIObjectConstruct and kEventHIObjectDestruct events. If it
 *    does not, the class cannot be registered (you will get a
 *    paramErr). 
 *    
 *    3)  Next, the Toolbox _directly_ calls your handler with an
 *    kEventHIObjectConstruct event. When called like this, you are not
 *    really being called in the context of a handler stack, so you
 *    cannot do things like CallNextEventHandler. The userData
 *    parameter is what you specified when you registered the class.
 *    Typically, during construction you will allocate memory yourself
 *    to store your own instance data; this allocation might be as
 *    simple as calling malloc or NewPtr, or it might involve creating
 *    your own C++ object. In the construct event, you are passed the
 *    base HIObjectRef of the object being created. Typically you would
 *    store this HIObjectRef in your own instance data for later use.
 *    When handling this construct event, you should be sure to use
 *    SetEventParameter to set the kEventParamHIObjectInstance
 *    parameter in the construction event with your own instance data.
 *    You must use typeVoidPtr as the type. 
 *    
 *    4)  The Toolbox looks for your instance of typeVoidPtr after you
 *    handle the construct event. It then takes that data and stores it
 *    off with the object and also sets the user data of the event
 *    handler it installed to be this instance data. This means that
 *    following the construct event, all calls to your event handler
 *    will have the instance data you returned to us. 
 *    
 *    5)  Once construction has completed successfully, we will send
 *    your object the initialize event passed into HIObjectCreate. At
 *    this point, all events are now sent to your object using standard
 *    Carbon Events mechanisms (it is only the construct event which is
 *    special). When we send the initialization event to your subclass,
 *    you should pass the event to your superclass before proceeding.
 *    You do this with CallNextEventHandler. Once back from that call,
 *    you should verify that the result is noErr, indicating that the
 *    superclass did in fact initialize properly. If it did not, your
 *    should return the error that CallNextEventHandler returned from
 *    your handler as well. The object will be destroyed by the
 *    Toolbox. Your object should be able to be destroyed in a
 *    partially initialized state such as this. This stage is optional,
 *    i.e. an object does not need to respond to the initialize event
 *    unless it is expecting certain parameters to be passed to it at
 *    creation time. This is where those parameters can be fetched.
 *    
 *    
 *    6)  Once initialization is successful, the HIObjectRef is
 *    returned to the caller of HIObjectCreate. From there, you can
 *    have all sorts of cool fun. 
 *    
 *    When someone has called HIObjectRelease enough such that the
 *    refcount of the object drops to zero, the object is destroyed.
 *    The Toolbox will send a kEventHIObjectDestruct event to your
 *    object. DO NOT CALL CALLNEXTEVENTHANDLER. You will be setting
 *    yourself up for some hurt. Just clean up and return from your
 *    handler.
 */
typedef struct OpaqueHIObjectClassRef*  HIObjectClassRef;
typedef struct OpaqueHIObjectRef*       HIObjectRef;

/*
 *  Discussion:
 *    HIObject errors
 */
enum {

  /*
   * You are trying to register a class ID that already exists.
   */
  hiObjectClassExistsErr        = -22080,

  /*
   * You are trying to unregister a class which has instances which
   * still exist. You must destroy them first, before they destroy you!
   */
  hiObjectClassHasInstancesErr  = -22081,
  hiObjectClassHasSubclassesErr = -22082,

  /*
   * You are trying to create an HIObject class that is defined as
   * being abstract. You must subclass it instead. Oh yes. Don't make
   * us say it twice!
   */
  hiObjectClassIsAbstractErr    = -22083
};



/*
    Parameters for HIObject events:

    kEventHIObjectConstruct
        -->     kEventParamHIObjectInstance             typeHIObjectRef
    
    kEventHIObjectInitialize
        This is up to the class and any superclasses. It will contain the
        union of all parameters needed by all classes to properly construct.
    
    kEventHIObjectDestruct
        No parameters are passed.
    
    kEventHIObjectIsEqual
        -->     kEventParamDirectObject             typeHIObjectRef

    kEventHIObjectPrintDebugInfo
        no parameters
*/

/*
 *  Discussion:
 *    These enums define the base class functionality of HIObjects. You
 *    should only need to be aware of these if you are implementing a
 *    subclass.
 */
enum {

  /*
   * The event class for HIObject events
   */
  kEventClassHIObject           = FOUR_CHAR_CODE('hiob'),

  /*
   * Your object is being constructed. When your event handler is
   * called with this event, it is being called directly and not
   * through the normal event dispatching mechanism. This means that
   * the EventHandlerCallRef passed to your handler will be NULL and
   * CallNextEventHandler will not work. You are passed the actual
   * HIObjectRef of your base class for you to record in your instance
   * data.
   */
  kEventHIObjectConstruct       = 1,

  /*
   * Your object is being initialized. Your handler should pass this
   * onto the superclass first before handling this event. This is done
   * by calling CallNextEventHandler with the event. When that function
   * returns, you should make sure the result is noErr. If not, you
   * should NOT continue to initialize your class.
   */
  kEventHIObjectInitialize      = 2,

  /*
   * Your object is being destroyed. This is your chance to dispose of
   * anything you might have allocated for your object. Do NOT call
   * through with CallNextEventHandler, as you will disrupt the fabric
   * of space-time.
   */
  kEventHIObjectDestruct        = 3,

  /*
   * HIObjectIsEqual has been called, and you are being asked to
   * determine if your object is equivalent to the one being passed to
   * your handler. You should return true if so, and false if not.
   */
  kEventHIObjectIsEqual         = 4,

  /*
   * HIObjectPrintDebugInfo has been called, and you are being asked to
   * print your information to stdout. This event is sent to all
   * handlers and you should NOT call CallNextEventHandler.
   */
  kEventHIObjectPrintDebugInfo  = 5
};

enum {
  kEventParamHIObjectInstance   = FOUR_CHAR_CODE('hioi'),
  typeHIObjectRef               = FOUR_CHAR_CODE('hiob')
};


#define _HIObjectRegisterSubclass HIObjectRegisterSubclass
/*
 *  HIObjectRegisterSubclass()
 *  
 *  Discussion:
 *    Registers a class with the Toolbox for creation later.
 *  
 *  Parameters:
 *    
 *    inClassID:
 *      The class ID of your class. It should be unique. We recommend
 *      using Java-style com.company.foo naming conventions to avoid
 *      collisions.
 *    
 *    inBaseClassID:
 *      The class ID of the class you derive from. Passing NULL
 *      indicates you wish to subclass HIObject (the base class)
 *      directly.
 *    
 *    inOptions:
 *      Any special options for your class. Currently you must pass 0
 *      for this parameter.
 *    
 *    inConstructProc:
 *      The construction proc for this subclass. You pass the address
 *      of an event handler into this parameter. This handler is called
 *      directly, rather than through the normal event dispatching
 *      mechanism. This means that the EventHandlerCallRef passed in
 *      will be NULL, and you cannot use it for calls like
 *      CallNextEventHandler. Other than that, you should return a
 *      result as usual. After your object is constructed, this proc
 *      will be installed as the event handler for the remaining events
 *      specified in the inEventList parameter.
 *    
 *    inNumEvents:
 *      The number of events you are installing.
 *    
 *    inEventList:
 *      The events your handler wishes to receive. You must handle the
 *      kEventHIObjectConstruct and kEventHIObjectDestruct event. If
 *      these events are not specified, an error is returned.
 *    
 *    inConstructData:
 *      Pass any info you want passed into your event handler here. For
 *      a C++ hierarchy based on HIObjects, you might actually pass a
 *      static method to construct your object here, and the base class
 *      event handler to do construction as your event handler.
 *    
 *    outClassRef:
 *      The newly created class reference. Pass NULL if you don't care.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIObjectRegisterSubclass(
  CFStringRef            inClassID,
  CFStringRef            inBaseClassID,
  OptionBits             inOptions,
  EventHandlerUPP        inConstructProc,
  UInt32                 inNumEvents,
  const EventTypeSpec *  inEventList,
  void *                 inConstructData,
  HIObjectClassRef *     outClassRef);          /* can be NULL */


/*
 *  HIObjectUnregisterClass()
 *  
 *  Discussion:
 *    Unregisters a previously registered subclass of HIObject. You
 *    will receive an error if there are subclasses of your class or
 *    instances of it which still exist. All instances and subclasses
 *    must be disposed of and unregistered first.
 *  
 *  Parameters:
 *    
 *    inClassRef:
 *      The class ref of the class of object you wish to unregister.
 *    
 *    inConstructData:
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIObjectUnregisterClass(HIObjectClassRef inClassRef);


#define _HIObjectCreate HIObjectCreate
/*
 *  HIObjectCreate()
 *  
 *  Discussion:
 *    Creates an object derived from HIObject.
 *  
 *  Parameters:
 *    
 *    inClassID:
 *      The class ID of the class of object you wish to instantiate.
 *    
 *    inConstructData:
 *      If your class (or any class you derive from) accepts creation
 *      parameters, you need to pass an event into this parameter. The
 *      class must be kEventClassHIObject, and the kind should be
 *      kEventHIObjectInitialize. Any other parameters should be added
 *      as necessary. Specific subclasses of HIObject which require
 *      initialization parameters will specify those parameters in the
 *      appropriate headers.
 *    
 *    outObject:
 *      The instance of the object you create.
 *  
 *  Result:
 *    An operating system result code.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIObjectCreate(
  CFStringRef    inClassID,
  EventRef       inConstructData,
  HIObjectRef *  outObject);




#define _HIObjectGetEventTarget HIObjectGetEventTarget
/*
 *  HIObjectGetEventTarget()
 *  
 *  Discussion:
 *    Returns the event target of an HIObjectRef.
 *  
 *  Parameters:
 *    
 *    inObject:
 *      The object whose target you want.
 *  
 *  Result:
 *    An EventTargetRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( EventTargetRef )
HIObjectGetEventTarget(HIObjectRef inObject);


#define _HIObjectPrintDebugInfo HIObjectPrintDebugInfo
/*
 *  HIObjectPrintDebugInfo()
 *  
 *  Discussion:
 *    Prints the internal information of an HIObject for debugging
 *    purposes. It outputs the info to stdout.
 *  
 *  Parameters:
 *    
 *    inObject:
 *      The object to inspect.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
HIObjectPrintDebugInfo(HIObjectRef inObject);


#define _HIObjectCopyClassID HIObjectCopyClassID
/*
 *  HIObjectCopyClassID()
 *  
 *  Discussion:
 *    Returns the class ID of a given HIObject.
 *  
 *  Parameters:
 *    
 *    inObject:
 *      The object whose class ID you are interested in.
 *  
 *  Result:
 *    A CFStringRef containing the object's class ID.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( CFStringRef )
HIObjectCopyClassID(HIObjectRef inObject);


#define _HIObjectIsOfClass HIObjectIsOfClass
/*
 *  HIObjectIsOfClass()
 *  
 *  Discussion:
 *    Returns whether or not an object is of a certain class. You can
 *    us this to see whether or not an object you have derives from an
 *    expected superclass.
 *  
 *  Parameters:
 *    
 *    inObject:
 *      The object whose class ID you wish to check.
 *    
 *    inObjectClassID:
 *      The class ID in question.
 *  
 *  Result:
 *    A Boolean result indicating whether or not the object is of the
 *    class specified.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIObjectIsOfClass(
  HIObjectRef   inObject,
  CFStringRef   inObjectClassID);


#define _HIObjectDynamicCast HIObjectDynamicCast
/*
 *  HIObjectDynamicCast()
 *  
 *  Discussion:
 *    Returns the instance data for a specific class of an HIObject.
 *    The instance data returned is the same instance data the class's
 *    construction event handler returns in the instance data
 *    parameter. This is stored off with the class reference so that it
 *    can be fetched later for use by this function. It allows your
 *    subclass to easily get at the data it created, if your subclass
 *    needs that data outside of an event handler. (Inside an event
 *    handler, your subclass can get at its instance data via the
 *    userData parameter to the event handler.)
 *  
 *  Parameters:
 *    
 *    inObject:
 *      The object whose class ID you wish to check.
 *    
 *    inClassID:
 *      The class ID to get the instance data for.
 *  
 *  Result:
 *    A void * result which contains the instance data for the object,
 *    or NULL if the object is not an instance of the class.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void * )
HIObjectDynamicCast(
  HIObjectRef   inObject,
  CFStringRef   inClassID);


/*
 *  HIObjectCreateFromBundle()
 *  
 *  Discussion:
 *    Returns the HIObject for the given bundle. A bundle can be
 *    designed to communicate with an app through an HIObject. The
 *    bundle must be designed to create an HIObject and have a defined
 *    suite of CarbonEvents that clients can use to communicate with
 *    the bundle's HIObject. Given a CFBundleRef, this API will tell
 *    the bundle to create the HIObject and return it to the caller.
 *  
 *  Parameters:
 *    
 *    inBundle:
 *      The bundle that you wish to communicate with.
 *    
 *    outObject:
 *      The HIObject associated with the bundle.
 *  
 *  Result:
 *    An operating system result code. If the bundle's HIObject
 *    creation function cannot be found, cfragNoSymbolErr will be
 *    returned.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIObjectCreateFromBundle(
  CFBundleRef    inBundle,
  HIObjectRef *  outObject);


/*
 *  HIObjectIsAccessibilityIgnored()
 *  
 *  Discussion:
 *    Reports whether or not the given HIObject is marked as ignored
 *    for accessibility. 
 *    See the discussion of HIObjectSetAccessibilityIgnored for details
 *    on what it means to be accessibility ignored.
 *  
 *  Parameters:
 *    
 *    inObject:
 *      The object whose accessibility ignored state you wish to query.
 *  
 *  Result:
 *    A Boolean value indicating whether or not the HIObject is ignored
 *    for accessibility.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIObjectIsAccessibilityIgnored(HIObjectRef inObject);


/*
 *  HIObjectSetAccessibilityIgnored()
 *  
 *  Discussion:
 *    Marks an HIObject as ignored (or not) for the purposes of the
 *    accessibility APIs. 
 *    An HIObject that is ignored for accessibility will never be shown
 *    to an assistive application that uses the accessibility APIs to
 *    examine an interface. Your application's accessibility
 *    implementation can (and should) still report an ignored HIObject
 *    as usual. Carbon's accessibility engine will automatically prune
 *    any ignored HIObjects out of the data that is shown to an
 *    assistive application. 
 *    By default, an HIObject is *not* accessibility ignored.
 *  
 *  Parameters:
 *    
 *    inObject:
 *      The object whose accessibility ignored state you wish to change.
 *    
 *    inIgnored:
 *      A Boolean value indicating whether or not to ignore the object.
 *  
 *  Result:
 *    An OSStatus signifying success or failure.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( OSStatus )
HIObjectSetAccessibilityIgnored(
  HIObjectRef   inObject,
  Boolean       inIgnored);


/*==============================================================================*/
/*  DEPRECATED! DO NOT USE. USE CF ROUTINES INSTEAD!!!                          */
/*==============================================================================*/
/* Use CFRetain instead!*/
/*
 *  _HIObjectRetain()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIObjectRef )
_HIObjectRetain(HIObjectRef inObject);


/* Use CFRelease instead!*/
/*
 *  _HIObjectRelease()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( void )
_HIObjectRelease(HIObjectRef inObject);


/* Use CFGetRetainCount instead!*/
/*
 *  _HIObjectGetRetainCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( UInt32 )
_HIObjectGetRetainCount(HIObjectRef inObject);


/* Use CFEqual instead!*/
/*
 *  _HIObjectIsEqual()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
_HIObjectIsEqual(
  HIObjectRef   inObject,
  HIObjectRef   inOtherObject);


/*
   These are no longer necessary! Just put the HIObjectRef directly into
   an array using the standard CFType callbacks.
*/
/*
 *  kHIObjectCFArrayCallbacks
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
extern const CFArrayCallBacks kHIObjectCFArrayCallbacks;
/*
 *  kHIObjectCFDictKeyCallbacks
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
extern const CFDictionaryKeyCallBacks kHIObjectCFDictKeyCallbacks;
/*
 *  kHIObjectCFDictValueCallbacks
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
extern const CFDictionaryValueCallBacks kHIObjectCFDictValueCallbacks;



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HIOBJECT__ */

