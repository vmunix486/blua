#include <raylib.h>
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

/* ====== rcore ====== */
static int l_InitWindow(lua_State *L) {
	int width = luaL_checkinteger(L, 1);
	int height = luaL_checkinteger(L, 2);
	const char *title = luaL_checkstring(L, 3);
	InitWindow(width, height, title);
	return 0;
}

static int l_CloseWindow() {
	CloseWindow();
	return 0;
}

static int l_IsWindowReady() {
	IsWindowReady();
	return 0;
}

static int l_IsWindowFullscreen() {
	IsWindowFullscreen();
	return 0;
}

static int l_IsWindowHidden() {
	IsWindowHidden();
	return 0;
}

static int l_IsWindowMinimized() {
	IsWindowMinimized();
	return 0;
}

static int l_IsWindowMaximized() {
	IsWindowMaximized();
	return 0;
}

static int l_IsWindowFocused() {
	IsWindowFocused();
	return 0;
}

static int l_IsWindowResized() {
	IsWindowResized();
	return 0;
}

static int l_IsWindowState(lua_State *L) {
	unsigned int flag = luaL_checkinteger(L, 1);
	IsWindowState(flag);
	return 0;
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
	int r = luaL_checkinteger(L, 1);
	int g = luaL_checkinteger(L, 2);
	int b = luaL_checkinteger(L, 3);
	int a = luaL_optinteger(L, 4, 255);
	Color color = { 
			(unsigned char) r,
			(unsigned char) g,
			(unsigned char) b,
			(unsigned char) a,
	};
	ClearBackground(color);
	return 0;
}

static int l_BeginDrawing() {
	BeginDrawing();
	return 0;
}

static int l_EndDrawing() {
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

/* ====== Function Registration ====== */
static const luaL_Reg raylib_funcs[] = {
	/* == rcore == */
	{"InitWindow", l_InitWindow},
	{"CloseWindow", l_CloseWindow},
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

	/* vmunix: TODO: add more functions */

	{NULL, NULL}
};

/* ====== Module Initialization ====== */
void luaopen_raylib (lua_State *L) {
	luaL_newlib(L, raylib_funcs);
	lua_setglobal(L, "raylib");
}
