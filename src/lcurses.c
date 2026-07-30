#include <ncurses.h>
#include "lcurses.h"
#include "lua.h"
#include "lauxlib.h"

static int l_initscr(lua_State *L) {
	initscr();
	lua_pushinteger(L, COLS);
	lua_setglobal(L, "COLS");
	lua_pushinteger(L, LINES);
	lua_setglobal(L, "LINES");
	return 0;
}

static int l_endwin(lua_State *L) {
	endwin();
	return 0;
}

static int l_erase(lua_State *L) {
	erase();
	return 0;
}

static int l_refresh(lua_State *L) {
	refresh();
	lua_pushinteger(L, COLS);
	lua_setglobal(L, "COLS");
	lua_pushinteger(L, LINES);
	lua_setglobal(L, "LINES");
	return 0;
}

static int l_mvaddch(lua_State *L) {
	int y = luaL_checkinteger(L, 1);
	int x = luaL_checkinteger(L, 2);
	chtype ch = (chtype)luaL_checkinteger(L, 3);
	int ret = mvaddch(y, x, ch);
	lua_pushinteger(L, ret);
	return 1;
}

static int l_mvaddstr(lua_State *L) {
	int y = luaL_checkinteger(L, 1);
	int x = luaL_checkinteger(L, 2);
	const char *str = luaL_checkstring(L, 3);
	int ret = mvaddstr(y, x, str);
	lua_pushinteger(L, ret);
	return 1;
}

static int l_mvprintw(lua_State *L) {
	int y = luaL_checkinteger(L, 1);
	int x = luaL_checkinteger(L, 2);
	const char *str = luaL_checkstring(L, 3);
	int ret = mvprintw(y, x, "%s", str);
	lua_pushinteger(L, ret);
	return 1;
}

static const luaL_Reg curses_funcs[] = {
	{"initscr", l_initscr},
	{"endwin", l_endwin},
	{"erase", l_erase},
	{"refresh", l_refresh},
	{"mvaddch", l_mvaddch},
	{"mvaddstr", l_mvaddstr},
	{"mvprintw", l_mvprintw},
	{NULL, NULL}
};

void luaopen_curses(lua_State *L) {
	luaL_newlib(L, curses_funcs);
	lua_setglobal(L, "curses");

	/* Constants */
	lua_pushinteger(L, COLS);
	lua_setglobal(L, "COLS");
	lua_pushinteger(L, LINES);
	lua_setglobal(L, "LINES");

	/* Attribute constants */
	lua_pushinteger(L, A_NORMAL);
	lua_setglobal(L, "A_NORMAL");
	lua_pushinteger(L, A_BOLD);
	lua_setglobal(L, "A_BOLD");
	lua_pushinteger(L, A_UNDERLINE);
	lua_setglobal(L, "A_UNDERLINE");
	lua_pushinteger(L, A_REVERSE);
	lua_setglobal(L, "A_REVERSE");
	lua_pushinteger(L, A_STANDOUT);
	lua_setglobal(L, "A_STANDOUT");
	lua_pushinteger(L, A_DIM);
	lua_setglobal(L, "A_DIM");
	lua_pushinteger(L, A_BLINK);
	lua_setglobal(L, "A_BLINK");
	lua_pushinteger(L, A_INVIS);
	lua_setglobal(L, "A_INVIS");
	lua_pushinteger(L, A_ITALIC);
	lua_setglobal(L, "A_ITALIC");

	/* Color constants */
	lua_pushinteger(L, COLOR_BLACK);
	lua_setglobal(L, "COLOR_BLACK");
	lua_pushinteger(L, COLOR_RED);
	lua_setglobal(L, "COLOR_RED");
	lua_pushinteger(L, COLOR_GREEN);
	lua_setglobal(L, "COLOR_GREEN");
	lua_pushinteger(L, COLOR_YELLOW);
	lua_setglobal(L, "COLOR_YELLOW");
	lua_pushinteger(L, COLOR_BLUE);
	lua_setglobal(L, "COLOR_BLUE");
	lua_pushinteger(L, COLOR_MAGENTA);
	lua_setglobal(L, "COLOR_MAGENTA");
	lua_pushinteger(L, COLOR_CYAN);
	lua_setglobal(L, "COLOR_CYAN");
	lua_pushinteger(L, COLOR_WHITE);
	lua_setglobal(L, "COLOR_WHITE");

	/* Key constants */
	lua_pushinteger(L, KEY_UP);
	lua_setglobal(L, "KEY_UP");
	lua_pushinteger(L, KEY_DOWN);
	lua_setglobal(L, "KEY_DOWN");
	lua_pushinteger(L, KEY_LEFT);
	lua_setglobal(L, "KEY_LEFT");
	lua_pushinteger(L, KEY_RIGHT);
	lua_setglobal(L, "KEY_RIGHT");
	lua_pushinteger(L, KEY_ENTER);
	lua_setglobal(L, "KEY_ENTER");
	lua_pushinteger(L, KEY_BACKSPACE);
	lua_setglobal(L, "KEY_BACKSPACE");
	lua_pushinteger(L, KEY_DC);
	lua_setglobal(L, "KEY_DC");
	lua_pushinteger(L, KEY_IC);
	lua_setglobal(L, "KEY_IC");
	lua_pushinteger(L, KEY_HOME);
	lua_setglobal(L, "KEY_HOME");
	lua_pushinteger(L, KEY_END);
	lua_setglobal(L, "KEY_END");
	lua_pushinteger(L, KEY_NPAGE);
	lua_setglobal(L, "KEY_NPAGE");
	lua_pushinteger(L, KEY_PPAGE);
	lua_setglobal(L, "KEY_PPAGE");
	lua_pushinteger(L, KEY_F0);
	lua_setglobal(L, "KEY_F0");
}
