/*
     File:       Menus.h
 
     Contains:   Menu Manager Interfaces.
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1985-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __MENUS__
#define __MENUS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __MACERRORS__
#include <MacErrors.h>
#endif

#ifndef __TEXTCOMMON__
#include <TextCommon.h>
#endif

#ifndef __COLLECTIONS__
#include <Collections.h>
#endif

#ifndef __CFSTRING__
#include <CFString.h>
#endif

#ifndef __CFUUID__
#include <CFUUID.h>
#endif

#ifndef __FONTS__
#include <Fonts.h>
#endif

#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

#ifndef __APPLEEVENTS__
#include <AppleEvents.h>
#endif

#ifndef __PROCESSES__
#include <Processes.h>
#endif

#ifndef __CARBONEVENTSCORE__
#include <CarbonEventsCore.h>
#endif

#ifndef __EVENTS__
#include <Events.h>
#endif

#ifndef __HIOBJECT__
#include <HIObject.h>
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


/*
 *  Menu Manager
 */
/*--------------------------------------------------------------------------------------*/
/*  o Menu Constants                                                                    */
/*--------------------------------------------------------------------------------------*/
enum {
  noMark                        = 0     /* mark symbol for SetItemMark; other mark symbols are defined in Fonts.h */
};

/*
    A Short Course on Menu Definition Functions
    
    A menu definition function is used to implement a custom visual appearance for a menu.
    Menu definition functions are still supported in Carbon, but the messages sent to a
    menu definition function in Carbon are different than for a non-Carbon application.
    
    In general, Apple recommends using the system menu definition whenever possible.
    Menu definition functions will continue to be supported, but it is not easy to write
    a correct menu definition, especially one that attempts to imitate the standard system
    menu appearance. If you require special features in your menu that the system menu
    definition does not support, please mail <toolbox@apple.com> and describe your requirements;
    we would much rather enhance the system menu definition than have you write a custom one.
    
    Menu definition functions before Carbon used the following messages:
    
        kMenuDrawMsg
        kMenuChooseMsg
        kMenuSizeMsg
        kMenuPopUpMsg
        kMenuDrawItemMsg
        kMenuCalcItemMsg
        kMenuThemeSavvyMsg
        
    kMenuChooseMsg and kMenuDrawItemMsg are not supported in Carbon and are not sent to
    Carbon menu definitions. In Carbon, kMenuChooseMsg is replaced by kMenuFindItemMsg and
    kMenuHiliteItemMsg. Menu definition functions in Carbon use the following messages:
    
        kMenuInitMsg
        kMenuDisposeMsg
        kMenuFindItemMsg
        kMenuHiliteItemMsg
        kMenuDrawItemsMsg
        kMenuDrawMsg
        kMenuSizeMsg
        kMenuPopUpMsg
        kMenuCalcItemMsg
        kMenuThemeSavvyMsg
        
    The rest of this documentation will focus on Carbon menu definitions only.
    
    Menu Definition Messages
    
        Carbon menu definition functions should support the following messages:
        
        kMenuInitMsg
            
            menuRect        unused
            hitPt           unused
            whichItem       OSErr*
        
            Sent when a menu is created. This message gives the menu definition an opportunity
            to initialize its own state. If the menu definition encounters an error while
            initializing, it should set *whichItem to a non-zero error code; this will cause the
            Menu Manager to destroy the menu and return an error back from the API that was used
            to create the menu.
        
        kMenuDisposeMsg
            
            menuRect        unused
            hitPt           unused
            whichItem       unused
            
            Sent when a menu is destroyed. This message gives the menu definition an opportunity
            to destroy its own data.
            
        kMenuFindItemMsg
            
            menuRect        menu bounds
            hitPt           point to hit-test
            whichItem       MDEFFindItemData*
            
            Sent when the Menu Manager is displaying a menu and needs to know what item is under
            the mouse. The whichItem parameter is actually a pointer to a MenuTrackingData structure.
            On entry, the menu, virtualMenuTop, and virtualMenuBottom fields of this structure are
            valid. The menu definition should determine which item containst the given point, if any,
            and fill in the itemUnderMouse, itemSelected, and itemRect fields. If an item is found,
            the menu definition should always fill in the itemUnderMouse and itemRect fields. The
            menu definition should only fill in the itemSelected field if the item is available for
            selection; if it is unavailable (because it is disabled, or for some other reason), the
            menu definition should set the itemSelected field to zero.
            
            The values placed in the itemUnderMouse and itemSelected fields should be less than or
            equal to the number of items returned by CountMenuItems on this menu.
            
            The menu definition should not hilite the found item during this message. The Menu 
            Manager will send a separate kMenuHiliteItemMsg to request hiliting of the item.
            
            If the menu definition supports scrolling, it should scroll the menu during this message,
            and update the virtualMenuTop and virtualMenuBottom fields of the MenuTrackingData to
            indicate the menu's new scrolled position.
            
            If the menu definition uses QuickDraw to draw while scrolling, it should draw into the
            current port.
    
            If the menu definition uses CoreGraphics to draw while scrolling, it should use the
            CGContextRef passed in the context field of the MDEFHiliteItemData structure.
            
            Menu definitions must use the ScrollMenuImage API, if available, to scroll the menu contents.
            This API is available in CarbonLib 1.5 and later, and in Mac OS X 10.1 and later. ScrollMenuImage
            properly supports scrolling the alpha channel in the menu's image data. Use of QuickDraw's
            ScrollRect API to scroll the menu contents will result in the alpha channel being set to 0xFF
            (opaque) and the menu will no longer be transparent.
            
            The menu definition should not modify the menu field of the MenuTrackingData.
            
        kMenuHiliteItemMsg
        
            menuRect        menu bounds
            hitPt           unused
            whichItem       MDEFHiliteItemData*
            
            Sent when the Menu Manager is displaying a menu and needs to hilite a newly selected
            item. The whichItem parameter is actually a pointer to a MDEFHiliteItemData structure.
            The menu definition should unhilite the item in the previousItem field, if non-zero,
            and hilite the item in the newItem field.
            
            Menu definitions should use the EraseMenuBackground API to erase the old menu contents
            before unhiliting a menu item, if the menu definition is using the Appearance Manager's
            menu drawing APIs. This is necessary because the background of a menu is transparent on
            Aqua, and if the old hilite is not erased first, it will show through the new unhilited
            menu background.
            
            If the menu definition uses QuickDraw to draw, it should draw into the current port.
    
            If the menu definition uses CoreGraphics to draw, it should use the CGContextRef passed
            in the context field of the MDEFHiliteItemData structure.
            
        kMenuDrawItemsMsg
        
            menuRect        menu bounds
            hitPt           unused
            whichItem       MDEFDrawItemsData*
            
            Sent when the Menu Manager is displaying a menu and needs to redraw a portion of the
            menu. This message is used by the dynamic menu item support code in the Menu Manager;
            for example, if items five and six in a menu are a dynamic group, the Menu Manager will
            send a DrawItems message when the group's modifier key is pressed or released to redraw
            the appropriate item, but no other items in the menu.
            
            The whichItem parameter for this message is actually a pointer to a MDEFDrawItemsData
            structure. The menu definition should redraw the items starting with firstItem and
            ending with lastItem, inclusive.
            
            If the menu definition uses QuickDraw to draw, it should draw into the current port.
    
            If the menu definition uses CoreGraphics to draw, it should use the CGContextRef passed
            in the context field of the MDEFDrawItemsData structure.
            
        kMenuDrawMsg
            
            menuRect        menu bounds
            hitPt           unused
            whichItem       MDEFDrawData*
            
            Sent when the Menu Manager is displaying a menu and needs to redraw the entire menu.
            The whichItem parameter is actually a pointer to a MenuTrackingData structure. On entry,
            the menu field of this structure is valid. The menu definition should draw the menu and,
            if it supports scrolling, should also fill in the virtualMenuTop and virtualMenuBottom
            fields of the structure to indicate the menu's initial unscrolled position; typically, 
            virtualMenuTop would be set to the same value as the top coordinate of the menu rect,
            and virtualMenuBottom would be set to virtualMenuTop plus the virtual height of the menu.
            
            If the menu definition uses QuickDraw to draw, it should draw into the current port.
    
            If the menu definition uses CoreGraphics to draw, it should use the CGContextRef passed
            in the context field of the MDEFDrawData structure.
            
        kMenuSizeMsg
        
            menuRect        unused
            hitPt           maximum width and height of the menu
            whichItem       unused
            
            Sent when the Menu Manager needs to determine the size of a menu. The menu definition
            should calculate the width and height of the menu and store the sizes into the menu with
            SetMenuWidth and SetMenuHeight.
            
            If the gestaltMenuMgrSendsMenuBoundsToDefProc bit is set in the Menu Manager's Gestalt
            value, then the hitPt parameter to this message is the maximum width (hitPt.h) and height
            (hitPt.v) of the menu. The menu definition should ensure that the width and height that it
            places in the menu do not exceed these values. If the gestalt bit is not set, the menu
            definition should just use the main GDevice's width and height as constraints on the menu's
            width and height.
            
        kMenuPopUpMsg
        
            menuRect        on entry, constraints on the menu's position; on exit, menu bounds
            hitPt           requested menu location, with swapped coordinates
            whichItem       on entry, requested initial selection; on exit, virtual menu top
            
            Sent when the Menu Manager is about to display a popup menu. The menu definition should
            calculate the appropriate menu bounds to contain the menu based on the requested menu
            location and selected item. It should write the menuBounds into the rect given by the
            menuRect parameter.
            
            If the gestaltMenuMgrSendsMenuBoundsToDefProc bit is set in the Menu Manager's Gestalt
            value, then the menuRect parameter on entry to this message contains a constraint rect,
            in global coordinates, outside of which the popup menu should not be positioned. The menu
            definition should take this constraint rect into account as it calculates the menu bounds.
            If the gestalt bit is not set, the menu definition should use the bounds of the GDevice
            containing the menu's top left corner as a constraint on the menu's position.
            
            The hitPt parameter is a requested location for the top left corner of the menu. The
            coordinates of this parameter are swapped from their normal order; the h field of the
            hitPt parameter contains the vertical coordinate, and the v field of hitPt contains
            the horizontal coordinate.
            
            On entry, the whichItem parameter points at a menu item index which is requested to be
            the initial selection when the menu is displayed. After calculating the menu's bounds,
            the menu definition should write the menu's virtual top coordinate into the location
            pointed at by the whichItem parameter. If displaying the menu at the requested location
            does not require scrolling, the virtual top will be the same as the menu bounds top;
            if the menu must scroll to fit in the requested location, the virtual top may be different.
            
        kMenuCalcItemMsg
        
            menuRect        on exit, item bounds
            hitPt           unused
            whichItem       the item whose rect to calculate
            
            Sent when the Menu Manager needs to know the bounds of a menu item. The menu definition
            should calculate the size of the item specified by the whichItem parameter, and store 
            the bounds in the rect specified by the menuRect parameter.
            
            Some sample menu definition code provided by Apple has previously shown an implementation
            of this message that always sets the top left corner of the item bounds to (0,0), regardless
            of the item's actual position in the menu. For best future compatibility, menu definitions
            should begin storing an item bounds that gives the item's actual position in the menu based
            on the menu's current virtual top. For example, if the virtual menu top starts at 20, then
            the menu definition would calculate an item bounds for the first item that starts at (0,20),
            an item bounds for the second item that starts at (0,40), and so on. The menu definition
            should call GetMenuTrackingData to get the menu's current virtual position, and use zero
            for the menu top if GetMenuTrackingData returns an error.
            
        kMenuThemeSavvyMsg
        
            menuRect        unused
            hitPt           unused
            whichItem       on exit, indicates theme-savvyness of the menu definition
            
            Sent by the Menu Manager to determine whether the MDEF uses the Appearance Manager 
            menu-drawing functions to draw its menu. If it does, the menu definition should return
            kThemeSavvyMenuResponse in the location pointed to by whichItem. If the menu definition
            draws its own custom content without using the Appearance Manager menu-drawing functions,
            it should ignore this message.
    
    Low-memory Global Replacements
    
        Pre-Carbon menu definitions needed to use several low-memory globals to communicate with the
        Menu Manager. These globals have all been replaced or made obsolete in Carbon, as follows:
        
        MenuDisable
            
            MenuDisable is now set automatically by the Menu Manager using the value returned in the
            itemUnderMouse field of the MenuTrackingData structure passed to kMenuFindItemMsg.
            
        TopMenuItem
        AtMenuBottom
        
            TopMenuItem and AtMenuBottom are now set automatically by the Menu Manager using the
            values returned in the virtualMenuTop and virtualMenuBottom fields of the MenuTrackingData
            structure passed to kMenuDrawMsg and kMenuFindItemMsg.
            
        mbSaveLoc
    
            This undocumented low-memory global was used by pre-Carbon menu definitions to store
            the bounding rect of the currently selected item and to avoid drawing glitches while
            the menu definition was scrolling the contents of a menu that had submenus. The Menu
            Manager now automatically sets the selected item bounds using the value returned in
            the itemRect field of the MenuTrackingData structure passed to kMenuFindItemMsg. In
            order to correctly support scrolling of menus with submenus, a menu definition should
            verify, before scrolling the menu contents, that no submenus of the scrolling menu are
            currently visible. A menu definition can use GetMenuTrackingData to verify this condition,
            as follows:
            
                Boolean SafeToScroll( MenuRef menuBeingScrolled )
                {
                    MenuTrackingData lastMenuData;
                    return GetMenuTrackingData( NULL, &lastMenuData ) == noErr
                           && lastMenuData.menu == menuBeingScrolled;
                }
            
            If SafeToScroll returns false, the menu definition should not scroll the menu.
*/
enum {
                                        /* menu defProc messages */
  kMenuDrawMsg                  = 0,
  kMenuSizeMsg                  = 2,
  kMenuPopUpMsg                 = 3,
  kMenuCalcItemMsg              = 5,
  kMenuThemeSavvyMsg            = 7,    /* is your MDEF theme-savvy?  If so, return kThemeSavvyMenuResponse in the whichItem parameter*/
  mDrawMsg                      = 0,
  mSizeMsg                      = 2,
  mPopUpMsg                     = 3,    /* position the popup menu rect appropriately */
  mCalcItemMsg                  = 5
};

#if CALL_NOT_IN_CARBON
/*
   Carbon MDEFs must replace mChooseMsg with the new messages kMenuFindItemMsg and kMenuHiliteItemMsg. 
   mDrawItemMsg was used by the popup menu control before 8.5, but is no longer used. 
*/
enum {
  mChooseMsg                    = 1,
  mDrawItemMsg                  = 4,
  kMenuChooseMsg                = 1,
  kMenuDrawItemMsg              = 4
};

#endif  /* CALL_NOT_IN_CARBON */

enum {
  kThemeSavvyMenuResponse       = 0x7473 /* should be returned in *whichItem when handling kMenuThemeSavvyMsg*/
};

/* these MDEF messages are only supported in Carbon*/
enum {
  kMenuInitMsg                  = 8,
  kMenuDisposeMsg               = 9,
  kMenuFindItemMsg              = 10,
  kMenuHiliteItemMsg            = 11,
  kMenuDrawItemsMsg             = 12
};

enum {
  textMenuProc                  = 0,
  hMenuCmd                      = 27,   /*itemCmd == 0x001B ==> hierarchical menu*/
  hierMenu                      = -1,   /*a hierarchical menu - for InsertMenu call*/
  kInsertHierarchicalMenu       = -1,   /*a better name for hierMenu */
  mctAllItems                   = -98,  /*search for all Items for the given ID*/
  mctLastIDIndic                = -99   /*last color table entry has this in ID field*/
};

/* Constants for use with MacOS 8.0 (Appearance 1.0) and later*/
enum {
  kMenuStdMenuProc              = 63,
  kMenuStdMenuBarProc           = 63
};

/* For use with Get/SetMenuItemModifiers*/
enum {
  kMenuNoModifiers              = 0,    /* Mask for no modifiers*/
  kMenuShiftModifier            = (1 << 0), /* Mask for shift key modifier*/
  kMenuOptionModifier           = (1 << 1), /* Mask for option key modifier*/
  kMenuControlModifier          = (1 << 2), /* Mask for control key modifier*/
  kMenuNoCommandModifier        = (1 << 3) /* Mask for no command key modifier*/
};

/* For use with Get/SetMenuItemIconHandle*/
enum {
  kMenuNoIcon                   = 0,    /* No icon*/
  kMenuIconType                 = 1,    /* Type for ICON*/
  kMenuShrinkIconType           = 2,    /* Type for ICON plotted 16 x 16*/
  kMenuSmallIconType            = 3,    /* Type for SICN*/
  kMenuColorIconType            = 4,    /* Type for cicn*/
  kMenuIconSuiteType            = 5,    /* Type for Icon Suite*/
  kMenuIconRefType              = 6,    /* Type for Icon Ref*/
  kMenuCGImageRefType           = 7,    /* Type for a CGImageRef (Mac OS X only)*/
  kMenuSystemIconSelectorType   = 8,    /* Type for an IconRef registered with Icon Services under kSystemIconsCreator (Mac OS X 10.1 and later only)*/
  kMenuIconResourceType         = 9     /* Type for a CFStringRef naming a resource in the main bundle of the process (Mac OS X 10.1 and later only)*/
};

/* For use with Get/SetMenuItemKeyGlyph*/
enum {
  kMenuNullGlyph                = 0x00, /* Null (always glyph 1)*/
  kMenuTabRightGlyph            = 0x02, /* Tab to the right key (for left-to-right script systems)*/
  kMenuTabLeftGlyph             = 0x03, /* Tab to the left key (for right-to-left script systems)*/
  kMenuEnterGlyph               = 0x04, /* Enter key*/
  kMenuShiftGlyph               = 0x05, /* Shift key*/
  kMenuControlGlyph             = 0x06, /* Control key*/
  kMenuOptionGlyph              = 0x07, /* Option key*/
  kMenuSpaceGlyph               = 0x09, /* Space (always glyph 3) key*/
  kMenuDeleteRightGlyph         = 0x0A, /* Delete to the right key (for right-to-left script systems)*/
  kMenuReturnGlyph              = 0x0B, /* Return key (for left-to-right script systems)*/
  kMenuReturnR2LGlyph           = 0x0C, /* Return key (for right-to-left script systems)*/
  kMenuNonmarkingReturnGlyph    = 0x0D, /* Nonmarking return key*/
  kMenuPencilGlyph              = 0x0F, /* Pencil key*/
  kMenuDownwardArrowDashedGlyph = 0x10, /* Downward dashed arrow key*/
  kMenuCommandGlyph             = 0x11, /* Command key*/
  kMenuCheckmarkGlyph           = 0x12, /* Checkmark key*/
  kMenuDiamondGlyph             = 0x13, /* Diamond key*/
  kMenuAppleLogoFilledGlyph     = 0x14, /* Apple logo key (filled)*/
  kMenuParagraphKoreanGlyph     = 0x15, /* Unassigned (paragraph in Korean)*/
  kMenuDeleteLeftGlyph          = 0x17, /* Delete to the left key (for left-to-right script systems)*/
  kMenuLeftArrowDashedGlyph     = 0x18, /* Leftward dashed arrow key*/
  kMenuUpArrowDashedGlyph       = 0x19, /* Upward dashed arrow key*/
  kMenuRightArrowDashedGlyph    = 0x1A, /* Rightward dashed arrow key*/
  kMenuEscapeGlyph              = 0x1B, /* Escape key*/
  kMenuClearGlyph               = 0x1C, /* Clear key*/
  kMenuLeftDoubleQuotesJapaneseGlyph = 0x1D, /* Unassigned (left double quotes in Japanese)*/
  kMenuRightDoubleQuotesJapaneseGlyph = 0x1E, /* Unassigned (right double quotes in Japanese)*/
  kMenuTrademarkJapaneseGlyph   = 0x1F, /* Unassigned (trademark in Japanese)*/
  kMenuBlankGlyph               = 0x61, /* Blank key*/
  kMenuPageUpGlyph              = 0x62, /* Page up key*/
  kMenuCapsLockGlyph            = 0x63, /* Caps lock key*/
  kMenuLeftArrowGlyph           = 0x64, /* Left arrow key*/
  kMenuRightArrowGlyph          = 0x65, /* Right arrow key*/
  kMenuNorthwestArrowGlyph      = 0x66, /* Northwest arrow key*/
  kMenuHelpGlyph                = 0x67, /* Help key*/
  kMenuUpArrowGlyph             = 0x68, /* Up arrow key*/
  kMenuSoutheastArrowGlyph      = 0x69, /* Southeast arrow key*/
  kMenuDownArrowGlyph           = 0x6A, /* Down arrow key*/
  kMenuPageDownGlyph            = 0x6B, /* Page down key*/
  kMenuAppleLogoOutlineGlyph    = 0x6C, /* Apple logo key (outline)*/
  kMenuContextualMenuGlyph      = 0x6D, /* Contextual menu key*/
  kMenuPowerGlyph               = 0x6E, /* Power key*/
  kMenuF1Glyph                  = 0x6F, /* F1 key*/
  kMenuF2Glyph                  = 0x70, /* F2 key*/
  kMenuF3Glyph                  = 0x71, /* F3 key*/
  kMenuF4Glyph                  = 0x72, /* F4 key*/
  kMenuF5Glyph                  = 0x73, /* F5 key*/
  kMenuF6Glyph                  = 0x74, /* F6 key*/
  kMenuF7Glyph                  = 0x75, /* F7 key*/
  kMenuF8Glyph                  = 0x76, /* F8 key*/
  kMenuF9Glyph                  = 0x77, /* F9 key*/
  kMenuF10Glyph                 = 0x78, /* F10 key*/
  kMenuF11Glyph                 = 0x79, /* F11 key*/
  kMenuF12Glyph                 = 0x7A, /* F12 key*/
  kMenuF13Glyph                 = 0x87, /* F13 key*/
  kMenuF14Glyph                 = 0x88, /* F14 key*/
  kMenuF15Glyph                 = 0x89, /* F15 key*/
  kMenuControlISOGlyph          = 0x8A, /* Control key (ISO standard)*/
  kMenuEjectGlyph               = 0x8C  /* Eject key (available on Jaguar and later)*/
};


/*
 *  MenuAttributes
 *  
 *  Summary:
 *    Menu attributes control behavior of the entire menu. They are
 *    used with the Get/ChangeMenuAttributes APIs.
 */
typedef UInt32 MenuAttributes;
enum {

  /*
   * No column space is allocated for the mark character when this menu
   * is drawn.
   */
  kMenuAttrExcludesMarkColumn   = (1 << 0),

  /*
   * The menu title is automatically disabled when all items are
   * disabled.
   */
  kMenuAttrAutoDisable          = (1 << 2),

  /*
   * The pencil glyph from the Keyboard font (kMenuPencilGlyph) is used
   * to draw the Control modifier key in menu keyboard equivalents.
   * This appearance is typically used only by Japanese input method
   * menus.
   */
  kMenuAttrUsePencilGlyph       = (1 << 3),

  /*
   * The menu title is not drawn in the menubar, even when the menu is
   * inserted in the menubar. Useful for adding command keys that don't
   * correspond to a visible menu item; menu items with the desired
   * command keys can be added to the menu and inserted in the menubar
   * without making the menu visible. This attribute is available in
   * Mac OS X after 10.1.
   */
  kMenuAttrHidden               = (1 << 4)
};


/*
 *  MenuItemAttributes
 *  
 *  Summary:
 *    Menu item attributes control behavior of individual menu items.
 *    They are used with the Get/ChangeMenuItemAttributes APIs.
 */
typedef UInt32 MenuItemAttributes;
enum {

  /*
   * This item is disabled.
   */
  kMenuItemAttrDisabled         = (1 << 0),

  /*
   * This item's icon is disabled.
   */
  kMenuItemAttrIconDisabled     = (1 << 1),

  /*
   * Allows the parent item of a submenu to be selectable.
   */
  kMenuItemAttrSubmenuParentChoosable = (1 << 2),

  /*
   * This item changes dynamically based on modifier key state.
   */
  kMenuItemAttrDynamic          = (1 << 3),

  /*
   * This item is not part of the same dynamic group as the previous
   * item.
   */
  kMenuItemAttrNotPreviousAlternate = (1 << 4),

  /*
   * This item is not drawn when the menu is displayed. It is also not
   * included in command key matching, unless the item also has either
   * the Dynamic or IncludeInCmdKeyMatching attributes.
   */
  kMenuItemAttrHidden           = (1 << 5),

  /*
   * This item is a separator; the text of the item is ignored.
   */
  kMenuItemAttrSeparator        = (1 << 6),

  /*
   * This item is a menu section header; it is disabled and
   * unselectable.
   */
  kMenuItemAttrSectionHeader    = (1 << 7),

  /*
   * Metacharacters in the text of this item (such as the dash) are
   * ignored.
   */
  kMenuItemAttrIgnoreMeta       = (1 << 8),

  /*
   * This item is recognized by IsMenuKeyEvent when it is passed an
   * auto-repeat keyboard event.
   */
  kMenuItemAttrAutoRepeat       = (1 << 9),

  /*
   * When MenuEvent and IsMenuKeyEvent compare this item's keyboard
   * equivalent against a keyboard event, they use the item's virtual
   * keycode equivalent rather than its character code equivalent.
   */
  kMenuItemAttrUseVirtualKey    = (1 << 10),

  /*
   * This item is drawn in a customized fashion by the application.
   * Causes custom menu item drawing Carbon events to be sent. This
   * attribute is available in CarbonLib 1.4 and Mac OS X 10.1, and
   * later.
   */
  kMenuItemAttrCustomDraw       = (1 << 11),

  /*
   * This item is examined during command key matching by MenuKey,
   * MenuEvent, and IsMenuKeyEvent. Normally, visible menu items are
   * included in command key matching, but hidden menu items are
   * excluded (unless the item also has the Dynamic menu item
   * attribute). The IncludeInCmdKeyMatching attribute can be used to
   * force a hidden, non-dynamic menu item to be included in command
   * key matching when it normally wouldn't. This attribute is
   * available in CarbonLib 1.6 and later, and in Mac OS X after 10.1.
   */
  kMenuItemAttrIncludeInCmdKeyMatching = (1 << 12)
};


/*
 *  MenuTrackingMode
 *  
 *  Summary:
 *    A menu tracking mode constant is part of the
 *    kEventMenuBeginTracking and kEventMenuChangeTrackingMode Carbon
 *    events. It indicates whether menus are being tracked using the
 *    mouse or keyboard.
 */
typedef UInt32 MenuTrackingMode;
enum {

  /*
   * Menus are being tracked using the mouse.
   */
  kMenuTrackingModeMouse        = 1,

  /*
   * Menus are being tracked using the keyboard.
   */
  kMenuTrackingModeKeyboard     = 2
};


/*
 *  MenuEventOptions
 *  
 *  Summary:
 *    Menu event options control how the menus are searched for an item
 *    matching a particular keyboard event. They are used with the
 *    IsMenuKeyEvent API.
 */
typedef UInt32 MenuEventOptions;
enum {

  /*
   * Disabled items are examined for a match.
   */
  kMenuEventIncludeDisabledItems = 0x0001,

  /*
   * Don't hilite the menu title if a match is found.
   */
  kMenuEventQueryOnly           = 0x0002,

  /*
   * Don't look for a match in submenus of the starting menu.
   */
  kMenuEventDontCheckSubmenus   = 0x0004
};

/*--------------------------------------------------------------------------------------*/
/*  o Menu Types                                                                        */
/*--------------------------------------------------------------------------------------*/
typedef SInt16                          MenuID;
typedef UInt16                          MenuItemIndex;
typedef UInt32                          MenuCommand;
#if !OPAQUE_TOOLBOX_STRUCTS
struct MenuInfo {
  MenuID              menuID;                 /* in Carbon use Get/SetMenuID*/
  short               menuWidth;              /* in Carbon use Get/SetMenuWidth*/
  short               menuHeight;             /* in Carbon use Get/SetMenuHeight*/
  Handle              menuProc;               /* not supported in Carbon*/
  long                enableFlags;            /* in Carbon use Enable/DisableMenuItem, IsMenuItemEnable*/
  Str255              menuData;               /* in Carbon use Get/SetMenuTitle*/
};
typedef struct MenuInfo                 MenuInfo;
typedef MenuInfo *                      MenuPtr;
typedef MenuPtr *                       MenuHandle;
typedef MenuHandle                      MenuRef;
#else
typedef struct OpaqueMenuRef*           MenuRef;
/* MenuHandle is old name for MenuRef*/
typedef MenuRef                         MenuHandle;
#endif  /* !OPAQUE_TOOLBOX_STRUCTS */


/*
   A MenuBarHandle is a handle to a MenuBarHeader. An instance of this structure is returned
   by the GetMenuBar and GetNewMBar APIs. It is typedef'd to a plain Handle to retain
   source compatibility with previous versions of this header file.
*/
typedef Handle                          MenuBarHandle;

/*
 *  MenuBarHeader
 *  
 *  Summary:
 *    This structure is contained in a MenuBarHandle. It contains a
 *    list of the non-hierarchical menus that have been inserted into
 *    the menubar.
 *  
 *  Discussion:
 *    The MenuBarHandle is a dynamically sized object which cannot be
 *    directly expressed as a C or Pascal structure. First is the
 *    MenuBarHeader structure, followed by a dynamically sized array of
 *    MenuBarMenus, one for each menu. This array is followed by the
 *    HMenuBarHeader, followed by another dynamically sized array of
 *    HMenuBarMenus, one for each hierarchical menu.
 */
struct MenuBarHeader {

  /*
   * Offset in bytes from the start of the header to the last menu in
   * the array of MenuBarMenus.
   */
  UInt16              lastMenu;

  /*
   * Global coordinate of the right edge of the rightmost menu; unused
   * in a MenuBarHandle returned by GetMenuBar or GetNewMBar.
   */
  SInt16              lastRight;

  /*
   * The MBDF resource ID; unused in a MenuBarHandle returned by
   * GetMenuBar or GetNewMBar.
   */
  SInt16              mbResID;
};
typedef struct MenuBarHeader            MenuBarHeader;

/*
 *  HMenuBarHeader
 *  
 *  Summary:
 *    This structure is contained in a MenuBarHandle. It contains a
 *    list of the hierarchical menus that have been inserted into the
 *    menubar with InsertMenu( menu, -1 ).
 *  
 *  Discussion:
 *    The hierarchical portion of the menubar follows the
 *    non-hierarchical portion in a menubar handle. The hierarchical
 *    portion begins with the HMenuBarHeader structure, followed by a
 *    dynamically sized array of HMenuBarMenus.
 */
struct HMenuBarHeader {

  /*
   * Offset in bytes from the start of the header to the last menu in
   * the array of HMenuBarMenus.
   */
  UInt16              lastHMenu;

  /*
   * Saved bits behind the hilited menu title; unused in a
   * MenuBarHandle returned by GetMenuBar or GetNewMBar.
   */
  PixMapHandle        menuTitleBits;
};
typedef struct HMenuBarHeader           HMenuBarHeader;

/*
 *  MenuBarMenu
 *  
 *  Summary:
 *    This structure contains a single menu in the menubar. It is an
 *    element in an array in the MenuBarHeader data strucuture.
 */
struct MenuBarMenu {

  /*
   * A menu in the menubar.
   */
  MenuRef             menu;

  /*
   * The global coordinate of the left edge of the menu title; unused
   * in a MenuBarHandle returned by GetMenuBar or GetNewMBar.
   */
  SInt16              menuLeft;
};
typedef struct MenuBarMenu              MenuBarMenu;

/*
 *  HMenuBarMenu
 *  
 *  Summary:
 *    This structure contains a single hierarchical menu in the
 *    menubar. It is an element in an array in the HMenuBarHeader data
 *    strucuture.
 */
struct HMenuBarMenu {

  /*
   * An hierarchical menu in the menubar.
   */
  MenuRef             menu;

  /*
   * This field is currently unused.
   */
  SInt16              reserved;
};
typedef struct HMenuBarMenu             HMenuBarMenu;
struct MCEntry {
  MenuID              mctID;                  /*menu ID.  ID = 0 is the menu bar*/
  short               mctItem;                /*menu Item. Item = 0 is a title*/
  RGBColor            mctRGB1;                /*usage depends on ID and Item*/
  RGBColor            mctRGB2;                /*usage depends on ID and Item*/
  RGBColor            mctRGB3;                /*usage depends on ID and Item*/
  RGBColor            mctRGB4;                /*usage depends on ID and Item*/
  short               mctReserved;            /*reserved for internal use*/
};
typedef struct MCEntry                  MCEntry;
typedef MCEntry *                       MCEntryPtr;
typedef MCEntry                         MCTable[1];
typedef MCEntry *                       MCTablePtr;
typedef MCTablePtr *                    MCTableHandle;
struct MenuCRsrc {
  short               numEntries;             /*number of entries*/
  MCTable             mcEntryRecs;            /*ARRAY [1..numEntries] of MCEntry*/
};
typedef struct MenuCRsrc                MenuCRsrc;
typedef MenuCRsrc *                     MenuCRsrcPtr;
typedef MenuCRsrcPtr *                  MenuCRsrcHandle;
#if TARGET_OS_WIN32
/* QuickTime 3.0 */
struct MenuAccessKeyRec {
  short               count;
  long                flags;
  unsigned char       keys[1];
};
typedef struct MenuAccessKeyRec         MenuAccessKeyRec;
typedef MenuAccessKeyRec *              MenuAccessKeyPtr;
typedef MenuAccessKeyPtr *              MenuAccessKeyHandle;
#if CALL_NOT_IN_CARBON
/*
 *  SetMenuItemHotKey()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
SetMenuItemHotKey(
  MenuRef   menu,
  short     itemID,
  char      hotKey,
  long      flags);


#endif  /* CALL_NOT_IN_CARBON */

#endif  /* TARGET_OS_WIN32 */


/*
 *  MenuTrackingData
 *  
 *  Summary:
 *    The MenuTrackingData structure contains information about a menu
 *    currently being displayed. It is used with the
 *    GetMenuTrackingData API.
 */
struct MenuTrackingData {
  MenuRef             menu;
  MenuItemIndex       itemSelected;
  MenuItemIndex       itemUnderMouse;
  Rect                itemRect;
  SInt32              virtualMenuTop;
  SInt32              virtualMenuBottom;
};
typedef struct MenuTrackingData         MenuTrackingData;
typedef MenuTrackingData *              MenuTrackingDataPtr;

/*
 *  MDEFHiliteItemData
 *  
 *  Summary:
 *    The MDEFHiliteItemData structure contains information about which
 *    menu items should be hilited and unhilited as the user moves
 *    through the menus. It is used by menu definition functions, which
 *    receive a pointer to an MDEFHiliteItemData structure as the
 *    whichItem parameter during kMenuHiliteItemMsg.
 */
struct MDEFHiliteItemData {

  /*
   * The item that was previously selected. It should be redrawn in an
   * unhilited state. May be zero if no item was previously selected.
   */
  MenuItemIndex       previousItem;

  /*
   * The item that is now selected. It should be redrawn in a hilited
   * state. May be zero if no item is now selected.
   */
  MenuItemIndex       newItem;

  /*
   * A CoreGraphics context that the MDEF should draw into. The Menu
   * Manager will flush the context after the MDEF has returned.
   */
  void *              context;
};
typedef struct MDEFHiliteItemData       MDEFHiliteItemData;
typedef MDEFHiliteItemData *            MDEFHiliteItemDataPtr;
typedef MDEFHiliteItemData              HiliteMenuItemData;
typedef MDEFHiliteItemDataPtr           HiliteMenuItemDataPtr;

/*
 *  MDEFDrawData
 *  
 *  Summary:
 *    The MDEFDrawData structure contains information needed to draw a
 *    menu. It is used by menu definition functions, which receive a
 *    pointer to an MDEFDrawData structure as the whichItem parameter
 *    during kMenuDrawMsg.
 */
struct MDEFDrawData {

  /*
   * Information about the menu being drawn. The MDEF should fill in
   * the virtualMenuTop and virtualMenuBottom fields of this structure
   * while drawing the menu.
   */
  MenuTrackingData    trackingData;

  /*
   * A CoreGraphics context that the MDEF should draw into. The Menu
   * Manager will flush the context after the MDEF has returned.
   */
  void *              context;
};
typedef struct MDEFDrawData             MDEFDrawData;
typedef MDEFDrawData *                  MDEFDrawDataPtr;

/*
 *  MDEFFindItemData
 *  
 *  Summary:
 *    The MDEFFindItemData structure contains information needed to
 *    determine which item is currently selected by the user. It is
 *    used by menu definition functions, which receive a pointer to an
 *    MDEFDrawData structure as the whichItem parameter during
 *    kMenuFindItemMsg.
 */
struct MDEFFindItemData {

  /*
   * Information about the menu being drawn. The MDEF should fill in
   * the itemSelected, itemUnderMouse, and itemRect fields of this
   * structure after determining which item is at the specified point.
   */
  MenuTrackingData    trackingData;

  /*
   * A CoreGraphics context that the MDEF should draw into if it needs
   * to scroll the menu during the FindItem message. The Menu Manager
   * will flush the context after the MDEF has returned.
   */
  void *              context;
};
typedef struct MDEFFindItemData         MDEFFindItemData;
typedef MDEFFindItemData *              MDEFFindItemDataPtr;

/*
 *  MDEFDrawItemsData
 *  
 *  Summary:
 *    The MDEFDrawItemsData structure contains information about which
 *    menu items to redraw. It is used by menu definition functions,
 *    which receive a pointer to an MDEFDrawItemsData structure as the
 *    whichItem parameter during kMenuDrawItemsMsg.
 */
struct MDEFDrawItemsData {

  /*
   * The first item to draw.
   */
  MenuItemIndex       firstItem;

  /*
   * The last item to draw.
   */
  MenuItemIndex       lastItem;

  /*
   * Information about the menu's tracking state. The virtualMenuTop
   * and virtualMenuBottom fields in this structure will be the most
   * useful in handling the DrawItems message.
   */
  MenuTrackingData *  trackingData;

  /*
   * A CoreGraphics context that the MDEF should draw into. The Menu
   * Manager will flush the context after the MDEF returns.
   */
  void *              context;
};
typedef struct MDEFDrawItemsData        MDEFDrawItemsData;
typedef MDEFDrawItemsData *             MDEFDrawItemsDataPtr;

/*
 *  Summary:
 *    A MenuItemDataFlags value indicates which fields of a
 *    MenuItemDataRec structure should be used by the
 *    Copy/SetMenuItemData APIs. All MenuItemDataFlags may be used when
 *    getting or setting the contents of a menu item; some may also be
 *    used when getting or setting information about the menu itself,
 *    if the item index given to Copy/SetMenuItemData is 0.
 */
enum {

  /*
   * Set or return the Str255 text of a menu using the
   * MenuItemDataRec.text field. If getting the text, the text field
   * must be initialized with a pointer to a Str255 variable before
   * calling CopyMenuItemData. If both kMenuItemDataText and
   * kMenuItemCFString are set on entry to CopyMenuItemData, the API
   * will determine whether the menu text was most recently set using a
   * Str255 or CFString, and return only that text format; the flags
   * value for the other format will be cleared. Valid for both menu
   * items and the menu title (if item number is 0).
   */
  kMenuItemDataText             = (1 << 0),

  /*
   * Set or return the mark character of a menu item using the
   * MenuItemDataRec.mark field. Valid only for menu items.
   */
  kMenuItemDataMark             = (1 << 1),

  /*
   * Set or return the command key of a menu item using the
   * MenuItemDataRec.cmdKey field. Valid only for menu items.
   */
  kMenuItemDataCmdKey           = (1 << 2),

  /*
   * Set or return the command key glyph of a menu item using the
   * MenuItemDataRec.cmdKeyGlyph field. Valid only for menu items.
   */
  kMenuItemDataCmdKeyGlyph      = (1 << 3),

  /*
   * Set or return the command key modifiers of a menu item using the
   * MenuItemDataRec.cmdKeyModifiers field. Valid only for menu items.
   */
  kMenuItemDataCmdKeyModifiers  = (1 << 4),

  /*
   * Set or return the QuickDraw text style of a menu item using the
   * MenuItemDataRec.style field. Valid only for menu items.
   */
  kMenuItemDataStyle            = (1 << 5),

  /*
   * Set or return the enable state of a menu using the
   * MenuItemDataRec.enabled field. Valid for both menu items and the
   * menu itself (if item number is 0).
   */
  kMenuItemDataEnabled          = (1 << 6),

  /*
   * Set or return the enable state of a menu item icon using the
   * MenuItemDataRec.iconEnabled field. Valid only for menu items.
   */
  kMenuItemDataIconEnabled      = (1 << 7),

  /*
   * Set or return the icon resource ID of a menu item using the
   * MenuItemDataRec.iconID field. Valid only for menu items.
   */
  kMenuItemDataIconID           = (1 << 8),

  /*
   * Set or return the icon handle of a menu item using the
   * MenuItemDataRec.iconType and MenuItemDataRec.iconHandle fields.
   * Both fields must be initialized if setting the icon handle; both
   * fields will be returned when getting the handle. The iconType
   * field should contain one of the constants kMenuIconType,
   * kMenuShrinkIconType, kMenuSmallIconType, kMenuColorIconType,
   * kMenuIconSuiteType, or kMenuIconRefType. An icon handle may be a
   * handle to an ICON resource, a SICN resource, a cicn resource, an
   * IconSuite, or an IconRef. Valid only for menu items.
   */
  kMenuItemDataIconHandle       = (1 << 9),

  /*
   * Set or return the command ID of a menu item using the
   * MenuItemDataRec.cmdID field. Valid only for menu items.
   */
  kMenuItemDataCommandID        = (1 << 10),

  /*
   * Set or return the text encoding of a menu item using the
   * MenuItemDataRec.encoding field. Valid only for menu items.
   */
  kMenuItemDataTextEncoding     = (1 << 11),
  kMenuItemDataSubmenuID        = (1 << 12),
  kMenuItemDataSubmenuHandle    = (1 << 13),
  kMenuItemDataFontID           = (1 << 14),
  kMenuItemDataRefcon           = (1 << 15),
  kMenuItemDataAttributes       = (1 << 16),
  kMenuItemDataCFString         = (1 << 17),

  /*
   * Set or return the properties of a menu using the
   * MenuItemDataRec.properties field. If setting the properties, the
   * properties field should contain a collection with the new
   * properties; existing menu properties with the same collection
   * creator and tag will be replaced by the new properties. If getting
   * the properties, the properties field should either be set to NULL
   * or to a valid Collection. If NULL, a new collection is allocated
   * by the CopyMenuItemData and returned in the properties field. If
   * not NULL, the entire contents of the collection are replaced by
   * the properties of the menu. Valid for both menu items and the menu
   * itself (if item number is 0).
   */
  kMenuItemDataProperties       = (1 << 18),

  /*
   * Set or return the item indent level of a menu item using the
   * MenuItemDataRec.indent field. Valid only for menu items.
   */
  kMenuItemDataIndent           = (1 << 19),

  /*
   * Set or return the virtual key code keyboard equivalent of a menu
   * item using the MenuItemDataRec.cmdVirtualKey field. Valid only for
   * menu items. On output, only valid if the item has the
   * kMenuItemAttrUseVirtualKeyCode attribute.
   */
  kMenuItemDataCmdVirtualKey    = (1 << 20),
  kMenuItemDataAllDataVersionOne = 0x000FFFFF,
  kMenuItemDataAllDataVersionTwo = kMenuItemDataAllDataVersionOne | kMenuItemDataCmdVirtualKey
};

typedef UInt64                          MenuItemDataFlags;

/*
 *  MenuItemDataRec
 *  
 *  Summary:
 *    The MenuItemDataRec structure is used to get and change aspects
 *    of a menu item. It is used with the Copy/SetMenuItemData APIs.
 *  
 *  Discussion:
 *    When using this structure with Copy/SetMenuItemData, the caller
 *    must first set the whichData field to a combination of
 *    MenuItemDataFlags indicating which specific data should be
 *    retrieved or set. Some fields also require initialization before
 *    calling CopyMenuItemData; see the individual MenuItemDataFlags
 *    documentation for details.
 */
struct MenuItemDataRec {
  MenuItemDataFlags   whichData;
  StringPtr           text;
  UniChar             mark;
  UniChar             cmdKey;
  UInt32              cmdKeyGlyph;
  UInt32              cmdKeyModifiers;
  Style               style;
  Boolean             enabled;
  Boolean             iconEnabled;
  UInt8               filler1;
  SInt32              iconID;
  UInt32              iconType;
  Handle              iconHandle;
  MenuCommand         cmdID;
  TextEncoding        encoding;
  MenuID              submenuID;
  MenuRef             submenuHandle;
  SInt32              fontID;
  UInt32              refcon;
  OptionBits          attr;
  CFStringRef         cfText;
  Collection          properties;
  UInt32              indent;
  UInt16              cmdVirtualKey;
};
typedef struct MenuItemDataRec          MenuItemDataRec;
typedef MenuItemDataRec *               MenuItemDataPtr;
typedef UInt32                          MenuItemID;
/*--------------------------------------------------------------------------------------*/
/*  o Menu ProcPtrs                                                                     */
/*                                                                                      */
/*  All of these procs are considered deprecated.  Developers interested in portability */
/*  to Carbon should avoid them entirely, if at all possible.                           */
/*--------------------------------------------------------------------------------------*/
typedef CALLBACK_API( void , MenuDefProcPtr )(short message, MenuRef theMenu, Rect *menuRect, Point hitPt, short *whichItem);
typedef STACK_UPP_TYPE(MenuDefProcPtr)                          MenuDefUPP;
/*
 *  NewMenuDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( MenuDefUPP )
NewMenuDefUPP(MenuDefProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppMenuDefProcInfo = 0x0000FF80 };  /* pascal no_return_value Func(2_bytes, 4_bytes, 4_bytes, 4_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(MenuDefUPP) NewMenuDefUPP(MenuDefProcPtr userRoutine) { return (MenuDefUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuDefProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewMenuDefUPP(userRoutine) (MenuDefUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuDefProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeMenuDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
DisposeMenuDefUPP(MenuDefUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeMenuDefUPP(MenuDefUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeMenuDefUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeMenuDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API_C( void )
InvokeMenuDefUPP(
  short       message,
  MenuRef     theMenu,
  Rect *      menuRect,
  Point       hitPt,
  short *     whichItem,
  MenuDefUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeMenuDefUPP(short message, MenuRef theMenu, Rect * menuRect, Point hitPt, short * whichItem, MenuDefUPP userUPP) { CALL_FIVE_PARAMETER_UPP(userUPP, uppMenuDefProcInfo, message, theMenu, menuRect, hitPt, whichItem); }
  #else
    #define InvokeMenuDefUPP(message, theMenu, menuRect, hitPt, whichItem, userUPP) CALL_FIVE_PARAMETER_UPP((userUPP), uppMenuDefProcInfo, (message), (theMenu), (menuRect), (hitPt), (whichItem))
  #endif
#endif

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewMenuDefProc(userRoutine)                         NewMenuDefUPP(userRoutine)
    #define CallMenuDefProc(userRoutine, message, theMenu, menuRect, hitPt, whichItem) InvokeMenuDefUPP(message, theMenu, menuRect, hitPt, whichItem, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

typedef CALLBACK_API( long , MenuBarDefProcPtr )(short selector, short message, short parameter1, long parameter2);
typedef CALLBACK_API( void , MenuHookProcPtr )(void);
typedef CALLBACK_API_REGISTER68K( short , MBarHookProcPtr, (Rect * menuRect) );
typedef STACK_UPP_TYPE(MenuBarDefProcPtr)                       MenuBarDefUPP;
typedef STACK_UPP_TYPE(MenuHookProcPtr)                         MenuHookUPP;
typedef REGISTER_UPP_TYPE(MBarHookProcPtr)                      MBarHookUPP;
#if CALL_NOT_IN_CARBON
/*
 *  NewMenuBarDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( MenuBarDefUPP )
NewMenuBarDefUPP(MenuBarDefProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppMenuBarDefProcInfo = 0x00003AB0 };  /* pascal 4_bytes Func(2_bytes, 2_bytes, 2_bytes, 4_bytes) */
  #ifdef __cplusplus
    inline DEFINE_API_C(MenuBarDefUPP) NewMenuBarDefUPP(MenuBarDefProcPtr userRoutine) { return (MenuBarDefUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuBarDefProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewMenuBarDefUPP(userRoutine) (MenuBarDefUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuBarDefProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewMenuHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( MenuHookUPP )
NewMenuHookUPP(MenuHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppMenuHookProcInfo = 0x00000000 };  /* pascal no_return_value Func() */
  #ifdef __cplusplus
    inline DEFINE_API_C(MenuHookUPP) NewMenuHookUPP(MenuHookProcPtr userRoutine) { return (MenuHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewMenuHookUPP(userRoutine) (MenuHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMenuHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  NewMBarHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( MBarHookUPP )
NewMBarHookUPP(MBarHookProcPtr userRoutine);
#if !OPAQUE_UPP_TYPES
  enum { uppMBarHookProcInfo = 0x000000CF };  /* SPECIAL_CASE_PROCINFO(12) */
  #ifdef __cplusplus
    inline DEFINE_API_C(MBarHookUPP) NewMBarHookUPP(MBarHookProcPtr userRoutine) { return (MBarHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMBarHookProcInfo, GetCurrentArchitecture()); }
  #else
    #define NewMBarHookUPP(userRoutine) (MBarHookUPP)NewRoutineDescriptor((ProcPtr)(userRoutine), uppMBarHookProcInfo, GetCurrentArchitecture())
  #endif
#endif

/*
 *  DisposeMenuBarDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeMenuBarDefUPP(MenuBarDefUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeMenuBarDefUPP(MenuBarDefUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeMenuBarDefUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeMenuHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeMenuHookUPP(MenuHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeMenuHookUPP(MenuHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeMenuHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  DisposeMBarHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
DisposeMBarHookUPP(MBarHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) DisposeMBarHookUPP(MBarHookUPP userUPP) { DisposeRoutineDescriptor((UniversalProcPtr)userUPP); }
  #else
      #define DisposeMBarHookUPP(userUPP) DisposeRoutineDescriptor(userUPP)
  #endif
#endif

/*
 *  InvokeMenuBarDefUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( long )
InvokeMenuBarDefUPP(
  short          selector,
  short          message,
  short          parameter1,
  long           parameter2,
  MenuBarDefUPP  userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(long) InvokeMenuBarDefUPP(short selector, short message, short parameter1, long parameter2, MenuBarDefUPP userUPP) { return (long)CALL_FOUR_PARAMETER_UPP(userUPP, uppMenuBarDefProcInfo, selector, message, parameter1, parameter2); }
  #else
    #define InvokeMenuBarDefUPP(selector, message, parameter1, parameter2, userUPP) (long)CALL_FOUR_PARAMETER_UPP((userUPP), uppMenuBarDefProcInfo, (selector), (message), (parameter1), (parameter2))
  #endif
#endif

/*
 *  InvokeMenuHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
InvokeMenuHookUPP(MenuHookUPP userUPP);
#if !OPAQUE_UPP_TYPES
  #ifdef __cplusplus
      inline DEFINE_API_C(void) InvokeMenuHookUPP(MenuHookUPP userUPP) { CALL_ZERO_PARAMETER_UPP(userUPP, uppMenuHookProcInfo); }
  #else
    #define InvokeMenuHookUPP(userUPP) CALL_ZERO_PARAMETER_UPP((userUPP), uppMenuHookProcInfo)
  #endif
#endif

/*
 *  InvokeMBarHookUPP()
 *  
 *  Availability:
 *    Non-Carbon CFM:   available as macro/inline
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( short )
InvokeMBarHookUPP(
  Rect *       menuRect,
  MBarHookUPP  userUPP);
#if !OPAQUE_UPP_TYPES && (!TARGET_OS_MAC || !TARGET_CPU_68K || TARGET_RT_MAC_CFM)
  #ifdef __cplusplus
      inline DEFINE_API_C(short) InvokeMBarHookUPP(Rect * menuRect, MBarHookUPP userUPP) { return (short)CALL_ONE_PARAMETER_UPP(userUPP, uppMBarHookProcInfo, menuRect); }
  #else
    #define InvokeMBarHookUPP(menuRect, userUPP) (short)CALL_ONE_PARAMETER_UPP((userUPP), uppMBarHookProcInfo, (menuRect))
  #endif
#endif

#endif  /* CALL_NOT_IN_CARBON */

#if CALL_NOT_IN_CARBON || OLDROUTINENAMES
    /* support for pre-Carbon UPP routines: New...Proc and Call...Proc */
    #define NewMenuBarDefProc(userRoutine)                      NewMenuBarDefUPP(userRoutine)
    #define NewMenuHookProc(userRoutine)                        NewMenuHookUPP(userRoutine)
    #define NewMBarHookProc(userRoutine)                        NewMBarHookUPP(userRoutine)
    #define CallMenuBarDefProc(userRoutine, selector, message, parameter1, parameter2) InvokeMenuBarDefUPP(selector, message, parameter1, parameter2, userRoutine)
    #define CallMenuHookProc(userRoutine)                       InvokeMenuHookUPP(userRoutine)
    #define CallMBarHookProc(userRoutine, menuRect)             InvokeMBarHookUPP(menuRect, userRoutine)
#endif /* CALL_NOT_IN_CARBON */

enum {
  kMenuDefProcPtr               = 0     /* raw proc-ptr access based on old MDEF */
};

typedef UInt32                          MenuDefType;
struct MenuDefSpec {
  MenuDefType         defType;
  union {
    MenuDefUPP          defProc;
  }                       u;
};
typedef struct MenuDefSpec              MenuDefSpec;
typedef MenuDefSpec *                   MenuDefSpecPtr;
/*--------------------------------------------------------------------------------------*/
/*  o Menu Manager Initialization                                                       */
/*--------------------------------------------------------------------------------------*/
#if CALL_NOT_IN_CARBON
/*
 *  InitProcMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
InitProcMenu(short resID)                                     ONEWORDINLINE(0xA808);


/*
 *  InitMenus()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
InitMenus(void)                                               ONEWORDINLINE(0xA930);


/*--------------------------------------------------------------------------------------*/
/*  o Menu Manipulation                                                                 */
/*--------------------------------------------------------------------------------------*/
#endif  /* CALL_NOT_IN_CARBON */

/*
 *  NewMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MenuRef )
NewMenu(
  MenuID             menuID,
  ConstStr255Param   menuTitle)                               ONEWORDINLINE(0xA931);


/*
 *  [Mac]GetMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacGetMenu GetMenu
#endif
EXTERN_API( MenuRef )
MacGetMenu(short resourceID)                                  ONEWORDINLINE(0xA9BF);


/*
 *  DisposeMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeMenu(MenuRef theMenu)                                  ONEWORDINLINE(0xA932);


/*
 *  CalcMenuSize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
CalcMenuSize(MenuRef theMenu)                                 ONEWORDINLINE(0xA948);


#if CALL_NOT_IN_CARBON
/*
 *  CountMItems()
 *  
 *  Summary:
 *    Renamed to CountMenuItems in Carbon
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( UInt16 )
CountMItems(MenuRef theMenu)                                  ONEWORDINLINE(0xA950);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  CountMenuItems()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.3 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt16 )
CountMenuItems(MenuRef theMenu)                               ONEWORDINLINE(0xA950);
#if CALL_NOT_IN_CARBON && !TARGET_CPU_68K
  #ifdef __cplusplus
    inline DEFINE_API(UInt16 ) CountMenuItems(MenuRef theMenu) { return CountMItems(theMenu); }
  #else
    #define CountMenuItems(theMenu) (CountMItems(theMenu))
  #endif
#endif




/* Routines available in Mac OS 8.5 and later, and on Mac OS 8.1 and later using CarbonLib 1.1 and later*/

/*
 *  GetMenuFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuFont(
  MenuRef   menu,
  SInt16 *  outFontID,
  UInt16 *  outFontSize);


/*
 *  SetMenuFont()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuFont(
  MenuRef   menu,
  SInt16    inFontID,
  UInt16    inFontSize);


/*
 *  GetMenuExcludesMarkColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
GetMenuExcludesMarkColumn(MenuRef menu);


/*
 *  SetMenuExcludesMarkColumn()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuExcludesMarkColumn(
  MenuRef   menu,
  Boolean   excludesMark);


/*
 *  RegisterMenuDefinition()
 *  
 *  Summary:
 *    Registers or unregisters a binding between a resource ID and a
 *    menu definition function.
 *  
 *  Discussion:
 *    In the Mac OS 8.x Menu Manager, a 'MENU' resource can contain an
 *    embedded MDEF procID that is used by the Menu Manager as the
 *    resource ID of an 'MDEF' resource to measure and draw the menu.
 *    The 'MDEF' resource is loaded by the Menu Manager when you load
 *    the menu with GetMenu. Since MDEFs can no longer be packaged as
 *    code resources on Carbon, the procID can no longer refer directly
 *    to an MDEF resource. However, using RegisterMenuDefinition you
 *    can instead specify a UniversalProcPtr pointing to code in your
 *    application code fragment.
 *  
 *  Parameters:
 *    
 *    inResID:
 *      An MDEF proc ID, as used in a 'MENU' resource.
 *    
 *    inDefSpec:
 *      Specifies the MenuDefUPP that should be used for menus with the
 *      given MDEF proc ID. Passing NULL allows you to unregister the
 *      menu definition that had been associated with the given MDEF
 *      proc ID.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RegisterMenuDefinition(
  SInt16           inResID,
  MenuDefSpecPtr   inDefSpec);


/*
 *  CreateNewMenu()
 *  
 *  Summary:
 *    Creates a new, untitled, empty menu.
 *  
 *  Discussion:
 *    CreateNewMenu is preferred over NewMenu because it allows you to
 *    specify the menu's attributes and it does not require you to
 *    specify a Str255-based menu title. To set the menu title, you can
 *    use either SetMenuTitle or SetMenuTitleWithCFString.
 *  
 *  Parameters:
 *    
 *    inMenuID:
 *      The menu ID to use for the new menu.
 *    
 *    inMenuAttributes:
 *      The menu attributes to use for the new menu.
 *    
 *    outMenuRef:
 *      On exit, contains the new menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateNewMenu(
  MenuID           inMenuID,
  MenuAttributes   inMenuAttributes,
  MenuRef *        outMenuRef);


/*
 *  CreateCustomMenu()
 *  
 *  Summary:
 *    Creates a new, untitled, empty menu using a custom menu
 *    definition function.
 *  
 *  Discussion:
 *    Similar to CreateNewMenu, but also allows you to specify a custom
 *    menu definition function.
 *  
 *  Parameters:
 *    
 *    inDefSpec:
 *      Specifies a custom menu definition function. defSpec->defType
 *      must be kMenuDefProcPtr.
 *    
 *    inMenuID:
 *      The menu ID to use for the new menu.
 *    
 *    inMenuAttributes:
 *      The menu attributes to use for the new menu.
 *    
 *    outMenuRef:
 *      On exit, contains the new menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateCustomMenu(
  const MenuDefSpec *  inDefSpec,
  MenuID               inMenuID,
  MenuAttributes       inMenuAttributes,
  MenuRef *            outMenuRef);


/*
 *  IsValidMenu()
 *  
 *  Summary:
 *    Determines if a menu is valid.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu to check for validity.
 *  
 *  Result:
 *    Indicates whether the menu is valid.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsValidMenu(MenuRef inMenu);


/*
 *  GetMenuRetainCount()
 *  
 *  Summary:
 *    Returns the retain count of this menu.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose retain count to increment.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ItemCount )
GetMenuRetainCount(MenuRef inMenu);


/*
 *  RetainMenu()
 *  
 *  Summary:
 *    Increments the retain count of a menu.
 *  
 *  Discussion:
 *    RetainMenu does not create a new menu. It simply adds one to the
 *    retain count. If called on a menu that was not created by
 *    CarbonLib, it will not affect the menu's retain count.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose retain count to increment.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RetainMenu(MenuRef inMenu);


/*
 *  ReleaseMenu()
 *  
 *  Summary:
 *    Decrements the retain count of a menu.
 *  
 *  Discussion:
 *    If called on a menu that was not created by CarbonLib, it will
 *    not affect the menu's retain count.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose retain count to decrement. If the retain count
 *      falls to zero, the menu is destroyed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ReleaseMenu(MenuRef inMenu);


/*
 *  DuplicateMenu()
 *  
 *  Summary:
 *    Creates a new menu that is a copy of another menu.
 *  
 *  Discussion:
 *    Unlike RetainMenu, DuplicateMenu creates an entirely new menu
 *    that is an exact copy of the original menu. The MDEF for the new
 *    menu will receive an init message after the menu has been fully
 *    created.
 *  
 *  Parameters:
 *    
 *    inSourceMenu:
 *      The menu to duplicate.
 *    
 *    outMenu:
 *      On exit, a copy of the source menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DuplicateMenu(
  MenuRef    inSourceMenu,
  MenuRef *  outMenu);


/*
 *  CopyMenuTitleAsCFString()
 *  
 *  Summary:
 *    Returns a CFString containing the title of a menu.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose title to return.
 *    
 *    outString:
 *      On exit, a CFString containing the menu's title. This string
 *      must be released by the caller.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CopyMenuTitleAsCFString(
  MenuRef        inMenu,
  CFStringRef *  outString);


/*
 *  SetMenuTitleWithCFString()
 *  
 *  Summary:
 *    Sets the title of a menu to the text contained in a CFString.
 *  
 *  Discussion:
 *    The Menu Manager will either make its own copy or just increment
 *    the refcount of the CFString before returning from
 *    SetMenuTitleWithCFString, depending on whether the string is
 *    mutable or immutable. If the string is mutable, modifying the
 *    string after calling SetMenuTitleWithCFString will have no effect
 *    on the menu's actual title. The caller may release the string
 *    after calling SetMenuTitleWithCFString.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose title to set.
 *    
 *    inString:
 *      The string containing the new menu title text.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuTitleWithCFString(
  MenuRef       inMenu,
  CFStringRef   inString);


/*
 *  SetMenuTitleIcon()
 *  
 *  Summary:
 *    Sets the title of a menu to be an icon.
 *  
 *  Discussion:
 *    The Menu Manager takes ownership of the supplied icon after this
 *    call. When a menu with an title icon is disposed, the Menu
 *    Manager will dispose the icon also; the Menu Manager will also
 *    dispose of the current title icon when a new text or icon title
 *    is supplied for a menu. If an IconRef is specified, the Menu
 *    Manager will increment its refcount, so you may freely release
 *    your reference to the IconRef without invalidating the Menu
 *    Manager's copy. The menubar will be invalidated by this call, and
 *    redrawn at the next opportunity.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose title to set.
 *    
 *    inType:
 *      The type of icon being used to specify the icon title; use
 *      kMenuNoIcon to remove the icon from the menu title. The
 *      supported types are kMenuIconSuiteType and kMenuIconRefType.
 *    
 *    inIcon:
 *      The icon; must be NULL if inType is kMenuNoIcon. The supported
 *      icon formats are IconSuiteRef and IconRef.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuTitleIcon(
  MenuRef   inMenu,
  UInt32    inType,
  void *    inIcon);


/*
 *  GetMenuTitleIcon()
 *  
 *  Summary:
 *    Retrieves the icon, if any, being used as the title of a menu.
 *  
 *  Discussion:
 *    This API does not increment a refcount on the returned icon. The
 *    caller should not release the icon.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose icon title to retrieve.
 *    
 *    outType:
 *      On exit, contains the type of icon being used as the title of
 *      the menu. Contains kMenuNoIcon if the menu does not have an
 *      icon title.
 *    
 *    outIcon:
 *      On exit, contains the IconSuiteRef or IconRef being used as the
 *      title of the menu, or NULL if the menu does not have an icon
 *      title. May be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuTitleIcon(
  MenuRef   inMenu,
  UInt32 *  outType,       /* can be NULL */
  void **   outIcon);      /* can be NULL */


/*
 *  InvalidateMenuSize()
 *  
 *  Summary:
 *    Invalidates the menu size so that it will be recalculated when
 *    next displayed.
 *  
 *  Discussion:
 *    The pre-Carbon technique for invalidating the menu size was to
 *    set the width and height to -1. Although this technique still
 *    works, for best compatibility it's preferable to use the
 *    InvalidateMenuSize API so that the Menu Manager has explicit
 *    notification that the menu is invalid.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose size to invalidate.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InvalidateMenuSize(MenuRef inMenu);


/*
 *  IsMenuSizeInvalid()
 *  
 *  Summary:
 *    Determines if a menu's size is invalid and should be recalculated.
 *  
 *  Discussion:
 *    The pre-Carbon technique for determining if a menu's size is
 *    invalid was to check if the width or height was -1. This
 *    technique is not always reliable on Carbon due to implementation
 *    changes in the Menu Manager. You should now use IsMenuSizeInvalid
 *    instead.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose size to examine.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuSizeInvalid(MenuRef inMenu);


/*--------------------------------------------------------------------------------------*/
/*  o.MDEF support                                                                      */
/*--------------------------------------------------------------------------------------*/
/*
 *  EraseMenuBackground()
 *  
 *  Summary:
 *    Erases a portion of a menu background in preparation for further
 *    drawing.
 *  
 *  Discussion:
 *    It is necessary to erase the menu background before calling
 *    DrawThemeMenuBackground because some themes (such as Aqua on Mac
 *    OS X) draw the menu background using the alpha channel, and if
 *    the area underneath the menu background is not erased, portions
 *    of the old image will show through the menu background.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose background to erase.
 *    
 *    inEraseRect:
 *      The bounds of the area to erase, in local coordinates to the
 *      current port.
 *    
 *    inContext:
 *      The CG context to erase. If NULL, EraseMenuBackground will
 *      create a context based on the current port.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
EraseMenuBackground(
  MenuRef        inMenu,
  const Rect *   inEraseRect,
  CGContextRef   inContext);        /* can be NULL */


/*
 *  ScrollMenuImage()
 *  
 *  Summary:
 *    Scrolls a portion of the menu image.
 *  
 *  Discussion:
 *    Menus on Mac OS X use an alpha channel, and QuickDraw does not
 *    support alpha channels. Therefore, scrolling a menu image with
 *    ScrollRect or other QuickDraw APIs does not work correctly; it
 *    results in the destruction of the alpha channel data. The
 *    ScrollMenuImage API uses CoreGraphics to move the menu image,
 *    preserving the alpha channel.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose image to scroll.
 *    
 *    inScrollRect:
 *      The bounds of the rect to scroll.
 *    
 *    inHScroll:
 *      The distance to scroll horizontally.
 *    
 *    inVScroll:
 *      The distance to scroll vertically.
 *    
 *    inContext:
 *      The CG context to erase. If NULL, ScrollMenuImage will create a
 *      context based on the current port.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.5 and later
 *    Mac OS X:         in version 10.1 and later
 */
EXTERN_API_C( OSStatus )
ScrollMenuImage(
  MenuRef        inMenu,
  const Rect *   inScrollRect,
  int            inHScroll,
  int            inVScroll,
  CGContextRef   inContext);         /* can be NULL */


/*--------------------------------------------------------------------------------------*/
/*  o Menu Item Insertion                                                               */
/*--------------------------------------------------------------------------------------*/
/*
 *  [Mac]AppendMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacAppendMenu AppendMenu
#endif
EXTERN_API( void )
MacAppendMenu(
  MenuRef            menu,
  ConstStr255Param   data)                                    ONEWORDINLINE(0xA933);


/*
 *  InsertResMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InsertResMenu(
  MenuRef   theMenu,
  ResType   theType,
  short     afterItem)                                        ONEWORDINLINE(0xA951);


/*
 *  AppendResMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
AppendResMenu(
  MenuRef   theMenu,
  ResType   theType)                                          ONEWORDINLINE(0xA94D);


/*
 *  [Mac]InsertMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacInsertMenuItem InsertMenuItem
#endif
EXTERN_API( void )
MacInsertMenuItem(
  MenuRef            theMenu,
  ConstStr255Param   itemString,
  short              afterItem)                               ONEWORDINLINE(0xA826);


/*
 *  DeleteMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DeleteMenuItem(
  MenuRef   theMenu,
  short     item)                                             ONEWORDINLINE(0xA952);


/*
 *  InsertFontResMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InsertFontResMenu(
  MenuRef   theMenu,
  short     afterItem,
  short     scriptFilter)                                     THREEWORDINLINE(0x303C, 0x0400, 0xA825);


/*
 *  InsertIntlResMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InsertIntlResMenu(
  MenuRef   theMenu,
  ResType   theType,
  short     afterItem,
  short     scriptFilter)                                     THREEWORDINLINE(0x303C, 0x0601, 0xA825);


/*
 *  AppendMenuItemText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
AppendMenuItemText(
  MenuRef            menu,
  ConstStr255Param   inString);


/*
 *  InsertMenuItemText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InsertMenuItemText(
  MenuRef            menu,
  ConstStr255Param   inString,
  MenuItemIndex      afterItem);


/*
 *  CopyMenuItems()
 *  
 *  Summary:
 *    Copies menu items from one menu to another.
 *  
 *  Parameters:
 *    
 *    inSourceMenu:
 *      The menu from which to copy items.
 *    
 *    inFirstItem:
 *      The first item to copy.
 *    
 *    inNumItems:
 *      The number of items to copy.
 *    
 *    inDestMenu:
 *      The menu to which to copy items.
 *    
 *    inInsertAfter:
 *      The menu item in the destination menu after which to insert the
 *      copied items.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CopyMenuItems(
  MenuRef         inSourceMenu,
  MenuItemIndex   inFirstItem,
  ItemCount       inNumItems,
  MenuRef         inDestMenu,
  MenuItemIndex   inInsertAfter);


/*
 *  DeleteMenuItems()
 *  
 *  Summary:
 *    Deletes multiple menu items.
 *  
 *  Discussion:
 *    This API is more efficient than calling DeleteMenuItem multiple
 *    times.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu from which to delete items.
 *    
 *    inFirstItem:
 *      The first item to delete.
 *    
 *    inNumItems:
 *      The number of items to delete.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DeleteMenuItems(
  MenuRef         inMenu,
  MenuItemIndex   inFirstItem,
  ItemCount       inNumItems);


/*
 *  AppendMenuItemTextWithCFString()
 *  
 *  Summary:
 *    Appends a new menu item with text from a CFString.
 *  
 *  Discussion:
 *    The Menu Manager will either make its own copy or just increment
 *    the refcount of the CFString before returning from
 *    AppendMenuItemWithTextCFString, depending on whether the string
 *    is mutable or immutable. If the string is mutable, modifying the
 *    string after calling AppendMenuItemTextWithCFString will have no
 *    effect on the menu item's actual text. The caller may release the
 *    string after calling AppendMenuItemTextWithCFString.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu to which to append the new item.
 *    
 *    inString:
 *      The text of the new item.
 *    
 *    inAttributes:
 *      The attributes of the new item.
 *    
 *    inCommandID:
 *      The command ID of the new item.
 *    
 *    outNewItem:
 *      On exit, the index of the new item. May be NULL if the caller
 *      does not need this information.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
AppendMenuItemTextWithCFString(
  MenuRef              inMenu,
  CFStringRef          inString,
  MenuItemAttributes   inAttributes,
  MenuCommand          inCommandID,
  MenuItemIndex *      outNewItem);        /* can be NULL */


/*
 *  InsertMenuItemTextWithCFString()
 *  
 *  Summary:
 *    Inserts a new menu item with text from a CFString.
 *  
 *  Discussion:
 *    The Menu Manager will either make its own copy or just increment
 *    the refcount of the CFString before returning from
 *    InsertMenuItemWithCFString, depending on whether the string is
 *    mutable or immutable. If the string is mutable, modifying the
 *    string after calling InsertMenuItemWithCFString will have no
 *    effect on the menu item's actual text. The caller may release the
 *    string after calling InsertMenuItemWithCFString.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to insert the new item.
 *    
 *    inString:
 *      The text of the new item.
 *    
 *    inAfterItem:
 *      The item after which to insert the new item.
 *    
 *    inAttributes:
 *      The attributes of the new item.
 *    
 *    inCommandID:
 *      The command ID of the new item.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InsertMenuItemTextWithCFString(
  MenuRef              inMenu,
  CFStringRef          inString,
  MenuItemIndex        inAfterItem,
  MenuItemAttributes   inAttributes,
  MenuCommand          inCommandID);


/*--------------------------------------------------------------------------------------*/
/*  o Menu Events                                                                       */
/*--------------------------------------------------------------------------------------*/
/*
 *  MenuKey()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
MenuKey(CharParameter ch)                                     ONEWORDINLINE(0xA93E);


/*
 *  MenuSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
MenuSelect(Point startPt)                                     ONEWORDINLINE(0xA93D);


/*
 *  PopUpMenuSelect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
PopUpMenuSelect(
  MenuRef   menu,
  short     top,
  short     left,
  short     popUpItem)                                        ONEWORDINLINE(0xA80B);


/*
 *  MenuChoice()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( long )
MenuChoice(void)                                              ONEWORDINLINE(0xAA66);


/*
 *  MenuEvent()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( UInt32 )
MenuEvent(const EventRecord * inEvent)                        THREEWORDINLINE(0x303C, 0x020C, 0xA825);


/*
 *  IsMenuKeyEvent()
 *  
 *  Summary:
 *    Determines if an event corresponds to a menu command key.
 *  
 *  Discussion:
 *    By default, IsMenuKeyEvent searches the menus in the current menu
 *    bar and hilites the menu title of the menu containing the
 *    selected item.
 *  
 *  Parameters:
 *    
 *    inStartMenu:
 *      IsMenuKeyEvent searches for matching menu items in this menu
 *      and all of its submenus. May be NULL to search the current menu
 *      bar contents.
 *    
 *    inEvent:
 *      The event to match against. Non-keyboard events are ignored.
 *    
 *    inOptions:
 *      Options controlling how to search. Pass kNilOptions for the
 *      default behavior.
 *    
 *    outMenu:
 *      On exit, the menu containing the matching item. May be NULL.
 *    
 *    outMenuItem:
 *      On exit, the menu item that matched. May be NULL.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuKeyEvent(
  MenuRef            inStartMenu,
  EventRef           inEvent,
  MenuEventOptions   inOptions,
  MenuRef *          outMenu,           /* can be NULL */
  MenuItemIndex *    outMenuItem);      /* can be NULL */


/*
 *  InvalidateMenuEnabling()
 *  
 *  Summary:
 *    Causes the menu enable state to be recalculated at the next
 *    convenient opportunity.
 *  
 *  Discussion:
 *    It is common for state changes in an application (for example,
 *    selection of text) to cause a change in the enabling of items in
 *    the application's menu (for example, the Copy menu item might
 *    become enabled). In a Carbon-event-savvy application, menu items
 *    are enabled or disabled in response to an
 *    kEventCommandUpdateStatus event; however, this event is normally
 *    only sent before a command key press or a click in the menubar.
 *    You can request an explicit recalculation of a menu's enable
 *    state with the InvalidateMenuEnabling API. The Carbon Event
 *    Manager will automatically invalidate the enable state of all
 *    top-level menus when a user event is dispatched, the user focus
 *    changes, or the active window changes, so in many cases you will
 *    not need to explicitly invalidate the menu enabling state.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      A menu to re-enable, or NULL if all menus in the root menu
 *      should be re-enabled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.3 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InvalidateMenuEnabling(MenuRef inMenu);



/*--------------------------------------------------------------------------------------*/
/*  o Menu Bar                                                                          */
/*--------------------------------------------------------------------------------------*/
/*
 *  GetMBarHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( short )
GetMBarHeight(void)                                           TWOWORDINLINE(0x3EB8, 0x0BAA);


/*
 *  [Mac]DrawMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacDrawMenuBar DrawMenuBar
#endif
EXTERN_API( void )
MacDrawMenuBar(void)                                          ONEWORDINLINE(0xA937);


/*
 *  InvalMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
InvalMenuBar(void)                                            ONEWORDINLINE(0xA81D);


/*
 *  IsMenuBarInvalid()
 *  
 *  Summary:
 *    Determines if the menubar is invalid and should be redrawn.
 *  
 *  Parameters:
 *    
 *    rootMenu:
 *      The root menu for the menubar to be examined. Pass NULL to
 *      check the state of the current menubar.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuBarInvalid(MenuRef rootMenu);


/*
 *  HiliteMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HiliteMenu(MenuID menuID)                                     ONEWORDINLINE(0xA938);


/*
 *  GetNewMBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MenuBarHandle )
GetNewMBar(short menuBarID)                                   ONEWORDINLINE(0xA9C0);


/*
 *  GetMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MenuBarHandle )
GetMenuBar(void)                                              ONEWORDINLINE(0xA93B);


/*
 *  SetMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMenuBar(MenuBarHandle mbar)                                ONEWORDINLINE(0xA93C);


/*
 *  DuplicateMenuBar()
 *  
 *  Summary:
 *    Duplicates a menubar handle.
 *  
 *  Discussion:
 *    This API should be used in Carbon applications when duplicating a
 *    handle returned from GetMenuBar or GetNewMBar. You should not use
 *    Memory Manager APIs (HandToHand, NewHandle, etc) to duplicate
 *    such a handle. This is necessary in Carbon so that the refcounts
 *    of the menus in the menubar handle can be incremented when the
 *    handle is duplicated.
 *  
 *  Parameters:
 *    
 *    inMbar:
 *      The menubar handle to duplicate.
 *    
 *    outMbar:
 *      On exit, contains the new menubar handle.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DuplicateMenuBar(
  MenuBarHandle    inMbar,
  MenuBarHandle *  outMbar);


/*
 *  DisposeMenuBar()
 *  
 *  Summary:
 *    Releases a menubar handle.
 *  
 *  Discussion:
 *    This API should be used in Carbon applications when releasing a
 *    handle returned from GetMenuBar or GetNewMBar. You should not use
 *    DisposeHandle to release such a handle. This is necessary in
 *    Carbon so that the refcounts of the menus in the menubar handle
 *    can be decremented when the handle is released.
 *  
 *  Parameters:
 *    
 *    inMbar:
 *      The menubar handle to release.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0.2 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
DisposeMenuBar(MenuBarHandle inMbar);


/*
 *  GetMenuHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MenuRef )
GetMenuHandle(MenuID menuID)                                  ONEWORDINLINE(0xA949);


#define GetMenuRef GetMenuHandle
/*
 *  [Mac]InsertMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacInsertMenu InsertMenu
#endif
EXTERN_API( void )
MacInsertMenu(
  MenuRef   theMenu,
  MenuID    beforeID)                                         ONEWORDINLINE(0xA935);


/*
 *  [Mac]DeleteMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacDeleteMenu DeleteMenu
#endif
EXTERN_API( void )
MacDeleteMenu(MenuID menuID)                                  ONEWORDINLINE(0xA936);


/*
 *  ClearMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ClearMenuBar(void)                                            ONEWORDINLINE(0xA934);


#if CALL_NOT_IN_CARBON
/*
 *  SetMenuFlash()
 *  
 *  Summary:
 *    Renamed to SetMenuFlashCount in Carbon
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SetMenuFlash(short count)                                     ONEWORDINLINE(0xA94A);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  SetMenuFlashCount()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.3 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMenuFlashCount(short count)                                ONEWORDINLINE(0xA94A);
#if CALL_NOT_IN_CARBON && !TARGET_CPU_68K
  #ifdef __cplusplus
    inline DEFINE_API(void) SetMenuFlashCount(short count) { SetMenuFlash(count); }
  #else
    #define SetMenuFlashCount(count) (SetMenuFlash(count))
  #endif
#endif



/*
 *  FlashMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
FlashMenuBar(MenuID menuID)                                   ONEWORDINLINE(0xA94C);


/* These are obsolete because Carbon does not support desk accessories.*/
#if CALL_NOT_IN_CARBON
/*
 *  SystemEdit()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( Boolean )
SystemEdit(short editCmd)                                     ONEWORDINLINE(0xA9C2);


/*
 *  SystemMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
SystemMenu(long menuResult)                                   ONEWORDINLINE(0xA9B5);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  IsMenuBarVisible()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuBarVisible(void);


/*
 *  ShowMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
ShowMenuBar(void);


/*
 *  HideMenuBar()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
HideMenuBar(void);


/*
 *  AcquireRootMenu()
 *  
 *  Summary:
 *    Get the menu whose contents are displayed in the menubar.
 *  
 *  Discussion:
 *    The refcount of the root menu is incremented by this API. The
 *    caller should release a refcount with ReleaseMenu when it's done
 *    with the menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MenuRef )
AcquireRootMenu(void);


/*
 *  SetRootMenu()
 *  
 *  Summary:
 *    Sets the menu whose contents are displayed in the menubar.
 *  
 *  Discussion:
 *    The refcount of the root menu is incremented by this API. The
 *    caller may release the menu after calling SetRootMenu.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The new root menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetRootMenu(MenuRef inMenu);


/*--------------------------------------------------------------------------------------*/
/*  o Menu Item Accessors                                                               */
/*--------------------------------------------------------------------------------------*/
#if CALL_NOT_IN_CARBON
/*
 *  CheckItem()
 *  
 *  Summary:
 *    Renamed to CheckMenuItem in Carbon
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
CheckItem(
  MenuRef   theMenu,
  short     item,
  Boolean   checked)                                          ONEWORDINLINE(0xA945);



#endif  /* CALL_NOT_IN_CARBON */

/*
 *  [Mac]CheckMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.3 and later or as macro/inline
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacCheckMenuItem CheckMenuItem
#endif
EXTERN_API( void )
MacCheckMenuItem(
  MenuRef   theMenu,
  short     item,
  Boolean   checked)                                          ONEWORDINLINE(0xA945);
#if CALL_NOT_IN_CARBON && !TARGET_CPU_68K
  #ifdef __cplusplus
    inline DEFINE_API(void) MacCheckMenuItem(MenuRef theMenu, short item, Boolean checked) { CheckItem(theMenu, item, checked); }
  #else
    #if TARGET_OS_MAC
      #define CheckMenuItem(theMenu, item, checked) (CheckItem(theMenu, item, checked))
    #else
      #define MacCheckMenuItem(theMenu, item, checked) (CheckItem(theMenu, item, checked))
    #endif
#endif
#endif



/*
 *  SetMenuItemText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMenuItemText(
  MenuRef            theMenu,
  short              item,
  ConstStr255Param   itemString)                              ONEWORDINLINE(0xA947);


/*
 *  GetMenuItemText()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetMenuItemText(
  MenuRef   theMenu,
  short     item,
  Str255    itemString)                                       ONEWORDINLINE(0xA946);


/*
 *  SetItemMark()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetItemMark(
  MenuRef         theMenu,
  short           item,
  CharParameter   markChar)                                   ONEWORDINLINE(0xA944);


/*
 *  GetItemMark()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetItemMark(
  MenuRef          theMenu,
  short            item,
  CharParameter *  markChar)                                  ONEWORDINLINE(0xA943);


/*
 *  SetItemCmd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetItemCmd(
  MenuRef         theMenu,
  short           item,
  CharParameter   cmdChar)                                    ONEWORDINLINE(0xA84F);


/*
 *  GetItemCmd()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetItemCmd(
  MenuRef          theMenu,
  short            item,
  CharParameter *  cmdChar)                                   ONEWORDINLINE(0xA84E);


/*
 *  SetItemIcon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetItemIcon(
  MenuRef   theMenu,
  short     item,
  short     iconIndex)                                        ONEWORDINLINE(0xA940);


/* icon is returned in high byte of 16-bit iconIndex */
/*
 *  GetItemIcon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetItemIcon(
  MenuRef   theMenu,
  short     item,
  short *   iconIndex)                                        ONEWORDINLINE(0xA93F);


/*
 *  SetItemStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetItemStyle(
  MenuRef          theMenu,
  short            item,
  StyleParameter   chStyle)                                   ONEWORDINLINE(0xA942);


/*
 *  GetItemStyle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
GetItemStyle(
  MenuRef   theMenu,
  short     item,
  Style *   chStyle);


/* These APIs are not supported in Carbon. Please use EnableMenuItem and */
/* DisableMenuItem (available back through Mac OS 8.5) instead.          */
#if CALL_NOT_IN_CARBON
/*
 *  DisableItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
DisableItem(
  MenuRef   theMenu,
  short     item)                                             ONEWORDINLINE(0xA93A);


/*
 *  EnableItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( void )
EnableItem(
  MenuRef   theMenu,
  short     item)                                             ONEWORDINLINE(0xA939);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  SetMenuItemCommandID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemCommandID(
  MenuRef       inMenu,
  SInt16        inItem,
  MenuCommand   inCommandID)                                  THREEWORDINLINE(0x303C, 0x0502, 0xA825);


/*
 *  GetMenuItemCommandID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemCommandID(
  MenuRef        inMenu,
  SInt16         inItem,
  MenuCommand *  outCommandID)                                THREEWORDINLINE(0x303C, 0x0503, 0xA825);


/*
 *  SetMenuItemModifiers()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemModifiers(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt8     inModifiers)                                      THREEWORDINLINE(0x303C, 0x0404, 0xA825);


/*
 *  GetMenuItemModifiers()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemModifiers(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt8 *   outModifiers)                                     THREEWORDINLINE(0x303C, 0x0505, 0xA825);


/*
 *  SetMenuItemIconHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemIconHandle(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt8     inIconType,
  Handle    inIconHandle)                                     THREEWORDINLINE(0x303C, 0x0606, 0xA825);


/*
 *  GetMenuItemIconHandle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemIconHandle(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt8 *   outIconType,
  Handle *  outIconHandle)                                    THREEWORDINLINE(0x303C, 0x0707, 0xA825);


/*
 *  SetMenuItemTextEncoding()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemTextEncoding(
  MenuRef        inMenu,
  SInt16         inItem,
  TextEncoding   inScriptID)                                  THREEWORDINLINE(0x303C, 0x0408, 0xA825);


/*
 *  GetMenuItemTextEncoding()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemTextEncoding(
  MenuRef         inMenu,
  SInt16          inItem,
  TextEncoding *  outScriptID)                                THREEWORDINLINE(0x303C, 0x0509, 0xA825);


/*
 *  SetMenuItemHierarchicalID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemHierarchicalID(
  MenuRef   inMenu,
  SInt16    inItem,
  MenuID    inHierID)                                         THREEWORDINLINE(0x303C, 0x040D, 0xA825);


/*
 *  GetMenuItemHierarchicalID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemHierarchicalID(
  MenuRef   inMenu,
  SInt16    inItem,
  MenuID *  outHierID)                                        THREEWORDINLINE(0x303C, 0x050E, 0xA825);


/*
 *  SetMenuItemFontID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemFontID(
  MenuRef   inMenu,
  SInt16    inItem,
  SInt16    inFontID)                                         THREEWORDINLINE(0x303C, 0x040F, 0xA825);


/*
 *  GetMenuItemFontID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemFontID(
  MenuRef   inMenu,
  SInt16    inItem,
  SInt16 *  outFontID)                                        THREEWORDINLINE(0x303C, 0x0510, 0xA825);


/*
 *  SetMenuItemRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemRefCon(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt32    inRefCon)                                         THREEWORDINLINE(0x303C, 0x050A, 0xA825);


/*
 *  GetMenuItemRefCon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemRefCon(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt32 *  outRefCon)                                        THREEWORDINLINE(0x303C, 0x050B, 0xA825);


/* Please use the menu item property APIs in Carbon.*/
#if CALL_NOT_IN_CARBON
/*
 *  SetMenuItemRefCon2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
SetMenuItemRefCon2(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt32    inRefCon2)                                        THREEWORDINLINE(0x303C, 0x0511, 0xA825);


/*
 *  GetMenuItemRefCon2()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API( OSErr )
GetMenuItemRefCon2(
  MenuRef   inMenu,
  SInt16    inItem,
  UInt32 *  outRefCon2)                                       THREEWORDINLINE(0x303C, 0x0512, 0xA825);


#endif  /* CALL_NOT_IN_CARBON */

/*
 *  SetMenuItemKeyGlyph()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
SetMenuItemKeyGlyph(
  MenuRef   inMenu,
  SInt16    inItem,
  SInt16    inGlyph)                                          THREEWORDINLINE(0x303C, 0x0513, 0xA825);


/*
 *  GetMenuItemKeyGlyph()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in AppearanceLib 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSErr )
GetMenuItemKeyGlyph(
  MenuRef   inMenu,
  SInt16    inItem,
  SInt16 *  outGlyph)                                         THREEWORDINLINE(0x303C, 0x0514, 0xA825);


/* Routines available in Mac OS 8.5 and later (supporting enabling/disabling of > 31 items)*/

/*
 *  [Mac]EnableMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
#if TARGET_OS_MAC
    #define MacEnableMenuItem EnableMenuItem
#endif
EXTERN_API( void )
MacEnableMenuItem(
  MenuRef         theMenu,
  MenuItemIndex   item);


/*
 *  DisableMenuItem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisableMenuItem(
  MenuRef         theMenu,
  MenuItemIndex   item);


/*
 *  IsMenuItemEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuItemEnabled(
  MenuRef         menu,
  MenuItemIndex   item);


/*
 *  EnableMenuItemIcon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EnableMenuItemIcon(
  MenuRef         theMenu,
  MenuItemIndex   item)                                       THREEWORDINLINE(0x303C, 0x0019, 0xA825);


/*
 *  DisableMenuItemIcon()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisableMenuItemIcon(
  MenuRef         theMenu,
  MenuItemIndex   item)                                       THREEWORDINLINE(0x303C, 0x0020, 0xA825);


/*
 *  IsMenuItemIconEnabled()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuItemIconEnabled(
  MenuRef         menu,
  MenuItemIndex   item)                                       THREEWORDINLINE(0x303C, 0x0018, 0xA825);


/*
 *  SetMenuItemHierarchicalMenu()
 *  
 *  Summary:
 *    Attaches a submenu to a menu item.
 *  
 *  Discussion:
 *    Using SetMenuItemHierarchicalMenu, it is possible to directly
 *    specify the submenu for a menu item without specifying its menu
 *    ID. It is not necessary to insert the submenu into the
 *    hierarchical portion of the menubar, and it is not necessary for
 *    the submenu to have a unique menu ID; it is recommended that you
 *    use 0 as the menu ID for the submenu, and identify selections
 *    from the menu by command ID. The Menu Manager will increment the
 *    refcount of the submenu that you specify, and the submenu's
 *    refcount will be decremented automatically when the parent menu
 *    item is deleted or the parent menu is disposed.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The parent menu.
 *    
 *    inItem:
 *      The parent item.
 *    
 *    inHierMenu:
 *      The submenu. You may pass NULL to remove any existing submenu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuItemHierarchicalMenu(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  MenuRef         inHierMenu);      /* can be NULL */


/*
 *  GetMenuItemHierarchicalMenu()
 *  
 *  Summary:
 *    Returns the submenu attached to a menu item.
 *  
 *  Discussion:
 *    GetMenuItemHierarchicalMenu will return the submenu attached to a
 *    menu item regardless of how the submenu was specified. If the
 *    submenu was specified by menu ID (using SetItemCmd or
 *    SetMenuItemHierarchicalID), GetMenuItemHierarchicalMenu will
 *    return the currently installed menu with that ID, if any. The
 *    only case where GetMenuItemHierarchicalMenu will fail to return
 *    the item's submenu is when the submenu is specified by menu ID,
 *    but the submenu is not currently inserted in the menu bar.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The parent menu.
 *    
 *    inItem:
 *      The parent item.
 *    
 *    outHierMenu:
 *      On exit, the item's submenu, or NULL if it does not have one.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuItemHierarchicalMenu(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  MenuRef *       outHierMenu);


/*
 *  CopyMenuItemTextAsCFString()
 *  
 *  Summary:
 *    Returns a CFString containing the text of a menu item.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu containing the item.
 *    
 *    inItem:
 *      The item whose text to return.
 *    
 *    outString:
 *      On exit, a CFString containing the item's text. This string
 *      must be released by the caller.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CopyMenuItemTextAsCFString(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  CFStringRef *   outString);


/*
 *  SetMenuItemTextWithCFString()
 *  
 *  Summary:
 *    Sets the text of a menu item to the text contained in a CFString.
 *  
 *  Discussion:
 *    The Menu Manager will either make its own copy or just increment
 *    the refcount of the CFString before returning from
 *    SetMenuItemTextWithCFString, depending on whether the string is
 *    mutable or immutable. If the string is mutable, modifying the
 *    string after calling SetMenuItemTextWithCFString will have no
 *    effect on the menu item's actual text. The caller may release the
 *    string after calling SetMenuItemTextWithCFString.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu containing the item.
 *    
 *    inItem:
 *      The item whose text to return.
 *    
 *    inString:
 *      The string containing the new menu item text.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuItemTextWithCFString(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  CFStringRef     inString);


/*
 *  GetMenuItemIndent()
 *  
 *  Summary:
 *    Gets the indent level of a menu item.
 *  
 *  Discussion:
 *    The indent level of an item is an amount of extra space added to
 *    the left of the item's icon or checkmark. The level is simply a
 *    number, starting at zero, which the Menu Manager multiplies by a
 *    constant to get the indent in pixels. The default indent level is
 *    zero.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu containing the item.
 *    
 *    inItem:
 *      The item whose indent to retrieve.
 *    
 *    outIndent:
 *      On exit, the indent level of the item.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuItemIndent(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  UInt32 *        outIndent);


/*
 *  SetMenuItemIndent()
 *  
 *  Summary:
 *    Sets the indent level of a menu item.
 *  
 *  Discussion:
 *    The indent level of an item is an amount of extra space added to
 *    the left of the item's icon or checkmark. The level is simply a
 *    number, starting at zero, which the Menu Manager multiplies by a
 *    constant to get the indent in pixels. The default indent level is
 *    zero.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu containing the item.
 *    
 *    inItem:
 *      The item whose indent to set.
 *    
 *    inIndent:
 *      The new indent level of the item.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuItemIndent(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  UInt32          inIndent);


/*
 *  GetMenuItemCommandKey()
 *  
 *  Summary:
 *    Gets the keyboard equivalent of a menu item.
 *  
 *  Discussion:
 *    A menu item's keyboard equivalent may be either a character code
 *    or a virtual keycode. An item's character code and virtual
 *    keycode are stored separately and may contain different values,
 *    but only one is used by the Menu Manager at any given time. When
 *    requesting a menu item's virtual keycode equivalent, you should
 *    first check that the item is using a virtual keycode by testing
 *    the kMenuItemAttrUseVirtualKey attribute for that item. If this
 *    attribute is not set, the item's virtual keycode is ignored by
 *    the Menu Manager. Note that zero is a valid virtual keycode, so
 *    you cannot test the returned keycode against zero to determine if
 *    the item is using a virtual keycode equivalent. You must test the
 *    kMenuItemAttrUseVirtualKey attribute.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu containing the item.
 *    
 *    inItem:
 *      The item whose keyboard equivalent to retrieve.
 *    
 *    inGetVirtualKey:
 *      Indicates whether to retrieve the item's character code or
 *      virtual keycode equivalent.
 *    
 *    outKey:
 *      On exit, the keyboard equivalent of the item.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuItemCommandKey(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  Boolean         inGetVirtualKey,
  UInt16 *        outKey);


/*
 *  SetMenuItemCommandKey()
 *  
 *  Summary:
 *    Sets the keyboard equivalent of a menu item.
 *  
 *  Discussion:
 *    A menu item's keyboard equivalent may be either a character code
 *    or a virtual keycode. The character code is always used to draw
 *    the item's keyboard equivalent in the menu, but either may be
 *    used for keyboard equivalent matching by MenuEvent and
 *    IsMenuKeyEvent, depending on whether the
 *    kMenuItemAttrUseVirtualKey item attribute is set. If
 *    SetMenuItemCommandKey is used to set the virtual keycode
 *    equivalent for a menu item, it also automatically sets the
 *    kMenuItemAttrUseVirtualKey item attribute. To make the menu item
 *    stop using the virtual keycode equivalent and use the character
 *    code equivalent instead, use ChangeMenuItemAttributes to clear
 *    the kMenuItemAttrUseVirtualKey item attribute.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu containing the item.
 *    
 *    inItem:
 *      The item whose keyboard equivalent to set.
 *    
 *    inSetVirtualKey:
 *      Indicates whether to set the item's character code or virtual
 *      keycode equivalent.
 *    
 *    inKey:
 *      The item's new character code or virtual keycode equivalent.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuItemCommandKey(
  MenuRef         inMenu,
  MenuItemIndex   inItem,
  Boolean         inSetVirtualKey,
  UInt16          inKey);


/*--------------------------------------------------------------------------------------*/
/*  o Menu Item Color Tables                                                            */
/*                                                                                      */
/*  Menu color manipulation is considered deprecated with the advent of the Appearance  */
/*  Manager.  Avoid using these routines if possible                                    */
/*--------------------------------------------------------------------------------------*/
/*
 *  DeleteMCEntries()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DeleteMCEntries(
  MenuID   menuID,
  short    menuItem)                                          ONEWORDINLINE(0xAA60);


/*
 *  GetMCInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MCTableHandle )
GetMCInfo(void)                                               ONEWORDINLINE(0xAA61);


/*
 *  SetMCInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMCInfo(MCTableHandle menuCTbl)                             ONEWORDINLINE(0xAA62);


/*
 *  DisposeMCInfo()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisposeMCInfo(MCTableHandle menuCTbl)                         ONEWORDINLINE(0xAA63);


/*
 *  GetMCEntry()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MCEntryPtr )
GetMCEntry(
  MenuID   menuID,
  short    menuItem)                                          ONEWORDINLINE(0xAA64);


/*
 *  SetMCEntries()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMCEntries(
  short        numEntries,
  MCTablePtr   menuCEntries)                                  ONEWORDINLINE(0xAA65);



/*--------------------------------------------------------------------------------------*/
/* o Properties  (Mac OS 8.5 and later)                                                 */
/*                                                                                      */
/* With the following property APIs, you can attach any piece of data you'd like to a   */
/* menu or menu item. Passing zero for the item number parameter indicates you'd like   */
/* to attach the data to the menu itself, and not to any specific menu item.            */
/*--------------------------------------------------------------------------------------*/
enum {
  kMenuPropertyPersistent       = 0x00000001 /* whether this property gets saved when flattening the menu*/
};

/*
 *  GetMenuItemProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuItemProperty(
  MenuRef         menu,
  MenuItemIndex   item,
  OSType          propertyCreator,
  OSType          propertyTag,
  UInt32          bufferSize,
  UInt32 *        actualSize,
  void *          propertyBuffer);


/*
 *  GetMenuItemPropertySize()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuItemPropertySize(
  MenuRef         menu,
  MenuItemIndex   item,
  OSType          propertyCreator,
  OSType          propertyTag,
  UInt32 *        size);


/*
 *  SetMenuItemProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuItemProperty(
  MenuRef         menu,
  MenuItemIndex   item,
  OSType          propertyCreator,
  OSType          propertyTag,
  UInt32          propertySize,
  const void *    propertyData);


/*
 *  RemoveMenuItemProperty()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in MenusLib 8.5 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RemoveMenuItemProperty(
  MenuRef         menu,
  MenuItemIndex   item,
  OSType          propertyCreator,
  OSType          propertyTag);


/*
 *  GetMenuItemPropertyAttributes()
 *  
 *  Summary:
 *    Gets the attributes of a menu item property.
 *  
 *  Parameters:
 *    
 *    menu:
 *      The menu.
 *    
 *    item:
 *      The menu item.
 *    
 *    propertyCreator:
 *      The creator code of the property.
 *    
 *    propertyTag:
 *      The property tag.
 *    
 *    attributes:
 *      On exit, contains the attributes of the property.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuItemPropertyAttributes(
  MenuRef         menu,
  MenuItemIndex   item,
  OSType          propertyCreator,
  OSType          propertyTag,
  UInt32 *        attributes);


/*
 *  ChangeMenuItemPropertyAttributes()
 *  
 *  Summary:
 *    Changes the attributes of a menu item property.
 *  
 *  Parameters:
 *    
 *    menu:
 *      The menu.
 *    
 *    item:
 *      The menu item.
 *    
 *    propertyCreator:
 *      The creator code of the property.
 *    
 *    propertyTag:
 *      The property tag.
 *    
 *    attributesToSet:
 *      The attributes to add to the menu item property.
 *    
 *    attributesToClear:
 *      The attributes to remove from the menu item property.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ChangeMenuItemPropertyAttributes(
  MenuRef         menu,
  MenuItemIndex   item,
  OSType          propertyCreator,
  OSType          propertyTag,
  UInt32          attributesToSet,
  UInt32          attributesToClear);


/*--------------------------------------------------------------------------------------*/
/*  o Attributes (Carbon and later)                                                     */
/*                                                                                      */
/*  Each menu and menu item has attribute flags.                                        */
/*--------------------------------------------------------------------------------------*/
/*
 *  GetMenuAttributes()
 *  
 *  Summary:
 *    Gets the attributes of a menu.
 *  
 *  Parameters:
 *    
 *    menu:
 *      The menu.
 *    
 *    outAttributes:
 *      On exit, contains the attributes of the menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuAttributes(
  MenuRef           menu,
  MenuAttributes *  outAttributes);


/*
 *  ChangeMenuAttributes()
 *  
 *  Summary:
 *    Changes the attributes of a menu.
 *  
 *  Parameters:
 *    
 *    menu:
 *      The menu.
 *    
 *    setTheseAttributes:
 *      The attributes to add to the menu.
 *    
 *    clearTheseAttributes:
 *      The attributes to remove from the menu.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ChangeMenuAttributes(
  MenuRef          menu,
  MenuAttributes   setTheseAttributes,
  MenuAttributes   clearTheseAttributes);


/*
 *  GetMenuItemAttributes()
 *  
 *  Summary:
 *    Gets the attributes of a menu item.
 *  
 *  Parameters:
 *    
 *    menu:
 *      The menu.
 *    
 *    item:
 *      The menu item.
 *    
 *    outAttributes:
 *      On exit, contains the attributes of the menu item.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuItemAttributes(
  MenuRef               menu,
  MenuItemIndex         item,
  MenuItemAttributes *  outAttributes);


/*
 *  ChangeMenuItemAttributes()
 *  
 *  Summary:
 *    Changes the attributes of a menu item.
 *  
 *  Parameters:
 *    
 *    menu:
 *      The menu.
 *    
 *    item:
 *      The menu item.
 *    
 *    setTheseAttributes:
 *      The attributes to add to the menu item.
 *    
 *    clearTheseAttributes:
 *      The attributes to remove from the menu item.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ChangeMenuItemAttributes(
  MenuRef              menu,
  MenuItemIndex        item,
  MenuItemAttributes   setTheseAttributes,
  MenuItemAttributes   clearTheseAttributes);


/*--------------------------------------------------------------------------------------*/
/*  o Mass menu item enabling and disabling (Carbon and later)                          */
/*                                                                                      */
/*  Useful when rewriting code that modifies the enableFlags field directly.            */
/*--------------------------------------------------------------------------------------*/
/*
 *  DisableAllMenuItems()
 *  
 *  Summary:
 *    Disables all items in a menu.
 *  
 *  Discussion:
 *    This API is equivalent to pre-Carbon code that masked the
 *    enableFlags field of the MenuInfo with 0x01. It disables all
 *    items (including items past item 31) but does not affect the
 *    state of the menu title.
 *  
 *  Parameters:
 *    
 *    theMenu:
 *      The menu whose items to disable.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisableAllMenuItems(MenuRef theMenu);


/*
 *  EnableAllMenuItems()
 *  
 *  Summary:
 *    Enables all items in a menu.
 *  
 *  Discussion:
 *    This API is equivalent to pre-Carbon code that or'd the
 *    enableFlags field of the MenuInfo with 0xFFFFFFFE. It enables all
 *    items (including items past item 31) but does not affect the
 *    state of the menu title.
 *  
 *  Parameters:
 *    
 *    theMenu:
 *      The menu whose items to enable.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EnableAllMenuItems(MenuRef theMenu);


/*
 *  MenuHasEnabledItems()
 *  
 *  Summary:
 *    Determines if any items in a menu are enabled.
 *  
 *  Discussion:
 *    This API is equivalent to pre-Carbon code that compared the
 *    enableFlags field of the MenuInfo with 0. It checks the enable
 *    state of all items to see if any are enabled, but ignores the
 *    state of the menu title. It will return true even if the menu
 *    title is disabled.
 *  
 *  Parameters:
 *    
 *    theMenu:
 *      The menu whose items to examine.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
MenuHasEnabledItems(MenuRef theMenu);


/*--------------------------------------------------------------------------------------*/
/*  o Menu tracking status (Carbon and later)                                           */
/*                                                                                      */
/*  Get info about the selected menu item during menu tracking. Replaces direct access  */
/*  to low-mem globals that previously held this info.                                  */
/*--------------------------------------------------------------------------------------*/
/*
 *  GetMenuTrackingData()
 *  
 *  Summary:
 *    Gets information about the menu currently selected by the user.
 *  
 *  Discussion:
 *    This API replaces direct access to the low-memory globals
 *    TopMenuItem, AtMenuBottom, MenuDisable, and mbSaveLoc. It is only
 *    valid to call this API while menu tracking is occurring. This API
 *    will most commonly be used by custom MDEFs.
 *  
 *  Parameters:
 *    
 *    theMenu:
 *      The menu about which to get tracking information. May be NULL
 *      to get information about the menu that the user is currently
 *      selecting. If the menu is not currently open, menuNotFoundErr
 *      is returned.
 *    
 *    outData:
 *      On exit, contains tracking data about the menu. On CarbonLib,
 *      the itemRect field is not supported and is always set to an
 *      empty rect.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuTrackingData(
  MenuRef             theMenu,
  MenuTrackingData *  outData);


/*
 *  GetMenuType()
 *  
 *  Summary:
 *    Gets the display type (pulldown, hierarchical, or popup) of a
 *    menu.
 *  
 *  Discussion:
 *    This API may only be called when the menu is displayed. If the
 *    menu is not currently open, an error is returned. The display
 *    type of a menu may vary from one menu tracking session to
 *    another; for example, the same menu might be displayed as a
 *    pulldown menu and as a popup menu.
 *  
 *  Parameters:
 *    
 *    theMenu:
 *      The menu whose type to get.
 *    
 *    outType:
 *      On exit, the type of the menu. The returned value will be one
 *      of the ThemeMenuType constants: kThemeMenuTypePullDown, PopUp,
 *      or Hierarchical. The kThemeMenuTypeInactive bit will never be
 *      set.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuType(
  MenuRef   theMenu,
  UInt16 *  outType);


/*--------------------------------------------------------------------------------------*/
/*  o.Universal command ID access (Carbon and later)                                    */
/*                                                                                      */
/*  These APIs allow you to operate on menu items strictly by command ID, with no       */
/*  knowledge of a menu item's index.                                                   */
/*--------------------------------------------------------------------------------------*/
/*
 *  CountMenuItemsWithCommandID()
 *  
 *  Summary:
 *    Counts the menu items with a specified command ID.
 *  
 *  Discussion:
 *    In CarbonLib 1.0.x and 1.1, this API always returns zero or one;
 *    it stops after finding the first menu item with the specified
 *    command ID. In CarbonLib 1.2 and Mac OS X 10.0 and later, it
 *    counts all menu items with the specified command ID. In Mac OS X
 *    10.0 and CarbonLib 1.0 through 1.4, this API only searches
 *    top-level menus (menus visible in the menubar) and submenus of
 *    top-level menus. It does not search hierarchical menus that are
 *    inserted in the menubar but are not submenus of a top-level menus
 *    (for example, it does not search menus that are inserted for use
 *    in a popup menu control). In Mac OS X 10.1 and CarbonLib 1.5 and
 *    later, this API also searches inserted hierarchical menus.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for items with the
 *      specified command ID. Pass NULL to hegin searching with the
 *      root menu. The search will descend into all submenus of this
 *      menu.
 *    
 *    inCommandID:
 *      The command ID for which to search.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( ItemCount )
CountMenuItemsWithCommandID(
  MenuRef       inMenu,
  MenuCommand   inCommandID);


/*
 *  GetIndMenuItemWithCommandID()
 *  
 *  Summary:
 *    Finds a menu item with a specified command ID.
 *  
 *  Discussion:
 *    This API searches the specified menu and its submenus for the
 *    n'th menu item with the specified command ID. In CarbonLib 1.0.x
 *    and 1.1, only the first menu item is returned. In CarbonLib 1.2
 *    and Mac OS X 10.0 and later, this API iterates over all menu
 *    items with the specified command ID. In Mac OS X 10.0 and
 *    CarbonLib 1.0 through 1.4, this API only searches top-level menus
 *    (menus visible in the menubar) and submenus of top-level menus.
 *    It does not search hierarchical menus that are inserted in the
 *    menubar but are not submenus of a top-level menus (for example,
 *    it does not search menus that are inserted for use in a popup
 *    menu control). In Mac OS X 10.1 and CarbonLib 1.5 and later, this
 *    API also searches inserted hierarchical menus.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for items with the
 *      specified command ID. Pass NULL to hegin searching with the
 *      root menu. The search will descend into all submenus of this
 *      menu.
 *    
 *    inCommandID:
 *      The command ID for which to search.
 *    
 *    inItemIndex:
 *      The 1-based index of the menu item to retrieve. In CarbonLib
 *      1.0.x and 1.1, this parameter must be 1. In CarbonLib 1.2 and
 *      Mac OS X 10.0, this parameter may vary from 1 to the number of
 *      menu items with the specified command ID.
 *    
 *    outMenu:
 *      On exit, the menu containing the menu item with the specified
 *      command ID.
 *    
 *    outIndex:
 *      On exit, the item index of the menu item with the specified
 *      command ID.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetIndMenuItemWithCommandID(
  MenuRef          inMenu,
  MenuCommand      inCommandID,
  UInt32           inItemIndex,
  MenuRef *        outMenu,           /* can be NULL */
  MenuItemIndex *  outIndex);         /* can be NULL */


/*
 *  EnableMenuCommand()
 *  
 *  Summary:
 *    Enables the menu item with a specified command ID.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item to be enabled. If more than one
 *      item has this command ID, only the first will be enabled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
EnableMenuCommand(
  MenuRef       inMenu,
  MenuCommand   inCommandID);


/*
 *  DisableMenuCommand()
 *  
 *  Summary:
 *    Disables the menu item with a specified command ID.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item to be disabled. If more than
 *      one item has this command ID, only the first will be disabled.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
DisableMenuCommand(
  MenuRef       inMenu,
  MenuCommand   inCommandID);


/*
 *  IsMenuCommandEnabled()
 *  
 *  Summary:
 *    Determines if the menu item with a specified command ID is
 *    enabled.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item to examine. If more than one
 *      item has this command ID, only the first will be examined.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuCommandEnabled(
  MenuRef       inMenu,
  MenuCommand   inCommandID);


/*
 *  SetMenuCommandMark()
 *  
 *  Summary:
 *    Locates the menu item with a specified command ID and sets its
 *    mark character.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item to be modified. If more than
 *      one item has this command ID, only the first will be modified.
 *    
 *    inMark:
 *      The new mark character. This is a Unicode character. On Mac OS
 *      8.x, the low byte of this character will be used as the mark
 *      character. On Mac OS X, the entire UniChar will be used and
 *      drawn.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuCommandMark(
  MenuRef       inMenu,
  MenuCommand   inCommandID,
  UniChar       inMark);


/*
 *  GetMenuCommandMark()
 *  
 *  Summary:
 *    Locates the menu item with a specified command ID and returns its
 *    mark character.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item to be examined. If more than
 *      one item has this command ID, only the first will be examined.
 *    
 *    outMark:
 *      On exit, the menu item's mark character.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuCommandMark(
  MenuRef       inMenu,
  MenuCommand   inCommandID,
  UniChar *     outMark);


/*
 *  GetMenuCommandProperty()
 *  
 *  Summary:
 *    Retrives property data for a menu item with a specified command
 *    ID.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item containing the property. If
 *      more than one item has this command ID, only the first will be
 *      used.
 *    
 *    inPropertyCreator:
 *      The property creator.
 *    
 *    inPropertyTag:
 *      The property tag.
 *    
 *    inBufferSize:
 *      The size of the output buffer, in bytes.
 *    
 *    outActualSize:
 *      On exit, contains the actual size of the property data. May be
 *      NULL if you do not need this information.
 *    
 *    inPropertyBuffer:
 *      The address of a buffer in which to place the property data.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuCommandProperty(
  MenuRef       inMenu,
  MenuCommand   inCommandID,
  OSType        inPropertyCreator,
  OSType        inPropertyTag,
  ByteCount     inBufferSize,
  ByteCount *   outActualSize,           /* can be NULL */
  void *        inPropertyBuffer);


/*
 *  GetMenuCommandPropertySize()
 *  
 *  Summary:
 *    Retrives the size of property data for a menu item with a
 *    specified command ID.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item containing the property. If
 *      more than one item has this command ID, only the first will be
 *      used.
 *    
 *    inPropertyCreator:
 *      The property creator.
 *    
 *    inPropertyTag:
 *      The property tag.
 *    
 *    outSize:
 *      On exit, contains the size of the property data.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuCommandPropertySize(
  MenuRef       inMenu,
  MenuCommand   inCommandID,
  OSType        inPropertyCreator,
  OSType        inPropertyTag,
  ByteCount *   outSize);


/*
 *  SetMenuCommandProperty()
 *  
 *  Summary:
 *    Sets property data for a menu item with a specified command ID.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item that will receive the property.
 *      If more than one item has this command ID, only the first will
 *      be modified.
 *    
 *    inPropertyCreator:
 *      The property creator.
 *    
 *    inPropertyTag:
 *      The property tag.
 *    
 *    inPropertySize:
 *      The size of the property data, in bytes.
 *    
 *    inPropertyData:
 *      The address of the property data.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuCommandProperty(
  MenuRef       inMenu,
  MenuCommand   inCommandID,
  OSType        inPropertyCreator,
  OSType        inPropertyTag,
  ByteCount     inPropertySize,
  const void *  inPropertyData);


/*
 *  RemoveMenuCommandProperty()
 *  
 *  Summary:
 *    Removes a property from a menu item with a specified command ID.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu in which to begin searching for the item. Pass NULL to
 *      begin searching with the root menu. The search will descend
 *      into all submenus of this menu.
 *    
 *    inCommandID:
 *      The command ID of the menu item from which the property will be
 *      removed. If more than one item has this command ID, only the
 *      first will be modified.
 *    
 *    inPropertyCreator:
 *      The property creator.
 *    
 *    inPropertyTag:
 *      The property tag.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
RemoveMenuCommandProperty(
  MenuRef       inMenu,
  MenuCommand   inCommandID,
  OSType        inPropertyCreator,
  OSType        inPropertyTag);



/*
 *  CopyMenuItemData()
 *  
 *  Summary:
 *    Returns multiple attributes of a menu item at once.
 *  
 *  Discussion:
 *    This function is used to retrieve many attributes of a menu item
 *    simultaneously; for example, it might be used by a menu
 *    definition function that needs to know how to draw a menu item.
 *    It is more efficient to use this function than to use the
 *    accessor functions for the individual attributes of the menu.
 *    This function returns a copy of the data in the menu, so any data
 *    in the MenuItemDataRec that is dynamically allocated (for
 *    example, the CFString item text) should be released by the caller.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu from which to copy data. If inIsCommandID is true, you
 *      may pass NULL for this parameter to search for an item in the
 *      root menu; if inIsCommandID is false, this parameter must be a
 *      valid MenuRef.
 *    
 *    inItem:
 *      The item or command ID from which to copy data.
 *    
 *    inIsCommandID:
 *      Indicates whether inItem is a MenuItemIndex or MenuCommand. If
 *      inIsCommandID is true, the inItem parameter is interpreted as a
 *      menu command ID, and data is copied from the first item in the
 *      menu with that command ID. If inIsCommandID is false, the
 *      inItem parameter is interpreted as a menu item index, and data
 *      is copied for that item in the specified menu.
 *    
 *    ioData:
 *      Data is copied from the item and placed here. On entry, the
 *      whichData field of this structure should be initialized to
 *      indicate which data the caller would like returned. Individual
 *      fields of the MenuItemDataRec structure may require
 *      pre-initialization also; see the individual MenuItemDataFlags
 *      documentation for details.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CopyMenuItemData(
  MenuRef           inMenu,              /* can be NULL */
  MenuItemID        inItem,
  Boolean           inIsCommandID,
  MenuItemDataPtr   ioData);


/*
 *  SetMenuItemData()
 *  
 *  Summary:
 *    Sets multiple attributes of a menu item at once.
 *  
 *  Discussion:
 *    This function is used to set many attributes of a menu item
 *    simultaneously. It is more efficient to use this function than to
 *    use the accessor functions for the individual attributes of the
 *    menu.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu to modify.
 *    
 *    inItem:
 *      The item or command ID to modify.
 *    
 *    inIsCommandID:
 *      Indicates whether inItem is a MenuItemIndex or MenuCommand. If
 *      inIsCommandID is true, the inItem parameter is interpreted as a
 *      menu command ID, and the first item in the menu with that
 *      command ID. is modified. If inIsCommandID is false, the inItem
 *      parameter is interpreted as a menu item index, and the item
 *      with that index in the specified menu is modified.
 *    
 *    inData:
 *      The data to set. The caller should set the whichData field of
 *      this structure to indicate which data should be set. Only the
 *      fields of the structure corresponding to the non-zero whichData
 *      flags must be initialized; other fields are ignored.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuItemData(
  MenuRef                  inMenu,
  MenuItemID               inItem,
  Boolean                  inIsCommandID,
  const MenuItemDataRec *  inData);



/*--------------------------------------------------------------------------------------*/
/*  o Dynamic menu item support (CarbonLib 1.1 and Carbon for Mac OS X, and later)      */
/*                                                                                      */
/*  Dynamic menu item support allows a menu item to be redrawn while the menu is open   */
/*  and visible to the user. Carbon contains automatic support for dynamic items based  */
/*  on keyboard modifier state. If you need to implement your own variable item state   */
/*  based on other system state, you can use these APIs to implement it.                */
/*                                                                                      */
/*  To use the built-in support for dynamic items, you should create a menu containing  */
/*  several contiguous items with the same command key but different text and modifier  */
/*  keys. For example, you might have:                                                  */
/*                                                                                      */
/*      Close       cmd-W                                                               */
/*      Close All   cmd-option-W                                                        */
/*                                                                                      */
/*  In your MENU resource, you would create the Close and Close All items and give      */
/*  them each the letter 'W' as the command key; using an associated xmnu resource,     */
/*  you would specify kMenuOptionModifier as the modifier for the Close All item.       */
/*                                                                                      */
/*  After loading your menu from the resource, you must set the kMenuItemAttrDynamic    */
/*  flag for each dynamic item. In this example, you would use:                         */
/*                                                                                      */
/*      ChangeMenuItemAttributes( menu, kCloseItem, kMenuItemAttrDynamic, 0 );          */
/*      ChangeMenuItemAttributes( menu, kCloseAllItem, kMenuItemAttrDynamic, 0 );       */
/*                                                                                      */
/*  The Menu Manager will now automatically display the correct item depending on       */
/*  whether the Option key is pressed. The result from MenuSelect will be the item      */
/*  number of the item that was visible when the menu closed.                           */
/*                                                                                      */
/*  If the Menu Manager's built-in support is not sufficient, you can also change the   */
/*  attributes of an item yourself and use the UpdateInvalidMenuItems API to cause      */
/*  the menu to redraw. Changes to a menu item (changing text, command key, style,      */
/*  etc.) that occur while the menu is open will cause the menu item to be invalidated, */
/*  but not redrawn. If you need to invalidate the item explicitly yourself, perhaps    */
/*  because you have a custom MDEF that depends on state not accessed using Menu        */
/*  Manager APIs, you can use the InvalidateMenuItems API. UpdateInvalidMenuItems will  */
/*  scan the menu for invalid items and redraw each, clearing its invalid flag          */
/*  afterwards.                                                                         */
/*                                                                                      */
/*  If you need to change menu contents based on modifier key state without using the   */
/*  built-in support in the Menu Manager, we recommend that you install a Carbon event  */
/*  handler on your menu for the [kEventClassKeyboard, kEventRawKeyModifiersChanged]    */
/*  event. Modifier key events are passed to the currently open menu before being sent  */
/*  to the user focus target.                                                           */
/*--------------------------------------------------------------------------------------*/
/*
 *  IsMenuItemInvalid()
 *  
 *  Summary:
 *    Determines if a menu item is invalid and should be redrawn.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose item to examine.
 *    
 *    inItem:
 *      The item to examine.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsMenuItemInvalid(
  MenuRef         inMenu,
  MenuItemIndex   inItem);


/*
 *  InvalidateMenuItems()
 *  
 *  Summary:
 *    Invalidates a group of menu items so that they will be redrawn
 *    when UpdateInvalidMenuItems is next called.
 *  
 *  Discussion:
 *    Menu items are automatically invalidated when their contents are
 *    changed using Menu Manager APIs while the menu is open. However,
 *    you might need to use this API if you have a custom MDEF that
 *    draws using state not contained in the menu.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu whose items to invalidate.
 *    
 *    inFirstItem:
 *      The first item to invalidate.
 *    
 *    inNumItems:
 *      The number of items to invalidate.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InvalidateMenuItems(
  MenuRef         inMenu,
  MenuItemIndex   inFirstItem,
  ItemCount       inNumItems);


/*
 *  UpdateInvalidMenuItems()
 *  
 *  Summary:
 *    Redraws the invalid items of an open menu.
 *  
 *  Discussion:
 *    It is not necessary to use UpdateInvalidMenuItems if you are
 *    using Carbon's built-in support for dynamic items based on
 *    modifier key state. However, if you are modifying items
 *    dynamically using your own implementation, you should call
 *    UpdateInvalidMenuItems after completing your modifications for a
 *    single menu. It will redraw any items that have been marked as
 *    invalid, and clear the invalid flag for those items.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      The menu to update.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
UpdateInvalidMenuItems(MenuRef inMenu);



/*--------------------------------------------------------------------------------------*/
/*  o.Standard font menu (Carbon and later)                                             */
/*                                                                                      */
/*  These APIs allow you to create and use the standard font menu.                      */
/*--------------------------------------------------------------------------------------*/
enum {
  kHierarchicalFontMenuOption   = 0x00000001
};

/*
 *  CreateStandardFontMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
CreateStandardFontMenu(
  MenuRef         menu,
  MenuItemIndex   afterItem,
  MenuID          firstHierMenuID,
  OptionBits      options,
  ItemCount *     outHierMenuCount);


/*
 *  UpdateStandardFontMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
UpdateStandardFontMenu(
  MenuRef      menu,
  ItemCount *  outHierMenuCount);


/*
 *  GetFontFamilyFromMenuSelection()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetFontFamilyFromMenuSelection(
  MenuRef         menu,
  MenuItemIndex   item,
  FMFontFamily *  outFontFamily,
  FMFontStyle *   outStyle);



/*--------------------------------------------------------------------------------------*/
/*  o Contextual Menu routines and constants                                            */
/*  available with Conxtextual Menu extension 1.0 and later                             */
/*--------------------------------------------------------------------------------------*/
/* Gestalt Selector for classic 68K apps only. */
/* CFM apps should weak link and check the symbols. */
enum {
  gestaltContextualMenuAttr     = FOUR_CHAR_CODE('cmnu'),
  gestaltContextualMenuUnusedBit = 0,
  gestaltContextualMenuTrapAvailable = 1,
  gestaltContextualMenuHasAttributeAndModifierKeys = 2, /* Contextual Menu Manager supports keyContextualMenuAttributes and keyContextualMenuModifiers */
  gestaltContextualMenuHasUnicodeSupport = 3 /* Contextual Menu Manager supports typeUnicodeText and typeCFStringRef */
};


/*
 *  Summary:
 *    Values indicating what kind of help the application supports
 */
enum {

  /*
   * The application does not support any help. The Menu Manager will
   * put an appropriate help string into the contextual menu and
   * disable the Help item.
   */
  kCMHelpItemNoHelp             = 0,

  /*
   * The application supports Apple Guide help. The Menu Manager will
   * put the name of the main Guide file into the contextual menu and
   * enable the Help item.
   */
  kCMHelpItemAppleGuide         = 1,

  /*
   * The application supports some other form of help. In this case,
   * the application must also pass a valid string into the
   * inHelpItemString parameter of ContextualMenuSelect. This string
   * will be the text of the Help item in the contextual menu, and the
   * Help item will be enabled.
   */
  kCMHelpItemOtherHelp          = 2,

  /*
   * The application does not support any help. The Menu Manager will
   * remove the Help item from the contextual menu. This constant is
   * available in Mac OS X and CarbonLib 1.6, and later; however, in
   * CarbonLib it is translated to kCMHelpItemNoHelp, and the Help item
   * is only disabled, not removed.
   */
  kCMHelpItemRemoveHelp         = 3
};


/*
 *  Summary:
 *    Values indicating what was chosen from a contextual menu
 */
enum {

  /*
   * The user did not choose an item from the contextual menu and the
   * application should do no further processing of the event.
   */
  kCMNothingSelected            = 0,

  /*
   * The user chose one of the application's items from the menu. The
   * application can examine the outMenuID and outMenuItem parameters
   * of ContextualMenuSelect to see what the menu selection was, and it
   * should then handle the selection appropriately.
   */
  kCMMenuItemSelected           = 1,

  /*
   * The user chose the Help item from the menu. The application should
   * open an Apple Guide database to a section appropriate for the
   * selection. If the application supports some other form of help, it
   * should be presented instead.
   */
  kCMShowHelpSelected           = 3
};



/*
 *  Summary:
 *    AERecord keywords used by the ExamineContext method of a
 *    Contextual Menu plugin to specify menu item contents.
 */
enum {

  /*
   * Specifies the text of an item in a contextual menu. Data for this
   * parameter can be in one of several formats. In Mac OS 7/8/9.x and
   * Mac OS X 10.0 and 10.1, typeChar and typeIntlText are supported.
   * In Mac OS X after 10.1, typeStyledText, typeAEText,
   * typeUnicodeText, and typeCFStringRef are also supported. If you
   * provide data as typeCFStringRef, the Contextual Menu Manager will
   * automatically release the CFStringRef once the menu has been
   * displayed. If you need the CFStringRef to have a longer timetime,
   * your plugin should retain the CFStringRef before inserting it into
   * the AERecord.
   */
  keyContextualMenuName         = FOUR_CHAR_CODE('pnam'),

  /*
   * Specifies the command ID of an item in a contextual menu. Data for
   * this parameter should be typeLongInteger.
   */
  keyContextualMenuCommandID    = FOUR_CHAR_CODE('cmcd'),

  /*
   * Specifies a contextual menu item with a submenu. Typically used
   * with AEPutKeyDesc to add an entire AEDesc containing the submenu
   * as the data for the parameter.
   */
  keyContextualMenuSubmenu      = FOUR_CHAR_CODE('cmsb'),

  /*
   * Specifies the menu item attributes of an item in a contextual
   * menu. Data for this parameter should be typeLongInteger. Available
   * in Mac OS X after 10.1.
   */
  keyContextualMenuAttributes   = FOUR_CHAR_CODE('cmat'),

  /*
   * Specifies the modifier keys of an item in a contextual menu (see
   * kMenuShiftModifier, kMenuControlModifier, etc.) Data for this
   * parameter should be typeLongInteger. Using this parameter together
   * with the keyContextualMenuAttributes parameter, it is possible to
   * create a contextual menu with dynamic items which change according
   * to the modifier keys pressed by the user. Available in Mac OS X
   * after 10.1.
   */
  keyContextualMenuModifiers    = FOUR_CHAR_CODE('cmmd')
};

/*
 *  InitContextualMenus()
 *  
 *  Summary:
 *    Adds the current process to the system registry of contextual
 *    menu clients.
 *  
 *  Discussion:
 *    On Mac OS 8.x and 9.x, your program should call the
 *    InitContextualMenus function early in your startup code to
 *    register your application as a contextual menu client. If you do
 *    not register your program, some system-level functions may
 *    respond as though your program does not use contextual menus. Not
 *    registering your program may also cause
 *    ProcessIsContextualMenuClient to return an incorrect value. On
 *    Mac OS X, it is not necessary to call InitContextualMenus.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ContextualMenu 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
InitContextualMenus(void)                                     TWOWORDINLINE(0x7001, 0xAA72);


/*
 *  IsShowContextualMenuClick()
 *  
 *  Summary:
 *    Determines whether a particular EventRecord could invoke a
 *    contextual menu.
 *  
 *  Discussion:
 *    Applications should call IsShowContextualMenuClick when they
 *    receive non-null events. If IsShowContextualMenuClick returns
 *    true, your application should generate its own menu and Apple
 *    Event descriptor (AEDesc), and then call ContextualMenuSelect to
 *    display and track the contextual menu, and then handle the user's
 *    choice. Some users may choose to use a two-button mouse with
 *    their Macintosh computer. This API does not return true for a
 *    right-click unless the mouse manufacturer has provided driver
 *    software that returns a control-left click in place of a right
 *    click. For proper recognition of a right-click gesture, you must
 *    use the IsShowContextualMenuEvent API.
 *  
 *  Parameters:
 *    
 *    inEvent:
 *      The event to examine.
 *  
 *  Result:
 *    Returns true if the application should display a contextual menu,
 *    false if not.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ContextualMenu 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsShowContextualMenuClick(const EventRecord * inEvent)        TWOWORDINLINE(0x7002, 0xAA72);


/*
 *  IsShowContextualMenuEvent()
 *  
 *  Summary:
 *    Determines whether a particular EventRef could invoke a
 *    contextual menu.
 *  
 *  Discussion:
 *    This API is similar to IsShowContextualMenuClick, but takes a
 *    Carbon EventRef as its parameter instead of an EventRecord.
 *    EventRecords cannot express a right-mouse-click, but EventRefs
 *    can, so this API will return true for a right- click where
 *    IsShowContextualMenuClick will not.
 *  
 *  Parameters:
 *    
 *    inEvent:
 *      The event to examine.
 *  
 *  Result:
 *    Returns true if the application should display a contextual menu,
 *    false if not.
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.1 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
IsShowContextualMenuEvent(EventRef inEvent);


/*
 *  ContextualMenuSelect()
 *  
 *  Summary:
 *    Displays a contextual menu.
 *  
 *  Discussion:
 *    If IsShowContextualMenuClick returns true, you should call the
 *    ContextualMenuSelect API after generating your own menu and
 *    preparing an Apple Event descriptor (AEDesc) that describes the
 *    item for which your application is displaying a contextual menu.
 *    This descriptor may contain an object specifier or raw data and
 *    will be passed to all contextual menu plug-ins. The system will
 *    add other items before displaying the contextual menu, and it
 *    will remove those items before returning, leaving the menu in its
 *    original state. After all the system commands are added, the
 *    contextual menu is displayed and tracked. If the user selects one
 *    of the system items, it is handled by the system and the call
 *    returns as though the user didn't select anything from the menu.
 *    If the user selects any other item (or no item at all), the Menu
 *    Manager passes back appropriate values in the parameters
 *    outUserSelectionType, outMenuID, and outMenuItem. Your
 *    application should provide visual feedback indicating the item
 *    that was clicked upon. For example, a click on an icon should
 *    highlight the icon, while a click on editable text should not
 *    eliminate the current selection. If the outUserSelectionType
 *    parameter contains kCMMenuItemSelected, you should look at the
 *    outMenuID and outMenuItem parameters to determine what menu item
 *    the user chose and handle it appropriately. If the user selected
 *    kCMHelpItemSelected, you should open the proper Apple Guide
 *    sequence or other form of custom help.
 *  
 *  Parameters:
 *    
 *    inMenu:
 *      A menu containing application commands to display. The caller
 *      creates this menu based on the current context, the mouse
 *      location, and the current selection (if it was the target of
 *      the mouse). If you pass nil, only system commands will be
 *      displayed. The menu should be added to the menu list as a
 *      pop-up menu (using the InsertMenu function).
 *    
 *    inGlobalLocation:
 *      The location (in global coordinates) of the mouse near which
 *      the menu is to be displayed.
 *    
 *    inReserved:
 *      Reserved for future use. Pass false for this parameter.
 *    
 *    inHelpType:
 *      An identifier specifying the type of help to be provided by the
 *      application; see kCMHelpItem constants.
 *    
 *    inHelpItemString:
 *      A string containing the text to be displayed for the help menu
 *      item. This string is unused unless you also pass the constant
 *      kCMHelpItemOtherHelp in the inHelpType parameter.
 *    
 *    inSelection:
 *      An object specifier for the current selection. This allows he
 *      system to examine the selection and add special system commands
 *      accordingly. Passing a value of nil indicates that no selection
 *      should be examined, and most likely, no special system actions
 *      will be included.
 *    
 *    outUserSelectionType:
 *      On exit, the value indicates what the user selected from the
 *      contextual menu; see kCMNothingSelected, kCMMenuItemSelected,
 *      and kCMShowHelpSelected.
 *    
 *    outMenuID:
 *      On exit, if outUserSelectionType is set to kCMMenuItemSelected,
 *      the value is set to the menu ID of the chosen item.
 *    
 *    outMenuItem:
 *      On exit, if outUserSelectionType is set to kCMMenuItemSelected,
 *      the value is set to the menu item chosen.
 *  
 *  Result:
 *    An OSStatus result code. ContextualMenuSelect returns the result
 *    code userCanceledErr and sets outUserSelectionType to
 *    kCMNothingSelected to indicate that the user did not select
 *    anything from the contextual menu and no further processing is
 *    needed.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ContextualMenu 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
ContextualMenuSelect(
  MenuRef            inMenu,
  Point              inGlobalLocation,
  Boolean            inReserved,
  UInt32             inHelpType,
  ConstStr255Param   inHelpItemString,           /* can be NULL */
  const AEDesc *     inSelection,                /* can be NULL */
  UInt32 *           outUserSelectionType,
  SInt16 *           outMenuID,
  MenuItemIndex *    outMenuItem)                             TWOWORDINLINE(0x7003, 0xAA72);


/*
 *  ProcessIsContextualMenuClient()
 *  
 *  Summary:
 *    Determines whether the specified process is a contextual menu
 *    client.
 *  
 *  Discussion:
 *    On Mac OS 9, this API consults a global table of all processes
 *    that have registered with the Contextual Menu Manager by calling
 *    InitContextualMenus. On Mac OS X, this API ignores the inPSN
 *    parameter and always returns whether the current process is
 *    registered with the Contextual Menu Manager.
 *  
 *  Parameters:
 *    
 *    inPSN:
 *      The process to examine.
 *  
 *  Result:
 *    Whether the specified process (or, on Mac OS X, the current
 *    process) is registered with the Contextual Menu Manager.
 *  
 *  Availability:
 *    Non-Carbon CFM:   in ContextualMenu 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( Boolean )
ProcessIsContextualMenuClient(ProcessSerialNumber * inPSN)    TWOWORDINLINE(0x7004, 0xAA72);



/*--------------------------------------------------------------------------------------*/
/*  Contextual Menu Plugin Interface                                                    */
/*                                                                                      */
/*  For Mac OS X 10.1, we support a new type of Contextual Menu Plugin: the CFPlugIn    */
/*  based plugin.  Each plugin must be in a CFPlugIn in the Contextual Menu Items       */
/*  folder in one of these paths:                                                       */
/*      /System/Library/Contextual Menu Items/                                          */
/*      /Library/Contextual Menu Items/                                                 */
/*      ~/Library/Contextual Menu Items/                                                */
/*                                                                                      */
/*  It must export the following functions using the following interface or a C++       */
/*  interface inheriting from IUnknown and including similar functions.                 */
/*--------------------------------------------------------------------------------------*/

/* The Contextual Menu Manager will only load CFPlugIns of type kContextualMenuTypeID */
#define kContextualMenuTypeID ( CFUUIDGetConstantUUIDWithBytes( NULL, \
  0x2F, 0x65, 0x22, 0xE9, 0x3E, 0x66, 0x11, 0xD5, \
  0x80, 0xA7, 0x00, 0x30, 0x65, 0xB3, 0x00, 0xBC ) )
 /* 2F6522E9-3E66-11D5-80A7-003065B300BC */

/* Contextual Menu Plugins must implement this Contexual Menu Plugin Interface */
#define kContextualMenuInterfaceID    ( CFUUIDGetConstantUUIDWithBytes( NULL, \
  0x32, 0x99, 0x7B, 0x62, 0x3E, 0x66, 0x11, 0xD5, \
  0xBE, 0xAB, 0x00, 0x30, 0x65, 0xB3, 0x00, 0xBC ) )
 /* 32997B62-3E66-11D5-BEAB-003065B300BC */

#define CM_IUNKNOWN_C_GUTS \
   void *_reserved; \
 SInt32 (*QueryInterface)(void *thisPointer, CFUUIDBytes iid, void ** ppv); \
   UInt32 (*AddRef)(void *thisPointer); \
 UInt32 (*Release)(void *thisPointer)

/* The function table for the interface */
struct ContextualMenuInterfaceStruct
{
    CM_IUNKNOWN_C_GUTS;
    OSStatus ( *ExamineContext )(
          void*               thisInstance,
          const AEDesc*       inContext,
         AEDescList*         outCommandPairs );
 OSStatus ( *HandleSelection )(
         void*               thisInstance,
          AEDesc*             inContext,
         SInt32              inCommandID );
 void ( *PostMenuCleanup )(
         void*               thisInstance );
};
typedef struct ContextualMenuInterfaceStruct ContextualMenuInterfaceStruct;

/*
 *  CMPluginExamineContext()
 *  
 *  Availability:
 *    Implemented by client
 */
EXTERN_API_C( OSStatus )
CMPluginExamineContext(
  void *          thisInstance,
  const AEDesc *  inContext,
  AEDescList *    outCommandPairs);


/*
 *  CMPluginHandleSelection()
 *  
 *  Availability:
 *    Implemented by client
 */
EXTERN_API_C( OSStatus )
CMPluginHandleSelection(
  void *    thisInstance,
  AEDesc *  inContext,
  SInt32    inCommandID);


/*
 *  CMPluginPostMenuCleanup()
 *  
 *  Availability:
 *    Implemented by client
 */
EXTERN_API_C( void )
CMPluginPostMenuCleanup(void * thisInstance);



/* previously in LowMem.h.  This functions return the menu ID of the hilited menu */
/*
 *  LMGetTheMenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
LMGetTheMenu(void)                                            TWOWORDINLINE(0x3EB8, 0x0A26);




#if CALL_NOT_IN_CARBON
/*
 *  newmenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( MenuRef )
newmenu(
  MenuID        menuID,
  const char *  menuTitle);


/*
 *  appendmenu()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
appendmenu(
  MenuRef       menu,
  const char *  data);


/*
 *  insertmenuitem()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
insertmenuitem(
  MenuRef       theMenu,
  const char *  itemString,
  short         afterItem);


/*
 *  menuselect()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( long )
menuselect(const Point * startPt);


/*
 *  setmenuitemtext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
setmenuitemtext(
  MenuRef       menu,
  short         item,
  const char *  itemString);


/*
 *  getmenuitemtext()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in InterfaceLib 7.1 and later
 *    CarbonLib:        not available
 *    Mac OS X:         not available
 */
EXTERN_API_C( void )
getmenuitemtext(
  MenuRef   menu,
  short     item,
  char *    itemString);


#endif  /* CALL_NOT_IN_CARBON */

#if OLDROUTINENAMES
#define AddResMenu(theMenu, theType) AppendResMenu(theMenu, theType)
#define InsMenuItem(theMenu, itemString, afterItem) InsertMenuItem(theMenu, itemString, afterItem)
#define DelMenuItem( theMenu, item ) DeleteMenuItem( theMenu, item )
#if TARGET_OS_MAC
#define SetItem MacSetItem
#define GetItem MacGetItem
#endif
#define MacSetItem(theMenu, item, itemString) SetMenuItemText(theMenu, item, itemString)
#define MacGetItem(theMenu, item, itemString) GetMenuItemText(theMenu, item, itemString)
#define GetMHandle(menuID) GetMenuHandle(menuID)
#define DelMCEntries(menuID, menuItem) DeleteMCEntries(menuID, menuItem)
#define DispMCInfo(menuCTbl) DisposeMCInfo(menuCTbl)
#if CALL_NOT_IN_CARBON
#define addresmenu(menu, data) appendresmenu(menu, data)
#define getitem(menu, item, itemString) getmenuitemtext(menu, item, itemString)
#define setitem(menu, item, itemString) setmenuitemtext(menu, item, itemString)
#define insmenuitem(theMenu, itemString, afterItem) insertmenuitem(theMenu, itemString, afterItem)
#endif
#endif  /* OLDROUTINENAMES */

#if ACCESSOR_CALLS_ARE_FUNCTIONS
/* Getters */
/*
 *  GetMenuID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( MenuID )
GetMenuID(MenuRef menu);


/*
 *  GetMenuWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
GetMenuWidth(MenuRef menu);


/*
 *  GetMenuHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( SInt16 )
GetMenuHeight(MenuRef menu);


/*
 *  GetMenuTitle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( StringPtr )
GetMenuTitle(
  MenuRef   menu,
  Str255    title);


/*
 *  GetMenuDefinition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
GetMenuDefinition(
  MenuRef          menu,
  MenuDefSpecPtr   outDefSpec);


/* Setters */
/*
 *  SetMenuID()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMenuID(
  MenuRef   menu,
  MenuID    menuID);


/*
 *  SetMenuWidth()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMenuWidth(
  MenuRef   menu,
  SInt16    width);


/*
 *  SetMenuHeight()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( void )
SetMenuHeight(
  MenuRef   menu,
  SInt16    height);


/*
 *  SetMenuTitle()
 *  
 *  Availability:
 *    Non-Carbon CFM:   in CarbonAccessors.o 1.0 and later
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuTitle(
  MenuRef            menu,
  ConstStr255Param   title);


/*
 *  SetMenuDefinition()
 *  
 *  Availability:
 *    Non-Carbon CFM:   not available
 *    CarbonLib:        in CarbonLib 1.0 and later
 *    Mac OS X:         in version 10.0 and later
 */
EXTERN_API( OSStatus )
SetMenuDefinition(
  MenuRef              menu,
  const MenuDefSpec *  defSpec);


#endif  /* ACCESSOR_CALLS_ARE_FUNCTIONS */




#if TARGET_OS_WIN32
#endif  /* TARGET_OS_WIN32 */


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

#endif /* __MENUS__ */

