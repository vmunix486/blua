#include <ncurses.h>
#include "lcurses.h"
#include "lua.h"
#include "lauxlib.h"

/* ======================== Screen management ======================== */

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

/* ======================== Input ======================== */

static int l_getch(lua_State *L) {
	int ch = getch();
	lua_pushinteger(L, ch);
	return 1;
}

static int l_ungetch(lua_State *L) {
	int ch = luaL_checkinteger(L, 1);
	ungetch(ch);
	return 0;
}

static int l_getstr(lua_State *L) {
	char buf[1024];
	int ret = getstr(buf);
	if (ret == ERR) {
		lua_pushnil(L);
		return 1;
	}
	lua_pushstring(L, buf);
	return 1;
}

static int l_has_key(lua_State *L) {
	int ch = luaL_checkinteger(L, 1);
	lua_pushboolean(L, has_key(ch));
	return 1;
}

/* ======================== Cursor ======================== */

static int l_move(lua_State *L) {
	int y = luaL_checkinteger(L, 1);
	int x = luaL_checkinteger(L, 2);
	lua_pushinteger(L, move(y, x));
	return 1;
}

static int l_getyx(lua_State *L) {
	int y, x;
	getyx(stdscr, y, x);
	lua_pushinteger(L, y);
	lua_pushinteger(L, x);
	return 2;
}

static int l_getmaxyx(lua_State *L) {
	int maxy, maxx;
	getmaxyx(stdscr, maxy, maxx);
	lua_pushinteger(L, maxy);
	lua_pushinteger(L, maxx);
	return 2;
}

/* ======================== Output ======================== */

static int l_addch(lua_State *L) {
	chtype ch = (chtype)luaL_checkinteger(L, 1);
	lua_pushinteger(L, addch(ch));
	return 1;
}

static int l_addstr(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	lua_pushinteger(L, addstr(str));
	return 1;
}

static int l_printw(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	lua_pushinteger(L, printw("%s", str));
	return 1;
}

static int l_mvaddch(lua_State *L) {
	int y = luaL_checkinteger(L, 1);
	int x = luaL_checkinteger(L, 2);
	chtype ch = (chtype)luaL_checkinteger(L, 3);
	lua_pushinteger(L, mvaddch(y, x, ch));
	return 1;
}

static int l_mvaddstr(lua_State *L) {
	int y = luaL_checkinteger(L, 1);
	int x = luaL_checkinteger(L, 2);
	const char *str = luaL_checkstring(L, 3);
	lua_pushinteger(L, mvaddstr(y, x, str));
	return 1;
}

static int l_mvprintw(lua_State *L) {
	int y = luaL_checkinteger(L, 1);
	int x = luaL_checkinteger(L, 2);
	const char *str = luaL_checkstring(L, 3);
	lua_pushinteger(L, mvprintw(y, x, "%s", str));
	return 1;
}

static int l_clrtoeol(lua_State *L) {
	lua_pushinteger(L, clrtoeol());
	return 1;
}

static int l_clrtobot(lua_State *L) {
	lua_pushinteger(L, clrtobot());
	return 1;
}

static int l_deleteln(lua_State *L) {
	lua_pushinteger(L, deleteln());
	return 1;
}

static int l_insertln(lua_State *L) {
	lua_pushinteger(L, insertln());
	return 1;
}

/* ======================== Attributes ======================== */

static int l_attron(lua_State *L) {
	attron((chtype)luaL_checkinteger(L, 1));
	return 0;
}

static int l_attroff(lua_State *L) {
	attroff((chtype)luaL_checkinteger(L, 1));
	return 0;
}

static int l_attrset(lua_State *L) {
	attrset((chtype)luaL_checkinteger(L, 1));
	return 0;
}

static int l_color_pair(lua_State *L) {
	int n = luaL_checkinteger(L, 1);
	lua_pushinteger(L, COLOR_PAIR(n));
	return 1;
}

/* ======================== Colors ======================== */

static int l_start_color(lua_State *L) {
	lua_pushinteger(L, start_color());
	return 1;
}

static int l_init_pair(lua_State *L) {
	int n = luaL_checkinteger(L, 1);
	short fg = (short)luaL_checkinteger(L, 2);
	short bg = (short)luaL_checkinteger(L, 3);
	lua_pushinteger(L, init_pair(n, fg, bg));
	return 1;
}

static int l_pair_content(lua_State *L) {
	int n = luaL_checkinteger(L, 1);
	short fg, bg;
	int ret = pair_content(n, &fg, &bg);
	if (ret == ERR) {
		lua_pushnil(L);
		return 1;
	}
	lua_pushinteger(L, fg);
	lua_pushinteger(L, bg);
	return 2;
}

static int l_has_colors(lua_State *L) {
	lua_pushboolean(L, has_colors());
	return 1;
}

static int l_use_default_colors(lua_State *L) {
	lua_pushinteger(L, use_default_colors());
	return 1;
}

/* ======================== Mode ======================== */

static int l_cbreak(lua_State *L) {
	lua_pushinteger(L, cbreak());
	return 1;
}

static int l_nocbreak(lua_State *L) {
	lua_pushinteger(L, nocbreak());
	return 1;
}

static int l_echo(lua_State *L) {
	lua_pushinteger(L, echo());
	return 1;
}

static int l_noecho(lua_State *L) {
	lua_pushinteger(L, noecho());
	return 1;
}

static int l_raw(lua_State *L) {
	lua_pushinteger(L, raw());
	return 1;
}

static int l_noraw(lua_State *L) {
	lua_pushinteger(L, noraw());
	return 1;
}

static int l_curs_set(lua_State *L) {
	int vis = luaL_checkinteger(L, 1);
	lua_pushinteger(L, curs_set(vis));
	return 1;
}

static int l_nodelay(lua_State *L) {
	int flag = lua_toboolean(L, 1);
	lua_pushinteger(L, nodelay(stdscr, flag));
	return 1;
}

static int l_timeout(lua_State *L) {
	int ms = luaL_checkinteger(L, 1);
	timeout(ms);
	return 0;
}

static int l_keypad(lua_State *L) {
	int flag = lua_toboolean(L, 1);
	lua_pushinteger(L, keypad(stdscr, flag));
	return 1;
}

/* ======================== Window ops ======================== */

static int l_border(lua_State *L) {
	border(0, 0, 0, 0, 0, 0, 0, 0);
	return 0;
}

static int l_box(lua_State *L) {
	chtype v = (chtype)luaL_optinteger(L, 1, 0);
	chtype h = (chtype)luaL_optinteger(L, 2, 0);
	lua_pushinteger(L, box(stdscr, v, h));
	return 1;
}

static int l_hline(lua_State *L) {
	chtype ch = (chtype)luaL_checkinteger(L, 1);
	int n = luaL_checkinteger(L, 2);
	lua_pushinteger(L, hline(ch, n));
	return 1;
}

static int l_vline(lua_State *L) {
	chtype ch = (chtype)luaL_checkinteger(L, 1);
	int n = luaL_checkinteger(L, 2);
	lua_pushinteger(L, vline(ch, n));
	return 1;
}

static int l_scroll(lua_State *L) {
	lua_pushinteger(L, scroll(stdscr));
	return 1;
}

static int l_scrollok(lua_State *L) {
	int flag = lua_toboolean(L, 1);
	lua_pushinteger(L, scrollok(stdscr, flag));
	return 1;
}

/* ======================== Registration ======================== */

static const luaL_Reg curses_funcs[] = {
	/* screen */
	{"initscr", l_initscr},
	{"endwin", l_endwin},
	{"erase", l_erase},
	{"refresh", l_refresh},
	/* input */
	{"getch", l_getch},
	{"ungetch", l_ungetch},
	{"getstr", l_getstr},
	{"has_key", l_has_key},
	/* cursor */
	{"move", l_move},
	{"getyx", l_getyx},
	{"getmaxyx", l_getmaxyx},
	/* output */
	{"addch", l_addch},
	{"addstr", l_addstr},
	{"printw", l_printw},
	{"mvaddch", l_mvaddch},
	{"mvaddstr", l_mvaddstr},
	{"mvprintw", l_mvprintw},
	{"clrtoeol", l_clrtoeol},
	{"clrtobot", l_clrtobot},
	{"deleteln", l_deleteln},
	{"insertln", l_insertln},
	/* attributes */
	{"attron", l_attron},
	{"attroff", l_attroff},
	{"attrset", l_attrset},
	{"color_pair", l_color_pair},
	/* colors */
	{"start_color", l_start_color},
	{"init_pair", l_init_pair},
	{"pair_content", l_pair_content},
	{"has_colors", l_has_colors},
	{"use_default_colors", l_use_default_colors},
	/* mode */
	{"cbreak", l_cbreak},
	{"nocbreak", l_nocbreak},
	{"echo", l_echo},
	{"noecho", l_noecho},
	{"raw", l_raw},
	{"noraw", l_noraw},
	{"curs_set", l_curs_set},
	{"nodelay", l_nodelay},
	{"timeout", l_timeout},
	{"keypad", l_keypad},
	/* window ops */
	{"border", l_border},
	{"box", l_box},
	{"hline", l_hline},
	{"vline", l_vline},
	{"scroll", l_scroll},
	{"scrollok", l_scrollok},
	{NULL, NULL}
};

void luaopen_curses(lua_State *L) {
	luaL_newlib(L, curses_funcs);
	lua_setglobal(L, "curses");

	/* Screen constants */
	lua_pushinteger(L, COLS);
	lua_setglobal(L, "COLS");
	lua_pushinteger(L, LINES);
	lua_setglobal(L, "LINES");

	/* Return code constants */
	lua_pushinteger(L, OK);
	lua_setglobal(L, "OK");
	lua_pushinteger(L, ERR);
	lua_setglobal(L, "ERR");

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
	lua_pushinteger(L, COLORS);
	lua_setglobal(L, "COLORS");
	lua_pushinteger(L, COLOR_PAIRS);
	lua_setglobal(L, "COLOR_PAIRS");
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
	lua_pushinteger(L, KEY_RESIZE);
	lua_setglobal(L, "KEY_RESIZE");
	lua_pushinteger(L, KEY_F0);
	lua_setglobal(L, "KEY_F0");
	lua_pushinteger(L, KEY_F(1));
	lua_setglobal(L, "KEY_F1");
	lua_pushinteger(L, KEY_F(2));
	lua_setglobal(L, "KEY_F2");
	lua_pushinteger(L, KEY_F(3));
	lua_setglobal(L, "KEY_F3");
	lua_pushinteger(L, KEY_F(4));
	lua_setglobal(L, "KEY_F4");
	lua_pushinteger(L, KEY_F(5));
	lua_setglobal(L, "KEY_F5");
	lua_pushinteger(L, KEY_F(6));
	lua_setglobal(L, "KEY_F6");
	lua_pushinteger(L, KEY_F(7));
	lua_setglobal(L, "KEY_F7");
	lua_pushinteger(L, KEY_F(8));
	lua_setglobal(L, "KEY_F8");
	lua_pushinteger(L, KEY_F(9));
	lua_setglobal(L, "KEY_F9");
	lua_pushinteger(L, KEY_F(10));
	lua_setglobal(L, "KEY_F10");
	lua_pushinteger(L, KEY_F(11));
	lua_setglobal(L, "KEY_F11");
	lua_pushinteger(L, KEY_F(12));
	lua_setglobal(L, "KEY_F12");
}
