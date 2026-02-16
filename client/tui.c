#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "../common/common.h"
#include "globals.h"
#include "tui.h"

static DWORD g_originalConsoleMode = 0;
static CONSOLE_CURSOR_INFO g_originalCursorInfo = {0};
static UINT g_originalOutputCP = 0;

int TUIInitConsole() {
    // Output: UTF-8 + remember original codepage
    g_originalOutputCP = GetConsoleOutputCP();
    if (!SetConsoleOutputCP(CP_UTF8)) return 1;
    // Hide cursor
    if (!GetConsoleCursorInfo(HOUT, &g_originalCursorInfo)) return 1;
    CONSOLE_CURSOR_INFO hidden = g_originalCursorInfo;
    hidden.bVisible = FALSE;
    if (!SetConsoleCursorInfo(HOUT, &hidden)) return 1;
    // Input: no echo + character mode (no line buffering)
    if (!GetConsoleMode(HIN, &g_originalConsoleMode)) return 1;
    DWORD rawMode = g_originalConsoleMode
                  & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    // Remove processed input for Ctrl+C / Ctrl+Break to pass through
    rawMode &= ~ENABLE_PROCESSED_INPUT;
    // Keep mouse / window resize events
    // rawMode |= ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    if (!SetConsoleMode(HIN, rawMode)) return 1;
    // Clear any junk that might be in the input buffer
    FlushConsoleInputBuffer(HIN);
    return 0;
}

void TUIRestoreConsole(void) {
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hStdin  = GetStdHandle(STD_INPUT_HANDLE);
    // Restore input mode
    if (g_originalConsoleMode != 0) {
        SetConsoleMode(hStdin, g_originalConsoleMode);
        FlushConsoleInputBuffer(hStdin);
    }
    // Restore cursor visibility
    if (g_originalCursorInfo.dwSize != 0) SetConsoleCursorInfo(hStdout, &g_originalCursorInfo);
    // Restore codepage
    if (g_originalOutputCP != 0) SetConsoleOutputCP(g_originalOutputCP);
}

Menu* TUIMenuCreate(const char *title, int capacity, int withBorder) {
    Menu *menu = calloc(1, sizeof(*menu));
    if (menu != NULL) {
        COORD size = {strlen(title) + 4, capacity + 2};
        menu->size = size;
        menu->title = title;
        menu->capacity = capacity;
        menu->showBorder = withBorder;
        menu->items = calloc(capacity, sizeof(*menu->items));
    }
    return menu;
}

int typewidth(MenuItemType type) {
    switch (type) {
        case MENU_ITEM_LABEL: return 2;
        case MENU_ITEM_OPTION: return 3;
        default: return 3;
    }
}

MenuItem* TUIMenuAddItem(Menu *menu, MenuItemType type, const char *text, int itemId) {
    if (menu->itemCount == menu->capacity) return NULL;
    MenuItem *item = calloc(1, sizeof(*item));
    if (item == NULL) return NULL;
    item->type = type;
    item->id = itemId;
    item->text = text;
    item->width = strlen(text) + typewidth(type);
    menu->items[menu->itemCount] = item;
    menu->itemCount++;
    menu->size.X = max(menu->size.X, item->width);
    return item;
}

void TUIMenuDestroy(Menu *menu) {
    for (int i = 0; i < menu->itemCount; i++) free(menu->items[i]);
    free(menu->items);
    free(menu);
}

int TUIDrawBox(COORD pos, COORD size, const char *title) {
    int x = pos.X;
    int y = pos.Y;
    int width = size.X;
    int height = size.Y;
    int titleLen = strlen(title);
    if (width < 4 || height < 3) return 1;
    if (width - titleLen < 4) return 1;
    if (x < 0 || y < 0 || x > COLS || y > ROWS) return 1;
    if (x + width > COLS || y + height > ROWS) return 1;
    COORD p = {x, y};
    SetConsoleCursorPosition(HOUT, p);
    printf("┌");
    printf(" %s ", title);
    for (int i = 0; i < width - titleLen - 4; i++) printf("─");
    printf("┐");
    for (int i = 1; i < height - 1; i++) {
        p.Y = y + i;
        SetConsoleCursorPosition(HOUT, p);
        printf("│%*s│", width - 2, "");
    }
    p.Y = y + height - 1;
    SetConsoleCursorPosition(HOUT, p);
    printf("└");
    for (int i = 0; i < width - 2; i++) printf("─");
    printf("┘");
    return 0;
}

int TUIMenuSelectItem(Menu *menu, int index) {
    COORD pos = menu->position;
    pos.X = menu->position.X + 1;
    pos.Y = menu->position.Y + 1 + menu->selectedIndex;
    SetConsoleCursorPosition(HOUT, pos);
    printf(" ");
    pos.Y = menu->position.Y + 1 + index;
    SetConsoleCursorPosition(HOUT, pos);
    printf(">");
    return 0;
}

int TUIMenuDraw(Menu *menu) {
    if (TUIDrawBox(menu->position, menu->size, menu->title)) return 1;
    COORD pos = menu->position;
    pos.X++;
    for (int i = 0; i < menu->itemCount; i++) {
        pos.Y++;
        SetConsoleCursorPosition(HOUT, pos);
        if (menu->items[i]->type == MENU_ITEM_OPTION) printf(" ");
        printf("%s", menu->items[i]->text);
    }
    return 0;
}

MenuControl GetMenuControl() {
    int c = getch();
    if (c == 0 || c == 224) {
        int c2 = getch();
        switch (c2) {
            case 72: return MENU_UP;
            case 80: return MENU_DOWN;
            default: return MENU_OTHER;
        }
    }
    switch (c) {
        case 13:
        case 10:
        case ' ':
            return MENU_ACCEPT;
        case 27:
            return MENU_CANCEL;
        default:
            if (c >= 32 && c <= 126) return MENU_TEXT;
            return MENU_OTHER;
    }
}

int selectable(Menu *menu, int direction) {
    const int start = menu->selectedIndex + direction;
    for (int i = start; i >= 0 && i < menu->itemCount; i += direction) {
        if (menu->items[i]->type == MENU_ITEM_OPTION) return i;
    }
    return -1;
}

MenuControl TUIMenuFocus(Menu *menu) {
    MenuControl c;
    do {
        c = GetMenuControl();
        if (c == MENU_UP || c == MENU_DOWN) {
            int i = selectable(menu, c);
            if (i != -1) {
                TUIMenuSelectItem(menu, i);
                menu->selectedIndex = i;
            }
        }
    } while (c != MENU_ACCEPT && c != MENU_CANCEL);
}

int TUIWrite(short x, short y, char *txt) {
    COORD pos = {x, y};
    if (!SetConsoleCursorPosition(HOUT, pos)) return 1;
    if (!fputs(txt, stdout)) return 1;
    return 0;
}