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

static Vector2 checkVector2(lua_State *L, int idx) {
	Vector2 v = {0, 0};
	if (!lua_istable(L, idx))
		luaL_argerror(L, idx, "expected {x, y} vector table");
	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	v.x = (float)lua_tonumber(L, -2);
	v.y = (float)lua_tonumber(L, -1);
	lua_pop(L, 2);
	return v;
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

#define registerKey(L, k) lua_pushinteger(L, k); lua_setglobal(L, #k)

static void registerKeys(lua_State *L) {
	registerKey(L, KEY_NULL);
	registerKey(L, KEY_APOSTROPHE);
	registerKey(L, KEY_COMMA);
	registerKey(L, KEY_MINUS);
	registerKey(L, KEY_PERIOD);
	registerKey(L, KEY_SLASH);
	registerKey(L, KEY_ZERO);
	registerKey(L, KEY_ONE);
	registerKey(L, KEY_TWO);
	registerKey(L, KEY_THREE);
	registerKey(L, KEY_FOUR);
	registerKey(L, KEY_FIVE);
	registerKey(L, KEY_SIX);
	registerKey(L, KEY_SEVEN);
	registerKey(L, KEY_EIGHT);
	registerKey(L, KEY_NINE);
	registerKey(L, KEY_SEMICOLON);
	registerKey(L, KEY_EQUAL);
	registerKey(L, KEY_A);
	registerKey(L, KEY_B);
	registerKey(L, KEY_C);
	registerKey(L, KEY_D);
	registerKey(L, KEY_E);
	registerKey(L, KEY_F);
	registerKey(L, KEY_G);
	registerKey(L, KEY_H);
	registerKey(L, KEY_I);
	registerKey(L, KEY_J);
	registerKey(L, KEY_K);
	registerKey(L, KEY_L);
	registerKey(L, KEY_M);
	registerKey(L, KEY_N);
	registerKey(L, KEY_O);
	registerKey(L, KEY_P);
	registerKey(L, KEY_Q);
	registerKey(L, KEY_R);
	registerKey(L, KEY_S);
	registerKey(L, KEY_T);
	registerKey(L, KEY_U);
	registerKey(L, KEY_V);
	registerKey(L, KEY_W);
	registerKey(L, KEY_X);
	registerKey(L, KEY_Y);
	registerKey(L, KEY_Z);
	registerKey(L, KEY_LEFT_BRACKET);
	registerKey(L, KEY_BACKSLASH);
	registerKey(L, KEY_RIGHT_BRACKET);
	registerKey(L, KEY_GRAVE);
	registerKey(L, KEY_SPACE);
	registerKey(L, KEY_ESCAPE);
	registerKey(L, KEY_ENTER);
	registerKey(L, KEY_TAB);
	registerKey(L, KEY_BACKSPACE);
	registerKey(L, KEY_INSERT);
	registerKey(L, KEY_DELETE);
	registerKey(L, KEY_RIGHT);
	registerKey(L, KEY_LEFT);
	registerKey(L, KEY_DOWN);
	registerKey(L, KEY_UP);
	registerKey(L, KEY_PAGE_UP);
	registerKey(L, KEY_PAGE_DOWN);
	registerKey(L, KEY_HOME);
	registerKey(L, KEY_END);
	registerKey(L, KEY_CAPS_LOCK);
	registerKey(L, KEY_SCROLL_LOCK);
	registerKey(L, KEY_NUM_LOCK);
	registerKey(L, KEY_PRINT_SCREEN);
	registerKey(L, KEY_PAUSE);
	registerKey(L, KEY_F1);
	registerKey(L, KEY_F2);
	registerKey(L, KEY_F3);
	registerKey(L, KEY_F4);
	registerKey(L, KEY_F5);
	registerKey(L, KEY_F6);
	registerKey(L, KEY_F7);
	registerKey(L, KEY_F8);
	registerKey(L, KEY_F9);
	registerKey(L, KEY_F10);
	registerKey(L, KEY_F11);
	registerKey(L, KEY_F12);
	registerKey(L, KEY_LEFT_SHIFT);
	registerKey(L, KEY_LEFT_CONTROL);
	registerKey(L, KEY_LEFT_ALT);
	registerKey(L, KEY_LEFT_SUPER);
	registerKey(L, KEY_RIGHT_SHIFT);
	registerKey(L, KEY_RIGHT_CONTROL);
	registerKey(L, KEY_RIGHT_ALT);
	registerKey(L, KEY_RIGHT_SUPER);
	registerKey(L, KEY_KB_MENU);
	registerKey(L, KEY_KP_0);
	registerKey(L, KEY_KP_1);
	registerKey(L, KEY_KP_2);
	registerKey(L, KEY_KP_3);
	registerKey(L, KEY_KP_4);
	registerKey(L, KEY_KP_5);
	registerKey(L, KEY_KP_6);
	registerKey(L, KEY_KP_7);
	registerKey(L, KEY_KP_8);
	registerKey(L, KEY_KP_9);
	registerKey(L, KEY_KP_DECIMAL);
	registerKey(L, KEY_KP_DIVIDE);
	registerKey(L, KEY_KP_MULTIPLY);
	registerKey(L, KEY_KP_SUBTRACT);
	registerKey(L, KEY_KP_ADD);
	registerKey(L, KEY_KP_ENTER);
	registerKey(L, KEY_KP_EQUAL);
	registerKey(L, KEY_BACK);
	registerKey(L, KEY_MENU);
	registerKey(L, KEY_VOLUME_UP);
	registerKey(L, KEY_VOLUME_DOWN);

	registerKey(L, MOUSE_BUTTON_LEFT);
	registerKey(L, MOUSE_BUTTON_RIGHT);
	registerKey(L, MOUSE_BUTTON_MIDDLE);
	registerKey(L, MOUSE_BUTTON_SIDE);
	registerKey(L, MOUSE_BUTTON_EXTRA);
	registerKey(L, MOUSE_BUTTON_FORWARD);
	registerKey(L, MOUSE_BUTTON_BACK);

	registerKey(L, FLAG_VSYNC_HINT);
	registerKey(L, FLAG_FULLSCREEN_MODE);
	registerKey(L, FLAG_WINDOW_RESIZABLE);
	registerKey(L, FLAG_WINDOW_UNDECORATED);
	registerKey(L, FLAG_WINDOW_HIDDEN);
	registerKey(L, FLAG_WINDOW_MINIMIZED);
	registerKey(L, FLAG_WINDOW_MAXIMIZED);
	registerKey(L, FLAG_WINDOW_UNFOCUSED);
	registerKey(L, FLAG_WINDOW_TOPMOST);
	registerKey(L, FLAG_WINDOW_ALWAYS_RUN);
	registerKey(L, FLAG_WINDOW_TRANSPARENT);
	registerKey(L, FLAG_WINDOW_HIGHDPI);
	registerKey(L, FLAG_WINDOW_MOUSE_PASSTHROUGH);
	registerKey(L, FLAG_BORDERLESS_WINDOWED_MODE);
	registerKey(L, FLAG_MSAA_4X_HINT);
	registerKey(L, FLAG_INTERLACED_HINT);
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

static int l_ToggleFullscreen(lua_State *L) {
	ToggleFullscreen();
	return 0;
}

static int l_GetScreenWidth(lua_State *L) {
	lua_pushinteger(L, GetScreenWidth());
	return 1;
}

static int l_GetScreenHeight(lua_State *L) {
	lua_pushinteger(L, GetScreenHeight());
	return 1;
}

/*
 *	Cursor-related functions
*/
static int l_ShowCursor() {
	ShowCursor();
	return 0;
}

static int l_HideCursor() {
	HideCursor();
	return 0;
}

static int l_IsCursorHidden(lua_State *L) {
        lua_pushboolean(L, IsCursorHidden());
	return 1;
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

/*
 *	Misc. functions
*/

static int l_SetConfigFlags(lua_State *L) {
	unsigned int flags = (unsigned int)luaL_checkinteger(L, 1);
	SetConfigFlags(flags);
	return 0;
}

/* 
 * Input Handline functions
*/

static int l_IsKeyPressed(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsKeyPressed(key));
	return 1;
}

static int l_IsKeyDown(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsKeyDown(key));
	return 1;
}


static int l_GetMouseX(lua_State *L) {
	lua_pushinteger(L, GetMouseX());
	return 1;
}

static int l_GetMouseY(lua_State *L) {
	lua_pushinteger(L, GetMouseY());
	return 1;
}

static int l_IsMouseButtonPressed(lua_State *L) {
	int button = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsMouseButtonPressed(button));
	return 1;
}

/* ====== rshapes ======= */

static int l_DrawPixel(lua_State *L) {
	int posX = luaL_checkinteger(L, 1);
	int posY = luaL_checkinteger(L, 2);
	Color color = checkColor(L, 3);
	DrawPixel(posX, posY, color);
	return 0;
}

static int l_DrawLine(lua_State *L) {
	int startPosX = luaL_checkinteger(L, 1);
	int startPosY = luaL_checkinteger(L, 2);
	int endPosX = luaL_checkinteger(L, 3);
	int endPosY = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawLine(startPosX, startPosY, endPosX, endPosY, color);
	return 0;
}

static int l_DrawCircle(lua_State *L) {
	int centerX = luaL_checkinteger(L, 1);
	int centerY = luaL_checkinteger(L, 2);
	float radius = (float)luaL_checknumber(L, 3);
	Color color = checkColor(L, 4);
	DrawCircle(centerX, centerY, radius, color);
	return 0;
}

static int l_DrawCircleV(lua_State *L) {
	Vector2 center = checkVector2(L, 1);
	float radius = (float)luaL_checknumber(L, 2);
	Color color = checkColor(L, 3);
	DrawCircleV(center, radius, color);
	return 0;
}

static int l_DrawCircleGradient(lua_State *L) {
	Vector2 center = checkVector2(L, 1);
	float radius = (float)luaL_checknumber(L, 2);
	Color inner = checkColor(L, 3);
	Color outer = checkColor(L, 4);
	DrawCircleGradient(center, radius, inner, outer);
	return 0;
}

static int l_DrawCircleLines(lua_State *L) {
	int centerX = luaL_checkinteger(L, 1);
	int centerY = luaL_checkinteger(L, 2);
	float radius = (float)luaL_checknumber(L, 3);
	Color color = checkColor(L, 4);
	DrawCircleLines(centerX, centerY, radius, color);
	return 0;
}

static int l_DrawEllipse(lua_State *L) {
	int centerX = luaL_checkinteger(L, 1);
	int centerY = luaL_checkinteger(L, 2);
	float radiusH = (float)luaL_checknumber(L, 3);
	float radiusV = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawEllipse(centerX, centerY, radiusH, radiusV, color);
	return 0;
}

static int l_DrawEllipseLines(lua_State *L) {
	int centerX = luaL_checkinteger(L, 1);
	int centerY = luaL_checkinteger(L, 2);
	float radiusH = (float)luaL_checknumber(L, 3);
	float radiusV = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawEllipseLines(centerX, centerY, radiusH, radiusV, color);
	return 0;
}

static int l_DrawRectangle(lua_State *L) {
	int posx = luaL_checkinteger(L, 1);
	int posy = luaL_checkinteger(L, 2);
	int width = luaL_checkinteger(L, 3);
	int height = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawRectangle(posx, posy, width, height, color);
	return 0;
}

static int l_DrawRectangleGradientV(lua_State *L) {
	int posx = luaL_checkinteger(L, 1);
	int posy = luaL_checkinteger(L, 2);
	int width = luaL_checkinteger(L, 3);
	int height = luaL_checkinteger(L, 4);
	Color top = checkColor(L, 5);
	Color bottom = checkColor(L, 6);
	DrawRectangleGradientV(posx, posy, width, height, top, bottom);
	return 0;
}

static int l_DrawRectangleGradientH(lua_State *L) {
	int posx = luaL_checkinteger(L, 1);
	int posy = luaL_checkinteger(L, 2);
	int width = luaL_checkinteger(L, 3);
	int height = luaL_checkinteger(L, 4);
	Color left = checkColor(L, 5);
	Color right = checkColor(L, 6);
	DrawRectangleGradientH(posx, posy, width, height, left, right);
	return 0;
}

static int l_DrawRectangleLines(lua_State *L) {
	int posx = luaL_checkinteger(L, 1);
	int posy = luaL_checkinteger(L, 2);
	int width = luaL_checkinteger(L, 3);
	int height = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawRectangleLines(posx, posy, width, height, color);
	return 0;
}

static int l_DrawTriangle(lua_State *L) {
	Vector2 v1 = checkVector2(L, 1);
	Vector2 v2 = checkVector2(L, 2);
	Vector2 v3 = checkVector2(L, 3);
	Color color = checkColor(L, 4);
	DrawTriangle(v1, v2, v3, color);
	return 0;
}

static int l_DrawTriangleLines(lua_State *L) {
	Vector2 v1 = checkVector2(L, 1);
	Vector2 v2 = checkVector2(L, 2);
	Vector2 v3 = checkVector2(L, 3);
	Color color = checkColor(L, 4);
	DrawTriangleLines(v1, v2, v3, color);
	return 0;
}

static int l_DrawPoly(lua_State *L) {
	Vector2 center = checkVector2(L, 1);
	int sides = luaL_checkinteger(L, 2);
	float radius = (float)luaL_checknumber(L, 3);
	float rotation = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawPoly(center, sides, radius, rotation, color);
	return 0;
}

static int l_DrawPolyLines(lua_State *L) {
	Vector2 center = checkVector2(L, 1);
	int sides = luaL_checkinteger(L, 2);
	float radius = (float)luaL_checknumber(L, 3);
	float rotation = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawPolyLines(center, sides, radius, rotation, color);
	return 0;
}

static int l_DrawPolyLinesEx(lua_State *L) {
	Vector2 center = checkVector2(L, 1);
	int sides = luaL_checkinteger(L, 2);
	float radius = (float)luaL_checknumber(L, 3);
	float rotation = (float)luaL_checknumber(L, 4);
	float lineThick = (float)luaL_checknumber(L, 5);
	Color color = checkColor(L, 6);
	DrawPolyLinesEx(center, sides, radius, rotation, lineThick, color);
	return 0;
}

/* ====== rtext ====== */

static int l_DrawFPS(lua_State *L) {
	int posX = luaL_checkinteger(L, 1);
	int posY = luaL_checkinteger(L, 2);
	DrawFPS(posX, posY);
	return 0; 
}

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
	{"ToggleFullscreen", l_ToggleFullscreen},
	{"GetScreenWidth", l_GetScreenWidth},
	{"GetScreenHeight", l_GetScreenHeight},

	/* Cursor funcs */
	{"ShowCursor", l_ShowCursor},
	{"HideCursor", l_HideCursor},
	{"IsCursorHidden", l_IsCursorHidden},

	/* Drawing funcs */
	{"ClearBackground", l_ClearBackground},
	{"BeginDrawing", l_BeginDrawing},
	{"EndDrawing", l_EndDrawing},

	/* Timing funcs */
	{"SetTargetFPS", l_SetTargetFPS},

	/* Misc funcs */
	{"SetConfigFlags", l_SetConfigFlags},

	/* Input funcs */
	{"IsKeyPressed", l_IsKeyPressed},
	{"IsKeyDown", l_IsKeyDown},
	{"GetMouseX", l_GetMouseX},
	{"GetMouseY", l_GetMouseY},
	{"IsMouseButtonPressed", l_IsMouseButtonPressed},

	/* Shapes */
	{"DrawPixel", l_DrawPixel},
	{"DrawLine", l_DrawLine},
	{"DrawCircle", l_DrawCircle},
	{"DrawCircleV", l_DrawCircleV},
	{"DrawCircleGradient", l_DrawCircleGradient},
	{"DrawCircleLines", l_DrawCircleLines},
	{"DrawEllipse", l_DrawEllipse},
	{"DrawEllipseLines", l_DrawEllipseLines},
	{"DrawRectangle", l_DrawRectangle},
	{"DrawRectangleGradientV", l_DrawRectangleGradientV},
	{"DrawRectangleGradientH", l_DrawRectangleGradientH},
	{"DrawRectangleLines", l_DrawRectangleLines},
	{"DrawTriangle", l_DrawTriangle},
	{"DrawTriangleLines", l_DrawTriangleLines},
	{"DrawPoly", l_DrawPoly},
	{"DrawPolyLines", l_DrawPolyLines},
	{"DrawPolyLinesEx", l_DrawPolyLinesEx},

	/* Text */
	{"DrawFPS", l_DrawFPS},
	{"DrawText", l_DrawText},

	/* vmunix: TODO: add more functions */

	{NULL, NULL}
};

/* ====== Module Initialization ====== */
void luaopen_raylib (lua_State *L) {
	registerColors(L);
	registerKeys(L);
	luaL_newlib(L, raylib_funcs);
	lua_setglobal(L, "raylib");
}
