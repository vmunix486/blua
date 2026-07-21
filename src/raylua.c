#include <raylib.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* ====== Helpers ====== */
static unsigned char hexval(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return 0;
}

/* Read a Color from a table at stack index `idx`.
 * Accepts: {r, g, b, a} or {r, g, b} (alpha defaults to 255),
 *           "#RGB", "#RRGGBB", "#RGBA", "#RRGGBBAA", or without '#'. */
static Color checkColor(lua_State *L, int idx) {
	if (lua_isstring(L, idx)) {
		const char *hex = lua_tostring(L, idx);
		if (hex[0] == '#') hex++;
		int len = 0;
		while (hex[len]) len++;
		Color c = {0, 0, 0, 255};
		if (len == 3) {
			c.r = hexval(hex[0]) * 17;
			c.g = hexval(hex[1]) * 17;
			c.b = hexval(hex[2]) * 17;
		} else if (len == 4) {
			c.r = hexval(hex[0]) * 17;
			c.g = hexval(hex[1]) * 17;
			c.b = hexval(hex[2]) * 17;
			c.a = hexval(hex[3]) * 17;
		} else if (len == 6) {
			c.r = hexval(hex[0]) * 16 + hexval(hex[1]);
			c.g = hexval(hex[2]) * 16 + hexval(hex[3]);
			c.b = hexval(hex[4]) * 16 + hexval(hex[5]);
		} else if (len == 8) {
			c.r = hexval(hex[0]) * 16 + hexval(hex[1]);
			c.g = hexval(hex[2]) * 16 + hexval(hex[3]);
			c.b = hexval(hex[4]) * 16 + hexval(hex[5]);
			c.a = hexval(hex[6]) * 16 + hexval(hex[7]);
		} else {
			luaL_argerror(L, idx, "invalid hex color string");
		}
		return c;
	}
	if (!lua_istable(L, idx))
		luaL_argerror(L, idx, "expected color table, hex string, or color constant");
	Color c;
	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	lua_rawgeti(L, idx, 3);
	lua_rawgeti(L, idx, 4);
	c.r = (unsigned char)lua_tointeger(L, -4);
	c.g = (unsigned char)lua_tointeger(L, -3);
	c.b = (unsigned char)lua_tointeger(L, -2);
	c.a = lua_isnil(L, -1) ? 255 : (unsigned char)lua_tointeger(L, -1);
	lua_pop(L, 4);
	return c;
}

static void pushColor(lua_State *L, Color c) {
	lua_createtable(L, 4, 0);
	lua_pushinteger(L, c.r);
	lua_rawseti(L, -2, 1);
	lua_pushinteger(L, c.g);
	lua_rawseti(L, -2, 2);
	lua_pushinteger(L, c.b);
	lua_rawseti(L, -2, 3);
	lua_pushinteger(L, c.a);
	lua_rawseti(L, -2, 4);
}

static void registerColors(lua_State *L) {
	struct { const char *name; Color val; } colors[] = {
		{"LIGHTGRAY",  {200, 200, 200, 255}},
		{"GRAY",       {130, 130, 130, 255}},
		{"DARKGRAY",   {80, 80, 80, 255}},
		{"YELLOW",     {253, 249, 0, 255}},
		{"GOLD",       {255, 203, 0, 255}},
		{"ORANGE",     {255, 161, 0, 255}},
		{"PINK",       {255, 109, 194, 255}},
		{"RED",        {230, 41, 55, 255}},
		{"MAROON",     {190, 33, 55, 255}},
		{"GREEN",      {0, 228, 48, 255}},
		{"LIME",       {0, 158, 47, 255}},
		{"DARKGREEN",  {0, 117, 44, 255}},
		{"SKYBLUE",    {102, 191, 255, 255}},
		{"BLUE",       {0, 121, 241, 255}},
		{"DARKBLUE",   {0, 82, 172, 255}},
		{"PURPLE",     {200, 122, 255, 255}},
		{"VIOLET",     {135, 60, 190, 255}},
		{"DARKPURPLE", {112, 31, 126, 255}},
		{"BEIGE",      {211, 176, 131, 255}},
		{"BROWN",      {127, 106, 79, 255}},
		{"DARKBROWN",  {76, 63, 47, 255}},
		{"WHITE",      {255, 255, 255, 255}},
		{"BLACK",      {0, 0, 0, 255}},
		{"BLANK",      {0, 0, 0, 0}},
		{"MAGENTA",    {255, 0, 255, 255}},
		{"RAYWHITE",   {245, 245, 245, 255}},
		{NULL, {0}}
	};
	for (int i = 0; colors[i].name != NULL; i++) {
		pushColor(L, colors[i].val);
		lua_setglobal(L, colors[i].name);
	}
}

/* ====== rcore ====== */
static int l_InitWindow(lua_State *L) {
	int width = luaL_checkinteger(L, 1);
	int height = luaL_checkinteger(L, 2);
	const char *title = luaL_checkstring(L, 3);
	InitWindow(width, height, title);
	return 0;
}

static int l_CloseWindow(lua_State *L) {
	CloseWindow();
	return 0;
}

static int l_WindowShouldClose(lua_State *L) {
	lua_pushboolean(L, WindowShouldClose());
	return 1;
}

static int l_IsWindowReady(lua_State *L) {
	lua_pushboolean(L, IsWindowReady());
	return 1;
}

static int l_IsWindowFullscreen(lua_State *L) {
	lua_pushboolean(L, IsWindowFullscreen());
	return 1;
}

static int l_IsWindowHidden(lua_State *L) {
	lua_pushboolean(L, IsWindowHidden());
	return 1;
}

static int l_IsWindowMinimized(lua_State *L) {
	lua_pushboolean(L, IsWindowMinimized());
	return 1;
}

static int l_IsWindowMaximized(lua_State *L) {
	lua_pushboolean(L, IsWindowMaximized());
	return 1;
}

static int l_IsWindowFocused(lua_State *L) {
	lua_pushboolean(L, IsWindowFocused());
	return 1;
}

static int l_IsWindowResized(lua_State *L) {
	lua_pushboolean(L, IsWindowResized());
	return 1;
}

static int l_IsWindowState(lua_State *L) {
	unsigned int flag = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsWindowState(flag));
	return 1;
}

static int l_SetWindowState(lua_State *L) {
	unsigned int flags = luaL_checkinteger(L, 1);
	SetWindowState(flags);
	return 0;
}

static int l_ClearWindowState(lua_State *L) {
	unsigned int flags = luaL_checkinteger(L, 1);
	ClearWindowState(flags);
	return 0;
}

/*
 *	Drawing-related functions
*/
static int l_ClearBackground(lua_State *L) {
	Color color = checkColor(L, 1);
	ClearBackground(color);
	return 0;
}

static int l_BeginDrawing(lua_State *L) {
	BeginDrawing();
	return 0;
}

static int l_EndDrawing(lua_State *L) {
	EndDrawing();
	return 0;
}

/*
 *	Timing-related functions
*/

static int l_SetTargetFPS(lua_State *L) {
	int fps = luaL_checkinteger(L, 1);
	SetTargetFPS(fps);
	return 0;
}



/* ====== rshapes ======= */

static int l_DrawRectangle(lua_State *L) {
	int posx = luaL_checkinteger(L, 1);
	int posy = luaL_checkinteger(L, 2);
	int width = luaL_checkinteger(L, 3);
	int height = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawRectangle(posx, posy, width, height, color);
	return 0;
}

/* ====== rtext ====== */

static int l_DrawText(lua_State *L) {
	const char *text = luaL_checkstring(L, 1);
	int posx = luaL_checkinteger(L, 2);
	int posy = luaL_checkinteger(L, 3);
	int fontsize = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawText(text, posx, posy, fontsize, color);
	return 0;
}

/* ====== Function Registration ====== */
static const luaL_Reg raylib_funcs[] = {
	/* == rcore == */
	{"InitWindow", l_InitWindow},
	{"CloseWindow", l_CloseWindow},
	{"WindowShouldClose", l_WindowShouldClose},
	{"IsWindowReady", l_IsWindowReady},
	{"IsWindowFullscreen", l_IsWindowFullscreen},
	{"IsWindowHidden", l_IsWindowHidden},
	{"IsWindowMinimized", l_IsWindowMinimized},
	{"IsWindowMaximized", l_IsWindowMaximized},
	{"IsWindowFocused", l_IsWindowFocused},
	{"IsWindowResized", l_IsWindowResized},
	{"IsWindowState", l_IsWindowState},
	{"SetWindowState", l_SetWindowState},
	{"ClearWindowState", l_ClearWindowState},

	/* Drawing funcs */
	{"ClearBackground", l_ClearBackground},
	{"BeginDrawing", l_BeginDrawing},
	{"EndDrawing", l_EndDrawing},

	/* Timing funcs */
	{"SetTargetFPS", l_SetTargetFPS},

	/* Shapes */
	{"DrawRectangle", l_DrawRectangle},

	/* Text */
	{"DrawText", l_DrawText},

	/* vmunix: TODO: add more functions */

	{NULL, NULL}
};

/* ====== Module Initialization ====== */
void luaopen_raylib (lua_State *L) {
	registerColors(L);
	luaL_newlib(L, raylib_funcs);
	lua_setglobal(L, "raylib");
}
