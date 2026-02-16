#ifndef TUI_H
#define TUI_H
#include <windows.h>

typedef enum {
    MENU_ITEM_OPTION,
    MENU_ITEM_LABEL
} MenuItemType;

typedef enum {
    MENU_OTHER  =  0,     // no meaningful input / normal char
    MENU_UP     = -1,
    MENU_DOWN   =  1,
    MENU_ACCEPT =  2,
    MENU_CANCEL =  3,      // printable char, escape, etc.
    MENU_TEXT   =  4
} MenuControl;


typedef struct MenuItem MenuItem;
typedef struct Menu Menu;

struct MenuItem {
    MenuItemType type;
    int id;
    const char *text;
    int width;
};

struct Menu {
    COORD position;
    COORD size;
    const char *title;
    MenuItem **items;
    int itemCount;
    int capacity;
    int selectedIndex;
    int showBorder;
};

int TUIInitConsole();
void TUIRestoreConsole();
int TUIHideCursor();
Menu* TUIMenuCreate(const char *title, int capacity, int withBorder);
MenuItem* TUIMenuAddItem(Menu *menu, MenuItemType type, const char *text, int itemId);
void TUIMenuDestroy(Menu *menu);
int TUIDrawBox(COORD pos, COORD size, const char *title);
int TUIMenuSelectItem(Menu *menu, int index);
int TUIMenuDraw(Menu *menu);
MenuControl GetMenuControl();
MenuControl TUIMenuFocus(Menu *menu);
int TUIWrite(short x, short y, char *txt);
#endif