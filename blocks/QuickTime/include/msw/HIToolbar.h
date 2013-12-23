/*
     File:       HIToolbar.h
 
     Contains:   Toolbar and Toolbar Item API
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __HITOOLBAR__
#define __HITOOLBAR__

#ifndef __ICONS__
#include <Icons.h>
#endif

#ifndef __HIOBJECT__
#include <HIObject.h>
#endif

#ifndef __MENUS__
#include <Menus.h>
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

typedef HIObjectRef                     HIToolbarRef;
typedef HIObjectRef                     HIToolbarItemRef;
/*----------------------------------------------------------------------------------*/
/* Standard Toolbox-provided item identifiers                                       */
/*----------------------------------------------------------------------------------*/

#define kHIToolbarSeparatorIdentifier   CFSTR("com.apple.hitoolbox.toolbar.separator")
#define kHIToolbarSpaceIdentifier       CFSTR("com.apple.hitoolbox.toolbar.space")
#define kHIToolbarFlexibleSpaceIdentifier  CFSTR("com.apple.hitoolbox.toolbar.flexiblespace")
#define kHIToolbarCustomizeIdentifier   CFSTR("com.apple.hitoolbox.toolbar.customize")
#define kHIToolbarPrintItemIdentifier   CFSTR("com.apple.hitoolbox.toolbar.print")
#define kHIToolbarFontsItemIdentifier   CFSTR("com.apple.hitoolbox.toolbar.fonts")



/*
 *  Summary:
 *    Toolbar Display Mode
 */
enum {

  /*
   * This indicates to use the default display mode. Currently, this is
   * defined as being both icon and label, but could change in the
   * future.
   */
  kHIToolbarDisplayModeDefault  = 0,

  /*
   * This indicates to display the image as well as the label of the
   * toolbar items.
   */
  kHIToolbarDisplayModeIconAndLabel = 1,

  /*
   * This indicates that only the image should be shown.
   */
  kHIToolbarDisplayModeIconOnly = 2,

  /*
   * This indicates that only the label should be shown.
   */
  kHIToolbarDisplayModeLabelOnly = 3
};

typedef UInt32                          HIToolbarDisplayMode;

/*
 *  Summary:
 *    Toolbar Display Size
 */
enum {

  /*
   * This indicates to use the default display size. Currently, this is
   * defined as using 32 x 32 icons ("normal" size).
   */
  kHIToolbarDisplaySizeDefault  = 0,

  /*
   * This size uses a larger text and icon size.
   */
  kHIToolbarDisplaySizeNormal   = 1,

  /*
   * This size uses a smaller text and icon size.
   */
  kHIToolbarDisplaySizeSmall    = 2
};

typedef UInt32                          HIToolbarDisplaySize;

/*
 *  Summary:
 *    Toolbar Attributes
 */
enum {

  /*
   * Pass this to indicate no attributes at all.
   */
  kHIToolbarNoAttributes        = 0,

  /*
   * Pass this attribute to allow the toolbar to save its configuration
   * automatically to your application's preferences. You must make
   * sure to synchronize the prefs at some point to ensure it gets
   * written to disk. The toolbar will also read its config from the
   * prefs if this attribute is set.
   */
  kHIToolbarAutoSavesConfig     = (1 << 0),

  /*
   * This attribute indicates that the toolbar is configurable, i.e.
   * the user can drag items around and bring up the configuration
   * palette, etc.
   */
  kHIToolbarIsConfigurable      = (1 << 1),
  kHIToolbarValidAttrs          = kHIToolbarAutoSavesConfig | kHIToolbarIsConfigurable
};



/*
 *  Summary:
 *    Toolbar Commands
 */
enum {

  /*
   * Sending this to a window with a toolbar will cause the
   * configuration sheet to appear. You can set a menu item's command
   * to this command ID and it will be handled and updated
   * automatically for you.
   */
  kHICommandCustomizeToolbar    = FOUR_CHAR_CODE('tcfg'),

  /*
   * This command causes a window's toolbar to be shown. You can set a
   * menu item's command to this ID and it will be handled and updated
   * automatically for you.
   */
  kHICommandShowToolbar         = FOUR_CHAR_CODE('tbsh'),

  /*
   * This command causes a window's toolbar to be hidden. You can set a
   * menu item's command to this ID and it will be handled and updated
   * automatically for you.
   */
  kHICommandHideToolbar         = FOUR_CHAR_CODE('tbhd')
};


/*
    Parameter Information:
    
    kEventToolbarGetDefaultIdentifiers
        -->     kEventParamToolbar                  typeHIToolbarRef
        -->     kEventParamMutableArray             typeCFMutableArrayRef

    kEventToolbarGetAllowedIdentifiers
        -->     kEventParamToolbar                  typeHIToolbarRef
        -->     kEventParamMutableArray             typeCFMutableArrayRef

    kEventToolbarCreateItemWithIdentifier
        -->     kEventParamToolbar                  typeHIToolbarRef
        -->     kEventParamToolbarItemIdentifier    typeCFStringRef
        -->     kEventParamToolbarItemConfigData    typeCFTypeRef (optional)
        <--     kEventParamToolbarItem              typeHIToolbarItemRef

    kEventToolbarCreateItemFromDrag
        -->     kEventParamDragRef                  typeDragRef
        <--     kEventParamToolbarItem              typeHIToolbarItemRef
*/

/*
 *  Summary:
 *    Toolbar Events
 */
enum {

  /*
   * This event is sent to the delegate to get a list of all of the
   * default item identifiers that should be created for a toolbar. You
   * are passed a mutable array to fill in with the identifiers.
   */
  kEventToolbarGetDefaultIdentifiers = 1,

  /*
   * This event is sent to the delegate to get a list of all the items
   * which could possibly be added to the toolbar. This is sent out
   * when the configuration sheet is about to be displayed.You are
   * passed a mutable array to fill in with the identifiers.
   */
  kEventToolbarGetAllowedIdentifiers = 2,

  /*
   * This event is sent to the delegate to when we need to create an
   * item from an identifier.
   */
  kEventToolbarCreateItemWithIdentifier = 3,

  /*
   * This event is sent to the delegate to when we need to create an
   * item from a drag. This allows you to be able to drag items into a
   * toolbar that aren't normal toolbar items. You might use this to
   * allow your toolbar to accept file system items, for example.
   */
  kEventToolbarCreateItemFromDrag = 4
};


/*
 *  Summary:
 *    Toolbar Item Model Events
 */
enum {

  /*
   * This event is sent to the item (itself) when the image changes.
   * Any interested parties can install handlers on the toolbar item to
   * receive notifications.
   */
  kEventToolbarItemImageChanged = 1,

  /*
   * This event is sent to the item (itself) when the label changes.
   * Any interested parties can install handlers on the toolbar item to
   * receive notifications.
   */
  kEventToolbarItemLabelChanged = 2,

  /*
   * This event is sent to the item (itself) when the help text
   * changes. Any interested parties can install handlers on the
   * toolbar item to receive notifications.
   */
  kEventToolbarItemHelpTextChanged = 3,

  /*
   * This event is sent to the item (itself) when the command ID
   * changes. Any interested parties can install handlers on the
   * toolbar item to receive notifications.
   */
  kEventToolbarItemCommandIDChanged = 4,

  /*
   * This event is sent to the item (itself) when the toolbar is going
   * to write out the configuration information for the item. Any
   * custom items can listen for this event and add any extra
   * information to what is written out into the config so that it can
   * be reanimated later on from the same config data. Typically, you'd
   * not need to handle this event.
   */
  kEventToolbarItemGetPersistentData = 5,

  /*
   * This event is sent to the toolbar item when it is time to create a
   * view for it to display its contents. Implementors of custom
   * toolbar items can install a handler for this event to create their
   * own custom views for their items.
   */
  kEventToolbarItemCreateCustomView = 6,

  /*
   * This event is sent to the item (itself) when the enabled state
   * changes. Any interested parties can install handlers on the
   * toolbar item to receive notifications.
   */
  kEventToolbarItemEnabledStateChanged = 7,

  /*
   * This event is sent when a toolbar item is clicked. Subclasses of
   * toolbar items can choose to do special actions by overriding this
   * event. If this event is unhandled, the default action of sending a
   * command event will occur.
   */
  kEventToolbarItemPerformAction = 8
};


/*
 *  Summary:
 *    Toolbar Item Attributes
 */
enum {
  kHIToolbarItemNoAttributes    = 0,
  kHIToolbarItemAllowDuplicates = (1 << 0),
  kHIToolbarItemCantBeRemoved   = (1 << 1),

  /*
   * This item cannot be moved at all by the user. It is anchored to
   * the left of the toolbar. It is important that there not be any
   * unanchored items to the left of this item, else dragging items
   * around will be a bit wacky. It is up you you, the home viewer, to
   * make sure that anchored items are all on the left. This allows you
   * to do toolbars like the the one in the System Preferences app,
   * where the first couple of items are stuck in place.
   */
  kHIToolbarItemAnchoredLeft    = (1 << 2),
  kHIToolbarItemIsSeparator     = (1 << 3),

  /*
   * If this attribute bit is set, the command that gets sent out will
   * be directed at the user focus instead of at the window the toolbar
   * is attached to.
   */
  kHIToolbarItemSendCmdToUserFocus = (1 << 4),
  kHIToolbarItemValidAttrs      = kHIToolbarItemAllowDuplicates | kHIToolbarItemIsSeparator | kHIToolbarItemCantBeRemoved | kHIToolbarItemAnchoredLeft | kHIToolbarItemSendCmdToUserFocus,
  kHIToolbarItemMutableAttrs    = kHIToolbarItemCantBeRemoved | kHIToolbarItemAnchoredLeft
};

/*======================================================================================*/
/* FUNCTIONS                                                                            */
/*======================================================================================*/
#define _HIToolbarCreate HIToolbarCreate
/*
 *  HIToolbarCreate()
 *  
 *  Discussion:
 *    Creates a toolbar.
 *  
 *  Parameters:
 *    
 *    inIdentifier:
 *      The identifier of the toolbar. If you specify that the toolbar
 *      auto-saves its configuration, this identifier is used to mark
 *      the config info in your application's preferences. You must
 *      specify an identifier.
 *    
 *    inAttributes:
 *      Any attributes you wish to specify.
 *    
 *    outToolbar:
 *      The toolbar reference to your shiny new toolbar.
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
HIToolbarCreate(
  CFStringRef     inIdentifier,
  OptionBits      inAttributes,
  HIToolbarRef *  outToolbar);


#define _HIToolbarGetAttributes HIToolbarGetAttributes
/*
 *  HIToolbarGetAttributes()
 *  
 *  Discussion:
 *    Returns the attributes for the given toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose attributes you desire.
 *    
 *    outAttributes:
 *      The attributes.
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
HIToolbarGetAttributes(
  HIToolbarRef   inToolbar,
  OptionBits *   outAttributes);


#define _HIToolbarChangeAttributes HIToolbarChangeAttributes
/*
 *  HIToolbarChangeAttributes()
 *  
 *  Discussion:
 *    Changes the attributes of a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose attributes you want to change.
 *    
 *    inAttrsToSet:
 *      The attributes you wish to set/enable.
 *    
 *    inAttrsToClear:
 *      The attributes you wish to clear/disable.
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
HIToolbarChangeAttributes(
  HIToolbarRef   inToolbar,
  OptionBits     inAttrsToSet,
  OptionBits     inAttrsToClear);


#define _HIToolbarGetDisplayMode HIToolbarGetDisplayMode
/*
 *  HIToolbarGetDisplayMode()
 *  
 *  Discussion:
 *    Returns the current display mode of a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose display mode you wish to receive.
 *    
 *    outDisplayMode:
 *      The display mode.
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
HIToolbarGetDisplayMode(
  HIToolbarRef            inToolbar,
  HIToolbarDisplayMode *  outDisplayMode);


#define _HIToolbarSetDisplayMode HIToolbarSetDisplayMode
/*
 *  HIToolbarSetDisplayMode()
 *  
 *  Discussion:
 *    Sets the current display mode of a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose display mode you wish to set.
 *    
 *    inDisplayMode:
 *      The display mode. If the toolbar is visible, it will be redrawn.
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
HIToolbarSetDisplayMode(
  HIToolbarRef           inToolbar,
  HIToolbarDisplayMode   inDisplayMode);


#define _HIToolbarGetDisplaySize HIToolbarGetDisplaySize
/*
 *  HIToolbarGetDisplaySize()
 *  
 *  Discussion:
 *    Gets the current display size of a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose display size you wish to get.
 *    
 *    outSize:
 *      The display size.
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
HIToolbarGetDisplaySize(
  HIToolbarRef            inToolbar,
  HIToolbarDisplaySize *  outSize);


#define _HIToolbarSetDisplaySize HIToolbarSetDisplaySize
/*
 *  HIToolbarSetDisplaySize()
 *  
 *  Discussion:
 *    Sets the current display size of a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose display size you wish to set.
 *    
 *    inSize:
 *      The display size. If the toolbar is visible, it will be redrawn.
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
HIToolbarSetDisplaySize(
  HIToolbarRef           inToolbar,
  HIToolbarDisplaySize   inSize);


#define _HIToolbarCopyIdentifier HIToolbarCopyIdentifier
/*
 *  HIToolbarCopyIdentifier()
 *  
 *  Discussion:
 *    Returns the identifier for a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose identifier you wish to get.
 *    
 *    outIdentifier:
 *      The identifier. You must release it when you are finished with
 *      it.
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
HIToolbarCopyIdentifier(
  HIToolbarRef   inToolbar,
  CFStringRef *  outIdentifier);


#define _HIToolbarCopyItems HIToolbarCopyItems
/*
 *  HIToolbarCopyItems()
 *  
 *  Discussion:
 *    Returns the array of toolbar items for a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar whose items you wish to receive.
 *    
 *    outItems:
 *      The array of toolbar items owned by the toolbar. You must
 *      release the array when you are finished with it.
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
HIToolbarCopyItems(
  HIToolbarRef   inToolbar,
  CFArrayRef *   outItems);


/*
 *  HIToolbarCreateItemWithIdentifier()
 *  
 *  Discussion:
 *    Creates an item specified by a particular identifier. Using this
 *    function allows you to create any item a delegate supports by
 *    naming its identifier. It also allows you to create standard
 *    items supplied by the Toolbox, such as the separator item.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar you are adding to.
 *    
 *    inIdentifier:
 *      The identifier of the item you wish to add.
 *    
 *    inConfigData:
 *      Any config data required by the item to safely construct.
 *      Standard items do not require any extra data, so NULL can be
 *      passed.
 *    
 *    outItem:
 *      The newly created toolbar item.
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
HIToolbarCreateItemWithIdentifier(
  HIToolbarRef        inToolbar,
  CFStringRef         inIdentifier,
  CFTypeRef           inConfigData,       /* can be NULL */
  HIToolbarItemRef *  outItem);



#define _HIToolbarInsertItemAtIndex HIToolbarInsertItemAtIndex
/*
 *  HIToolbarInsertItemAtIndex()
 *  
 *  Discussion:
 *    Inserts a toolbar item at a given index into a toolbar.
 *    Generally, you should always add items via identifier, and not
 *    with this routine.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar to receive the new item.
 *    
 *    inItem:
 *      The item reference of the toolbar item you are adding.
 *    
 *    inIndex:
 *      The index you wish to add the item at. This index is zero-based.
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
HIToolbarInsertItemAtIndex(
  HIToolbarRef       inToolbar,
  HIToolbarItemRef   inItem,
  CFIndex            inIndex);



#define _HIToolbarAppendItem HIToolbarAppendItem
/*
 *  HIToolbarAppendItem()
 *  
 *  Discussion:
 *    Appends an item to the end of a toolbar. Generally, you should
 *    always add items via identifier, and not with this routine.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar to receive the new item.
 *    
 *    inItem:
 *      The item reference of the toolbar item you are adding.
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
HIToolbarAppendItem(
  HIToolbarRef       inToolbar,
  HIToolbarItemRef   inItem);


#define _HIToolbarRemoveItemAtIndex HIToolbarRemoveItemAtIndex
/*
 *  HIToolbarRemoveItemAtIndex()
 *  
 *  Discussion:
 *    Removes an item at a given index from a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar you are removing the item from.
 *    
 *    inIndex:
 *      The index of the item to remove. This index is zero-based.
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
HIToolbarRemoveItemAtIndex(
  HIToolbarRef   inToolbar,
  CFIndex        inIndex);


#define _HIToolbarSetDelegate HIToolbarSetDelegate
/*
 *  HIToolbarSetDelegate()
 *  
 *  Discussion:
 *    Sets the delegate event target for a toolbar. This is necessary
 *    for a toolbar to work properly if it wants items of its own. The
 *    delegate is who is asked to create toolbar items. If the delegate
 *    does not respond, the toolbar will attempt to create a toolbar
 *    item, but it can only create the standard items defined at the
 *    top of this header.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar you are removing the item from.
 *    
 *    inDelegate:
 *      The HIObjectRef to act as the delegate.
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
HIToolbarSetDelegate(
  HIToolbarRef   inToolbar,
  HIObjectRef    inDelegate);


#define _HIToolbarGetDelegate HIToolbarGetDelegate
/*
 *  HIToolbarGetDelegate()
 *  
 *  Discussion:
 *    Returns the current delegate in use by a toolbar.
 *  
 *  Parameters:
 *    
 *    inToolbar:
 *      The toolbar you want the delegate from.
 *  
 *  Result:
 *    An HIObjectRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIObjectRef )
HIToolbarGetDelegate(HIToolbarRef inToolbar);


/*==========================================================================*/
/* HITOOLBARITEM                                                            */
/*==========================================================================*/
/* The HIObject class ID for the ToolbarItem class. */

#define kHIToolbarItemClassID           CFSTR("com.apple.hitoolbaritem")


/* Required construction parameters                                         */
/*                                                                          */
/* You must pass these parameters in the initialization event if you are    */
/* subclassing the toolbar item                                             */
/*                                                                          */
/*      kEventParamToolbarItemIdentifier            typeCFStringRef         */
/*      kEventParamAttibutes                        typeUInt32              */


#define _HIToolbarItemCreate HIToolbarItemCreate
/*
 *  HIToolbarItemCreate()
 *  
 *  Discussion:
 *    Creates a toolbar item for use in a toolbar. Typically, you would
 *    create toolbar items in your delegate. When a toolbar needs to
 *    create an item with a given identifier, your delegate is asked to
 *    create it.
 *  
 *  Parameters:
 *    
 *    inIdentifier:
 *      The identifier of the item in question.
 *    
 *    inOptions:
 *      Any options for the item.
 *    
 *    outItem:
 *      The item you created.
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
HIToolbarItemCreate(
  CFStringRef         inIdentifier,
  OptionBits          inOptions,
  HIToolbarItemRef *  outItem);


#define _HIToolbarItemCopyIdentifier HIToolbarItemCopyIdentifier
/*
 *  HIToolbarItemCopyIdentifier()
 *  
 *  Discussion:
 *    Returns the identifier of the given item. The toolbar uses this
 *    identifier when writing the config information to the preferences
 *    (if set up for auto-config).
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    outIdentifier:
 *      The identifier of the item. You should release this string when
 *      you are finished with it.
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
HIToolbarItemCopyIdentifier(
  HIToolbarItemRef   inItem,
  CFStringRef *      outIdentifier);


#define _HIToolbarItemGetAttributes HIToolbarItemGetAttributes
/*
 *  HIToolbarItemGetAttributes()
 *  
 *  Discussion:
 *    Returns the attributes of the given item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    outAttributes:
 *      The attributes of the item.
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
HIToolbarItemGetAttributes(
  HIToolbarItemRef   inItem,
  OptionBits *       outAttributes);


#define _HIToolbarItemChangeAttributes HIToolbarItemChangeAttributes
/*
 *  HIToolbarItemChangeAttributes()
 *  
 *  Discussion:
 *    Changes the attributes of a toolbar item. Only those attributes
 *    defined by the kHIToolbarItemChangeableAttrs can be passed into
 *    this API. All other options can only be specified at creation.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inAttrsToSet:
 *      The attributes to set on the item. This value can be
 *      kHIToolbarItemNoAttributes if you are only clearing attributes.
 *    
 *    inAttrsToClear:
 *      The attributes to clear on the item. This value can be
 *      kHIToolbarItemNoAttributes if you are only setting attributes.
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
HIToolbarItemChangeAttributes(
  HIToolbarItemRef   inItem,
  OptionBits         inAttrsToSet,
  OptionBits         inAttrsToClear);


#define _HIToolbarItemSetLabel HIToolbarItemSetLabel
/*
 *  HIToolbarItemSetLabel()
 *  
 *  Discussion:
 *    Sets the label of a toolbar item. This is what the toolbar view
 *    will display underneath the image. It is also used in the
 *    configuration palette for configurable toolbars.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inLabel:
 *      The label. The toolbox will copy the string passed in.
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
HIToolbarItemSetLabel(
  HIToolbarItemRef   inItem,
  CFStringRef        inLabel);


#define _HIToolbarItemCopyLabel HIToolbarItemCopyLabel
/*
 *  HIToolbarItemCopyLabel()
 *  
 *  Discussion:
 *    Returns the label of a toolbar item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    outLabel:
 *      The label of the item. You should release this when you are
 *      finished with it.
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
HIToolbarItemCopyLabel(
  HIToolbarItemRef   inItem,
  CFStringRef *      outLabel);


#define _HIToolbarItemSetHelpText HIToolbarItemSetHelpText
/*
 *  HIToolbarItemSetHelpText()
 *  
 *  Discussion:
 *    Sets the text used for help tags for a toolbar item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inShortText:
 *      The short help text. This is what is displayed normally by the
 *      help tag system when the user hovers over the toolbar item with
 *      the mouse.
 *    
 *    inLongText:
 *      The long help text. This is what is displayed by the help tag
 *      system when the user hovers over the toolbar item with the
 *      mouse and holds the command key down. This parameter is
 *      optional, you may pass NULL.
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
HIToolbarItemSetHelpText(
  HIToolbarItemRef   inItem,
  CFStringRef        inShortText,
  CFStringRef        inLongText);       /* can be NULL */


#define _HIToolbarItemCopyHelpText HIToolbarItemCopyHelpText
/*
 *  HIToolbarItemCopyHelpText()
 *  
 *  Discussion:
 *    Returns the text used for help tags for a toolbar item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    outShortText:
 *      The short help text. This is what is displayed normally by the
 *      help tag system when the user hovers over the toolbar item with
 *      the mouse. You should release this string when you are finished
 *      with it. If you do not wish to receive the short help text,
 *      pass NULL for this parameter.
 *    
 *    outLongText:
 *      The long help text. This is what is displayed by the help tag
 *      system when the user hovers over the toolbar item with the
 *      mouse and holds the command key down. You should release this
 *      string when you are finished with it. If you do not wish to
 *      receive the long help text, pass NULL for this parameter.
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
HIToolbarItemCopyHelpText(
  HIToolbarItemRef   inItem,
  CFStringRef *      outShortText,       /* can be NULL */
  CFStringRef *      outLongText);       /* can be NULL */


#define _HIToolbarItemSetCommandID HIToolbarItemSetCommandID
/*
 *  HIToolbarItemSetCommandID()
 *  
 *  Discussion:
 *    Sets the command ID of a toolbar item. When an toolbar item is
 *    clicked, the default behavior is to send out the command assigned
 *    to the item. This API lets you set that command ID. The command
 *    is sent out via the ProcessHICommand API, so it uses Carbon
 *    Events.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inCommandID:
 *      The command ID.
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
HIToolbarItemSetCommandID(
  HIToolbarItemRef   inItem,
  MenuCommand        inCommandID);


#define _HIToolbarItemGetCommandID HIToolbarItemGetCommandID
/*
 *  HIToolbarItemGetCommandID()
 *  
 *  Discussion:
 *    Gets the command ID of a toolbar item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    outCommandID:
 *      The command ID.
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
HIToolbarItemGetCommandID(
  HIToolbarItemRef   inItem,
  MenuCommand *      outCommandID);


#define _HIToolbarItemSetIconRef HIToolbarItemSetIconRef
/*
 *  HIToolbarItemSetIconRef()
 *  
 *  Discussion:
 *    Sets the icon for a toolbar item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inIcon:
 *      The icon ref. The toolbar will create an appropriate CGImageRef
 *      for the icon passed in. The icon can be released after this API
 *      is called. ooo NOTE: This API may change or disappear!
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
HIToolbarItemSetIconRef(
  HIToolbarItemRef   inItem,
  IconRef            inIcon);


#define _HIToolbarItemSetImage HIToolbarItemSetImage
/*
 *  HIToolbarItemSetImage()
 *  
 *  Discussion:
 *    Sets the image for a toolbar item. Currently, the image should be
 *    no higher than 32 pixels. This image is used both in the toolbar
 *    as well as the configuration sheet, if the toolbar is
 *    configurable.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inImage:
 *      The image. This image is retained by the toolbar item. You may
 *      release it after calling this API.
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
HIToolbarItemSetImage(
  HIToolbarItemRef   inItem,
  CGImageRef         inImage);


#define _HIToolbarItemCopyImage HIToolbarItemCopyImage
/*
 *  HIToolbarItemCopyImage()
 *  
 *  Discussion:
 *    Returns the image for a toolbar item. This image is already
 *    retained by the time you receive it, so you can release it when
 *    you are done with it.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    outImage:
 *      The retained image. You should release it when finished with it.
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
HIToolbarItemCopyImage(
  HIToolbarItemRef   inItem,
  CGImageRef *       outImage);


#define _HIToolbarItemSetMenu HIToolbarItemSetMenu
/*
 *  HIToolbarItemSetMenu()
 *  
 *  Discussion:
 *    Sets the submenu for a toolbar item. Normally, items do not have
 *    a submenu. You can attach one with this API. The submenu will, by
 *    default, show up both in the 'more items' indicator popup
 *    attached to the item name. It will also appear if the toolbar is
 *    in text only mode and the label is clicked. You should attach a
 *    Carbon Event handler to the menu to handle updating the menu
 *    items as appropriate before the menu is displayed.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inMenu:
 *      The menu. It is retained by the toolbar item, so you can safely
 *      release it after calling this API.
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
HIToolbarItemSetMenu(
  HIToolbarItemRef   inItem,
  MenuRef            inMenu);


#define _HIToolbarItemCopyMenu HIToolbarItemCopyMenu
/*
 *  HIToolbarItemCopyMenu()
 *  
 *  Discussion:
 *    Gets the submenu for a toolbar item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    outMenu:
 *      The retained menu. You should release it when you are finished
 *      with it.
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
HIToolbarItemCopyMenu(
  HIToolbarItemRef   inItem,
  MenuRef *          outMenu);


#define _HIToolbarItemGetToolbar HIToolbarItemGetToolbar
/*
 *  HIToolbarItemGetToolbar()
 *  
 *  Discussion:
 *    Gets the toolbar a toolbar item is attached to.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *  
 *  Result:
 *    The toolbar item reference of the toolbar this item is bound to,
 *    or NULL if this toolbar item is not attached to any toolbar.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( HIToolbarRef )
HIToolbarItemGetToolbar(HIToolbarItemRef inItem);


/*
 *  HIToolbarItemIsEnabled()
 *  
 *  Discussion:
 *    Used to determine if a toolbar item is enabled.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *  
 *  Result:
 *    A boolean result.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available in CarbonLib 1.x, is available on Mac OS X version 10.2 and later
 *    Mac OS X:         in version 10.2 and later
 */
EXTERN_API_C( Boolean )
HIToolbarItemIsEnabled(HIToolbarItemRef inItem);


/*
 *  HIToolbarItemSetEnabled()
 *  
 *  Discussion:
 *    Enables or disables a toolbar item.
 *  
 *  Parameters:
 *    
 *    inItem:
 *      The item in question.
 *    
 *    inEnabled:
 *      The new enabled state.
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
HIToolbarItemSetEnabled(
  HIToolbarItemRef   inItem,
  Boolean            inEnabled);



#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HITOOLBAR__ */

