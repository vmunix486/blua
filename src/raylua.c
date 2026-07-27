#include <raylib.h>
#include <string.h>
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

static Vector3 checkVector3(lua_State *L, int idx) {
	Vector3 v = {0, 0, 0};
	if (!lua_istable(L, idx))
		luaL_argerror(L, idx, "expected {x, y, z} vector table");
	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	lua_rawgeti(L, idx, 3);
	v.x = (float)lua_tonumber(L, -3);
	v.y = (float)lua_tonumber(L, -2);
	v.z = (float)lua_tonumber(L, -1);
	lua_pop(L, 3);
	return v;
}

static Vector4 checkVector4(lua_State *L, int idx) {
	Vector4 v = {0, 0, 0, 0};
	if (!lua_istable(L, idx))
		luaL_argerror(L, idx, "expected {x, y, z, w} vector table");
	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	lua_rawgeti(L, idx, 3);
	lua_rawgeti(L, idx, 4);
	v.x = (float)lua_tonumber(L, -4);
	v.y = (float)lua_tonumber(L, -3);
	v.z = (float)lua_tonumber(L, -2);
	v.w = (float)lua_tonumber(L, -1);
	lua_pop(L, 4);
	return v;
}

static Rectangle checkRectangle(lua_State *L, int idx) {
	Rectangle r = {0, 0, 0, 0};
	if (!lua_istable(L, idx))
		luaL_argerror(L, idx, "expected {x, y, width, height} rectangle table");
	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	lua_rawgeti(L, idx, 3);
	lua_rawgeti(L, idx, 4);
	r.x = (float)lua_tonumber(L, -4);
	r.y = (float)lua_tonumber(L, -3);
	r.width = (float)lua_tonumber(L, -2);
	r.height = (float)lua_tonumber(L, -1);
	lua_pop(L, 4);
	return r;
}

static void pushVector2(lua_State *L, Vector2 v) {
	lua_createtable(L, 2, 0);
	lua_pushnumber(L, v.x);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, v.y);
	lua_rawseti(L, -2, 2);
}

static void pushVector3(lua_State *L, Vector3 v) {
	lua_createtable(L, 3, 0);
	lua_pushnumber(L, v.x);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, v.y);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, v.z);
	lua_rawseti(L, -2, 3);
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

static void pushRectangle(lua_State *L, Rectangle r) {
	lua_createtable(L, 4, 0);
	lua_pushnumber(L, r.x);
	lua_rawseti(L, -2, 1);
	lua_pushnumber(L, r.y);
	lua_rawseti(L, -2, 2);
	lua_pushnumber(L, r.width);
	lua_rawseti(L, -2, 3);
	lua_pushnumber(L, r.height);
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

	/* Blend modes */
	registerKey(L, BLEND_ALPHA);
	registerKey(L, BLEND_ADDITIVE);
	registerKey(L, BLEND_MULTIPLIED);
	registerKey(L, BLEND_ADD_COLORS);
	registerKey(L, BLEND_SUBTRACT_COLORS);
	registerKey(L, BLEND_ALPHA_PREMULTIPLY);
	registerKey(L, BLEND_CUSTOM);
	registerKey(L, BLEND_CUSTOM_SEPARATE);

	/* Shader uniform locations */
	registerKey(L, SHADER_LOC_VERTEX_POSITION);
	registerKey(L, SHADER_LOC_VERTEX_TEXCOORD01);
	registerKey(L, SHADER_LOC_VERTEX_TEXCOORD02);
	registerKey(L, SHADER_LOC_VERTEX_NORMAL);
	registerKey(L, SHADER_LOC_VERTEX_TANGENT);
	registerKey(L, SHADER_LOC_VERTEX_COLOR);
	registerKey(L, SHADER_LOC_MATRIX_MVP);
	registerKey(L, SHADER_LOC_MATRIX_VIEW);
	registerKey(L, SHADER_LOC_MATRIX_PROJECTION);
	registerKey(L, SHADER_LOC_MATRIX_MODEL);
	registerKey(L, SHADER_LOC_MATRIX_NORMAL);
	registerKey(L, SHADER_LOC_MAP_ALBEDO);
	registerKey(L, SHADER_LOC_MAP_METALNESS);
	registerKey(L, SHADER_LOC_MAP_NORMAL);
	registerKey(L, SHADER_LOC_MAP_ROUGHNESS);
	registerKey(L, SHADER_LOC_MAP_OCCLUSION);
	registerKey(L, SHADER_LOC_MAP_EMISSION);
	registerKey(L, SHADER_LOC_MAP_HEIGHT);
	registerKey(L, SHADER_LOC_MAP_CUBEMAP);
	registerKey(L, SHADER_LOC_MAP_IRRADIANCE);
	registerKey(L, SHADER_LOC_MAP_PREFILTER);
	registerKey(L, SHADER_LOC_MAP_BRDF);

	/* Shader uniform types */
	registerKey(L, SHADER_UNIFORM_FLOAT);
	registerKey(L, SHADER_UNIFORM_VEC2);
	registerKey(L, SHADER_UNIFORM_VEC3);
	registerKey(L, SHADER_UNIFORM_VEC4);
	registerKey(L, SHADER_UNIFORM_INT);
	registerKey(L, SHADER_UNIFORM_IVEC2);
	registerKey(L, SHADER_UNIFORM_IVEC3);
	registerKey(L, SHADER_UNIFORM_IVEC4);
	registerKey(L, SHADER_UNIFORM_UINT);
	registerKey(L, SHADER_UNIFORM_UIVEC2);
	registerKey(L, SHADER_UNIFORM_UIVEC3);
	registerKey(L, SHADER_UNIFORM_UIVEC4);
	registerKey(L, SHADER_UNIFORM_SAMPLER2D);

	/* Shader attribute types */
	registerKey(L, SHADER_ATTRIB_FLOAT);
	registerKey(L, SHADER_ATTRIB_VEC2);
	registerKey(L, SHADER_ATTRIB_VEC3);
	registerKey(L, SHADER_ATTRIB_VEC4);

	/* Camera modes */
	registerKey(L, CAMERA_CUSTOM);
	registerKey(L, CAMERA_FREE);
	registerKey(L, CAMERA_ORBITAL);
	registerKey(L, CAMERA_FIRST_PERSON);
	registerKey(L, CAMERA_THIRD_PERSON);

	/* Camera projection */
	registerKey(L, CAMERA_PERSPECTIVE);
	registerKey(L, CAMERA_ORTHOGRAPHIC);
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
	(void)L;
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
	(void)L;
	ToggleFullscreen();
	return 0;
}

static int l_SetWindowTitle(lua_State *L) {
	const char *title = luaL_checkstring(L, 1);
	SetWindowTitle(title);
	return 0;
}

static int l_SetWindowPosition(lua_State *L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	SetWindowPosition(x, y);
	return 0;
}

static int l_SetWindowMinSize(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	SetWindowMinSize(w, h);
	return 0;
}

static int l_SetWindowSize(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	SetWindowSize(w, h);
	return 0;
}

static int l_GetWindowPosition(lua_State *L) {
	Vector2 pos = GetWindowPosition();
	pushVector2(L, pos);
	return 1;
}

static int l_GetWindowScaleDPI(lua_State *L) {
	Vector2 dpi = GetWindowScaleDPI();
	pushVector2(L, dpi);
	return 1;
}

static int l_GetScreenWidth(lua_State *L) {
	lua_pushinteger(L, GetScreenWidth());
	return 1;
}

static int l_GetScreenHeight(lua_State *L) {
	lua_pushinteger(L, GetScreenHeight());
	return 1;
}

static int l_GetRenderWidth(lua_State *L) {
	lua_pushinteger(L, GetRenderWidth());
	return 1;
}

static int l_GetRenderHeight(lua_State *L) {
	lua_pushinteger(L, GetRenderHeight());
	return 1;
}

static int l_GetTime(lua_State *L) {
	lua_pushnumber(L, GetTime());
	return 1;
}

static int l_GetFrameTime(lua_State *L) {
	lua_pushnumber(L, GetFrameTime());
	return 1;
}

static int l_GetFPS(lua_State *L) {
	lua_pushinteger(L, GetFPS());
	return 1;
}

/* ====== Cursor ====== */
static int l_ShowCursor(lua_State *L) {
	(void)L;
	ShowCursor();
	return 0;
}

static int l_HideCursor(lua_State *L) {
	(void)L;
	HideCursor();
	return 0;
}

static int l_DisableCursor(lua_State *L) {
	(void)L;
	DisableCursor();
	return 0;
}

static int l_EnableCursor(lua_State *L) {
	(void)L;
	EnableCursor();
	return 0;
}

static int l_IsCursorHidden(lua_State *L) {
	lua_pushboolean(L, IsCursorHidden());
	return 1;
}

static int l_IsCursorOnScreen(lua_State *L) {
	lua_pushboolean(L, IsCursorOnScreen());
	return 1;
}

static int l_SetMousePosition(lua_State *L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	SetMousePosition(x, y);
	return 0;
}

static int l_SetMouseOffset(lua_State *L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	SetMouseOffset(x, y);
	return 0;
}

static int l_SetMouseScale(lua_State *L) {
	float sx = (float)luaL_checknumber(L, 1);
	float sy = (float)luaL_checknumber(L, 2);
	SetMouseScale(sx, sy);
	return 0;
}

static int l_GetMousePosition(lua_State *L) {
	Vector2 pos = GetMousePosition();
	pushVector2(L, pos);
	return 1;
}

static int l_GetMouseDelta(lua_State *L) {
	Vector2 delta = GetMouseDelta();
	pushVector2(L, delta);
	return 1;
}

static int l_GetMouseWheelMove(lua_State *L) {
	lua_pushnumber(L, GetMouseWheelMove());
	return 1;
}

/* ====== Touch ====== */
static int l_GetTouchX(lua_State *L) {
	lua_pushinteger(L, GetTouchX());
	return 1;
}

static int l_GetTouchY(lua_State *L) {
	lua_pushinteger(L, GetTouchY());
	return 1;
}

static int l_GetTouchPosition(lua_State *L) {
	int id = luaL_checkinteger(L, 1);
	Vector2 pos = GetTouchPosition(id);
	pushVector2(L, pos);
	return 1;
}

static int l_GetTouchPointCount(lua_State *L) {
	lua_pushinteger(L, GetTouchPointCount());
	return 1;
}

/* ====== Gamepad ====== */
static int l_IsGamepadAvailable(lua_State *L) {
	int gamepad = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsGamepadAvailable(gamepad));
	return 1;
}

static int l_GetGamepadName(lua_State *L) {
	int gamepad = luaL_checkinteger(L, 1);
	const char *name = GetGamepadName(gamepad);
	lua_pushstring(L, name ? name : "");
	return 1;
}

static int l_IsGamepadButtonPressed(lua_State *L) {
	int gamepad = luaL_checkinteger(L, 1);
	int button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, IsGamepadButtonPressed(gamepad, button));
	return 1;
}

static int l_IsGamepadButtonDown(lua_State *L) {
	int gamepad = luaL_checkinteger(L, 1);
	int button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, IsGamepadButtonDown(gamepad, button));
	return 1;
}

static int l_IsGamepadButtonReleased(lua_State *L) {
	int gamepad = luaL_checkinteger(L, 1);
	int button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, IsGamepadButtonReleased(gamepad, button));
	return 1;
}

static int l_IsGamepadButtonUp(lua_State *L) {
	int gamepad = luaL_checkinteger(L, 1);
	int button = luaL_checkinteger(L, 2);
	lua_pushboolean(L, IsGamepadButtonUp(gamepad, button));
	return 1;
}

static int l_GetGamepadAxisMovement(lua_State *L) {
	int gamepad = luaL_checkinteger(L, 1);
	int axis = luaL_checkinteger(L, 2);
	lua_pushnumber(L, GetGamepadAxisMovement(gamepad, axis));
	return 1;
}

/* ====== Key state helpers ====== */
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

static int l_IsKeyUp(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsKeyUp(key));
	return 1;
}

static int l_IsKeyReleased(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsKeyReleased(key));
	return 1;
}

static int l_IsKeyPressedRepeat(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsKeyPressedRepeat(key));
	return 1;
}

static int l_GetKeyPressed(lua_State *L) {
	lua_pushinteger(L, GetKeyPressed());
	return 1;
}

static int l_GetCharPressed(lua_State *L) {
	lua_pushinteger(L, GetCharPressed());
	return 1;
}

static int l_SetExitKey(lua_State *L) {
	int key = luaL_checkinteger(L, 1);
	SetExitKey(key);
	return 0;
}

/* ====== MouseButton state helpers ====== */
static int l_IsMouseButtonPressed(lua_State *L) {
	int button = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsMouseButtonPressed(button));
	return 1;
}

static int l_IsMouseButtonUp(lua_State *L) {
	int button = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsMouseButtonUp(button));
	return 1;
}

static int l_IsMouseButtonReleased(lua_State *L) {
	int button = luaL_checkinteger(L, 1);
	lua_pushboolean(L, IsMouseButtonReleased(button));
	return 1;
}

/* ====== Drawing ====== */
static int l_ClearBackground(lua_State *L) {
	Color color = checkColor(L, 1);
	ClearBackground(color);
	return 0;
}

static int l_BeginDrawing(lua_State *L) {
	(void)L;
	BeginDrawing();
	return 0;
}

static int l_EndDrawing(lua_State *L) {
	(void)L;
	EndDrawing();
	return 0;
}

static int l_BeginMode2D(lua_State *L) {
	float ox = (float)luaL_checknumber(L, 1);
	float oy = (float)luaL_checknumber(L, 2);
	float tx = (float)luaL_checknumber(L, 3);
	float ty = (float)luaL_checknumber(L, 4);
	float rotation = (float)luaL_checknumber(L, 5);
	float zoom = (float)luaL_checknumber(L, 6);
	Camera2D cam = {{ox, oy}, {tx, ty}, rotation, zoom};
	BeginMode2D(cam);
	return 0;
}

static int l_EndMode2D(lua_State *L) {
	(void)L;
	EndMode2D();
	return 0;
}

static int l_BeginMode3D(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected camera table {position={x,y,z}, target={x,y,z}, up={x,y,z}, fov, projection}");
	Camera3D cam;
	lua_rawgeti(L, 1, 1); /* position */
	cam.position = checkVector3(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 2); /* target */
	cam.target = checkVector3(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 3); /* up */
	cam.up = checkVector3(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 4); /* fov */
	cam.fovy = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 5); /* projection */
	cam.projection = (int)lua_tointeger(L, -1); lua_pop(L, 1);
	BeginMode3D(cam);
	return 0;
}

static int l_EndMode3D(lua_State *L) {
	(void)L;
	EndMode3D();
	return 0;
}

static int l_BeginTextureMode(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected render texture table");
	RenderTexture2D rt;
	lua_rawgeti(L, 1, 1); rt.id = (unsigned int)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 2); rt.texture.width = (int)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 3); rt.texture.height = (int)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 4); rt.texture.mipmaps = (int)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 5); rt.texture.format = (int)lua_tointeger(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 6); rt.depth.id = (unsigned int)lua_tointeger(L, -1); lua_pop(L, 1);
	BeginTextureMode(rt);
	return 0;
}

static int l_EndTextureMode(lua_State *L) {
	(void)L;
	EndTextureMode();
	return 0;
}

static int l_BeginScissorMode(lua_State *L) {
	int x = luaL_checkinteger(L, 1);
	int y = luaL_checkinteger(L, 2);
	int w = luaL_checkinteger(L, 3);
	int h = luaL_checkinteger(L, 4);
	BeginScissorMode(x, y, w, h);
	return 0;
}

static int l_EndScissorMode(lua_State *L) {
	(void)L;
	EndScissorMode();
	return 0;
}

static int l_BeginBlendMode(lua_State *L) {
	int mode = luaL_checkinteger(L, 1);
	BeginBlendMode(mode);
	return 0;
}

static int l_EndBlendMode(lua_State *L) {
	(void)L;
	EndBlendMode();
	return 0;
}

/* ====== Timing ====== */
static int l_SetTargetFPS(lua_State *L) {
	int fps = luaL_checkinteger(L, 1);
	SetTargetFPS(fps);
	return 0;
}

/* ====== Misc ====== */
static int l_SetConfigFlags(lua_State *L) {
	unsigned int flags = (unsigned int)luaL_checkinteger(L, 1);
	SetConfigFlags(flags);
	return 0;
}

static int l_TraceLog(lua_State *L) {
	int logLevel = luaL_checkinteger(L, 1);
	const char *text = luaL_checkstring(L, 2);
	TraceLog(logLevel, text);
	return 0;
}

static int l_TakeScreenshot(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	TakeScreenshot(filename);
	return 0;
}

static int l_SetRandomSeed(lua_State *L) {
	unsigned int seed = (unsigned int)luaL_checkinteger(L, 1);
	SetRandomSeed(seed);
	return 0;
}

static int l_GetRandomValue(lua_State *L) {
	int min = luaL_checkinteger(L, 1);
	int max = luaL_checkinteger(L, 2);
	lua_pushinteger(L, GetRandomValue(min, max));
	return 1;
}

static int l_TextFormat(lua_State *L) {
	const char *text = luaL_checkstring(L, 1);
	/* TextFormat is variadic; for simplicity just return the string as-is
	   when no extra args, or use TraceLog-style formatting */
	lua_pushstring(L, text);
	return 1;
}

static int l_UpdateCamera(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected camera table");
	int mode = luaL_checkinteger(L, 2);

	/* Read camera from table */
	Camera cam = {0};
	lua_rawgeti(L, 1, 1); cam.position = checkVector3(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 2); cam.target = checkVector3(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 3); cam.up = checkVector3(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 4); cam.fovy = (float)lua_tonumber(L, -1); lua_pop(L, 1);
	lua_rawgeti(L, 1, 5); cam.projection = (int)lua_tointeger(L, -1); lua_pop(L, 1);

	UpdateCamera(&cam, mode);

	/* Write back to table */
	pushVector3(L, cam.position); lua_rawseti(L, 1, 1);
	pushVector3(L, cam.target); lua_rawseti(L, 1, 2);
	pushVector3(L, cam.up); lua_rawseti(L, 1, 3);
	lua_pushnumber(L, cam.fovy); lua_rawseti(L, 1, 4);
	lua_pushinteger(L, cam.projection); lua_rawseti(L, 1, 5);
	return 0;
}

/* ====== rshapes ====== */
static int l_DrawPixel(lua_State *L) {
	int posX = luaL_checkinteger(L, 1);
	int posY = luaL_checkinteger(L, 2);
	Color color = checkColor(L, 3);
	DrawPixel(posX, posY, color);
	return 0;
}

static int l_DrawPixelV(lua_State *L) {
	Vector2 pos = checkVector2(L, 1);
	Color color = checkColor(L, 2);
	DrawPixelV(pos, color);
	return 0;
}

static int l_DrawLine(lua_State *L) {
	int startX = luaL_checkinteger(L, 1);
	int startY = luaL_checkinteger(L, 2);
	int endX = luaL_checkinteger(L, 3);
	int endY = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawLine(startX, startY, endX, endY, color);
	return 0;
}

static int l_DrawLineV(lua_State *L) {
	Vector2 start = checkVector2(L, 1);
	Vector2 end = checkVector2(L, 2);
	Color color = checkColor(L, 3);
	DrawLineV(start, end, color);
	return 0;
}

static int l_DrawLineEx(lua_State *L) {
	Vector2 start = checkVector2(L, 1);
	Vector2 end = checkVector2(L, 2);
	float thick = (float)luaL_checknumber(L, 3);
	Color color = checkColor(L, 4);
	DrawLineEx(start, end, thick, color);
	return 0;
}

static int l_DrawLineBezier(lua_State *L) {
	Vector2 start = checkVector2(L, 1);
	Vector2 end = checkVector2(L, 2);
	float thick = (float)luaL_checknumber(L, 3);
	Color color = checkColor(L, 4);
	DrawLineBezier(start, end, thick, color);
	return 0;
}

static int l_DrawLineStrip(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected table of {x,y} points");
	int count = (int)luaL_len(L, 1);
	Vector2 *points = (Vector2 *)lua_newuserdatauv(L, sizeof(Vector2) * count, 0);
	for (int i = 0; i < count; i++) {
		lua_rawgeti(L, 1, i + 1);
		points[i] = checkVector2(L, -1);
		lua_pop(L, 1);
	}
	Color color = checkColor(L, 2);
	DrawLineStrip(points, count, color);
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

static int l_DrawCircleLinesV(lua_State *L) {
	Vector2 center = checkVector2(L, 1);
	float radius = (float)luaL_checknumber(L, 2);
	Color color = checkColor(L, 3);
	DrawCircleLinesV(center, radius, color);
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

static int l_DrawRectangleV(lua_State *L) {
	Vector2 pos = checkVector2(L, 1);
	Vector2 size = checkVector2(L, 2);
	Color color = checkColor(L, 3);
	DrawRectangleV(pos, size, color);
	return 0;
}

static int l_DrawRectangleRec(lua_State *L) {
	Rectangle rec = checkRectangle(L, 1);
	Color color = checkColor(L, 2);
	DrawRectangleRec(rec, color);
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

static int l_DrawRectangleGradientEx(lua_State *L) {
	Rectangle rec = checkRectangle(L, 1);
	Color topLeft = checkColor(L, 2);
	Color bottomLeft = checkColor(L, 3);
	Color topRight = checkColor(L, 4);
	Color bottomRight = checkColor(L, 5);
	DrawRectangleGradientEx(rec, topLeft, bottomLeft, topRight, bottomRight);
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

static int l_DrawRectangleLinesEx(lua_State *L) {
	Rectangle rec = checkRectangle(L, 1);
	float lineThick = (float)luaL_checknumber(L, 2);
	Color color = checkColor(L, 3);
	DrawRectangleLinesEx(rec, lineThick, color);
	return 0;
}

static int l_DrawRectangleRounded(lua_State *L) {
	Rectangle rec = checkRectangle(L, 1);
	float roundness = (float)luaL_checknumber(L, 2);
	int segments = luaL_checkinteger(L, 3);
	Color color = checkColor(L, 4);
	DrawRectangleRounded(rec, roundness, segments, color);
	return 0;
}

static int l_DrawRectangleRoundedLines(lua_State *L) {
	Rectangle rec = checkRectangle(L, 1);
	float roundness = (float)luaL_checknumber(L, 2);
	int segments = luaL_checkinteger(L, 3);
	Color color = checkColor(L, 4);
	DrawRectangleRoundedLines(rec, roundness, segments, color);
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

/* Collision detection */
static int l_CheckCollisionRecs(lua_State *L) {
	Rectangle a = checkRectangle(L, 1);
	Rectangle b = checkRectangle(L, 2);
	lua_pushboolean(L, CheckCollisionRecs(a, b));
	return 1;
}

static int l_CheckCollisionCircles(lua_State *L) {
	float x1 = (float)luaL_checknumber(L, 1);
	float y1 = (float)luaL_checknumber(L, 2);
	float r1 = (float)luaL_checknumber(L, 3);
	float x2 = (float)luaL_checknumber(L, 4);
	float y2 = (float)luaL_checknumber(L, 5);
	float r2 = (float)luaL_checknumber(L, 6);
	lua_pushboolean(L, CheckCollisionCircles(
		(Vector2){x1, y1}, r1, (Vector2){x2, y2}, r2));
	return 1;
}

static int l_CheckCollisionCircleRec(lua_State *L) {
	float cx = (float)luaL_checknumber(L, 1);
	float cy = (float)luaL_checknumber(L, 2);
	float r = (float)luaL_checknumber(L, 3);
	Rectangle rec = checkRectangle(L, 4);
	lua_pushboolean(L, CheckCollisionCircleRec((Vector2){cx, cy}, r, rec));
	return 1;
}

static int l_CheckCollisionPointRec(lua_State *L) {
	float px = (float)luaL_checknumber(L, 1);
	float py = (float)luaL_checknumber(L, 2);
	Rectangle rec = checkRectangle(L, 3);
	lua_pushboolean(L, CheckCollisionPointRec((Vector2){px, py}, rec));
	return 1;
}

static int l_CheckCollisionPointCircle(lua_State *L) {
	float px = (float)luaL_checknumber(L, 1);
	float py = (float)luaL_checknumber(L, 2);
	float cx = (float)luaL_checknumber(L, 3);
	float cy = (float)luaL_checknumber(L, 4);
	float r = (float)luaL_checknumber(L, 5);
	lua_pushboolean(L, CheckCollisionPointCircle((Vector2){px, py}, (Vector2){cx, cy}, r));
	return 1;
}

static int l_CheckCollisionPointTriangle(lua_State *L) {
	float px = (float)luaL_checknumber(L, 1);
	float py = (float)luaL_checknumber(L, 2);
	Vector2 a = checkVector2(L, 3);
	Vector2 b = checkVector2(L, 4);
	Vector2 c = checkVector2(L, 5);
	lua_pushboolean(L, CheckCollisionPointTriangle((Vector2){px, py}, a, b, c));
	return 1;
}

static int l_GetCollisionRec(lua_State *L) {
	Rectangle a = checkRectangle(L, 1);
	Rectangle b = checkRectangle(L, 2);
	Rectangle rec = GetCollisionRec(a, b);
	pushRectangle(L, rec);
	return 1;
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

static int l_DrawTextEx(lua_State *L) {
	const char *text = luaL_checkstring(L, 1);
	float posx = (float)luaL_checknumber(L, 2);
	float posy = (float)luaL_checknumber(L, 3);
	int fontsize = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	float spacing = lua_gettop(L) >= 6 ? (float)luaL_checknumber(L, 6) : 1.0f;
	DrawTextEx(GetFontDefault(), text, (Vector2){posx, posy}, fontsize, spacing, color);
	return 0;
}

static int l_MeasureText(lua_State *L) {
	const char *text = luaL_checkstring(L, 1);
	int fontsize = luaL_checkinteger(L, 2);
	lua_pushinteger(L, MeasureText(text, fontsize));
	return 1;
}

static int l_MeasureTextEx(lua_State *L) {
	const char *text = luaL_checkstring(L, 1);
	int fontsize = luaL_checkinteger(L, 2);
	float spacing = lua_gettop(L) >= 3 ? (float)luaL_checknumber(L, 3) : 1.0f;
	Vector2 size = MeasureTextEx(GetFontDefault(), text, fontsize, spacing);
	pushVector2(L, size);
	return 1;
}

static int l_GetFontLineHeight(lua_State *L) {
	lua_pushinteger(L, GetFontDefault().baseSize);
	return 1;
}

/* ====== Textures/Images ====== */
static int l_LoadImage(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	Image img = LoadImage(filename);
	if (img.data == NULL)
		return luaL_error(L, "failed to load image: %s", filename);
	/* Return a table: {width, height, mipmaps, format} */
	lua_createtable(L, 4, 0);
	lua_pushinteger(L, img.width); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, img.height); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, img.mipmaps); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, img.format); lua_rawseti(L, -2, 4);
	/* Store as userdata so we can free it later */
	void *ud = lua_newuserdatauv(L, sizeof(Image), 0);
	memcpy(ud, &img, sizeof(Image));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_UnloadImage(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	if (!lua_isnil(L, -1)) {
		Image *img = (Image *)lua_touserdata(L, -1);
		UnloadImage(*img);
		img->data = NULL;
	}
	return 0;
}

static int l_LoadTexture(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	Texture2D tex = LoadTexture(filename);
	if (tex.id == 0)
		return luaL_error(L, "failed to load texture: %s", filename);
	lua_createtable(L, 5, 0);
	lua_pushinteger(L, tex.id); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, tex.width); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, tex.height); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, tex.mipmaps); lua_rawseti(L, -2, 4);
	lua_pushinteger(L, tex.format); lua_rawseti(L, -2, 5);
	void *ud = lua_newuserdatauv(L, sizeof(Texture2D), 0);
	memcpy(ud, &tex, sizeof(Texture2D));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_LoadTextureFromImage(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	if (lua_isnil(L, -1))
		return luaL_error(L, "invalid image");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Texture2D tex = LoadTextureFromImage(*img);
	lua_createtable(L, 5, 0);
	lua_pushinteger(L, tex.id); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, tex.width); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, tex.height); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, tex.mipmaps); lua_rawseti(L, -2, 4);
	lua_pushinteger(L, tex.format); lua_rawseti(L, -2, 5);
	void *ud = lua_newuserdatauv(L, sizeof(Texture2D), 0);
	memcpy(ud, &tex, sizeof(Texture2D));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_UnloadTexture(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	if (!lua_isnil(L, -1)) {
		Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
		UnloadTexture(*tex);
		tex->id = 0;
	}
	return 0;
}

static int l_DrawTexture(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	if (lua_isnil(L, -1))
		return luaL_error(L, "invalid texture");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	int posX = luaL_checkinteger(L, 2);
	int posY = luaL_checkinteger(L, 3);
	Color color = checkColor(L, 4);
	DrawTexture(*tex, posX, posY, color);
	return 0;
}

static int l_DrawTextureEx(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Vector2 pos = checkVector2(L, 2);
	float rotation = (float)luaL_checknumber(L, 3);
	float scale = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawTextureEx(*tex, pos, rotation, scale, color);
	return 0;
}

static int l_DrawTextureRec(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Rectangle source = checkRectangle(L, 2);
	Vector2 pos = checkVector2(L, 3);
	Color color = checkColor(L, 4);
	DrawTextureRec(*tex, source, pos, color);
	return 0;
}

static int l_DrawTexturePro(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Rectangle source = checkRectangle(L, 2);
	Rectangle dest = checkRectangle(L, 3);
	Vector2 origin = checkVector2(L, 4);
	float rotation = (float)luaL_checknumber(L, 5);
	Color color = checkColor(L, 6);
	DrawTexturePro(*tex, source, dest, origin, rotation, color);
	return 0;
}

static int l_LoadRenderTexture(lua_State *L) {
	int width = luaL_checkinteger(L, 1);
	int height = luaL_checkinteger(L, 2);
	RenderTexture2D rt = LoadRenderTexture(width, height);
	lua_createtable(L, 6, 0);
	lua_pushinteger(L, rt.id); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, rt.texture.width); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, rt.texture.height); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, rt.texture.mipmaps); lua_rawseti(L, -2, 4);
	lua_pushinteger(L, rt.texture.format); lua_rawseti(L, -2, 5);
	lua_pushinteger(L, rt.depth.id); lua_rawseti(L, -2, 6);
	void *ud = lua_newuserdatauv(L, sizeof(RenderTexture2D), 0);
	memcpy(ud, &rt, sizeof(RenderTexture2D));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_UnloadRenderTexture(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected render texture table");
	lua_getfield(L, 1, "_ptr");
	if (!lua_isnil(L, -1)) {
		RenderTexture2D *rt = (RenderTexture2D *)lua_touserdata(L, -1);
		UnloadRenderTexture(*rt);
		rt->id = 0;
	}
	return 0;
}

static int l_GenTextureMipmaps(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	if (lua_isnil(L, -1))
		return luaL_error(L, "invalid texture");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	GenTextureMipmaps(tex);
	/* Update the table fields */
	lua_pushinteger(L, tex->mipmaps);
	lua_setfield(L, 1, "mipmaps");
	return 0;
}

static int l_SetTextureFilter(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	int filter = luaL_checkinteger(L, 2);
	SetTextureFilter(*tex, filter);
	return 0;
}

static int l_SetTextureWrap(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 1, "_ptr");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	int wrap = luaL_checkinteger(L, 2);
	SetTextureWrap(*tex, wrap);
	return 0;
}

static int l_GetPixelColor(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	if (lua_isnil(L, -1))
		return luaL_error(L, "invalid image");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	int x = luaL_checkinteger(L, 2);
	int y = luaL_checkinteger(L, 3);
	Color c = GetImageColor(*img, x, y);
	pushColor(L, c);
	return 1;
}

static int l_ImageResize(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	int w = luaL_checkinteger(L, 2);
	int h = luaL_checkinteger(L, 3);
	ImageResize(img, w, h);
	return 0;
}

static int l_ImageFormat(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	int format = luaL_checkinteger(L, 2);
	ImageFormat(img, format);
	return 0;
}

static int l_ImageToPOT(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Color fill = lua_gettop(L) >= 2 ? checkColor(L, 2) : (Color){0, 0, 0, 0};
	ImageToPOT(img, fill);
	return 0;
}

static int l_ImageCrop(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Rectangle crop = checkRectangle(L, 2);
	ImageCrop(img, crop);
	return 0;
}

static int l_ImageFlipVertical(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	ImageFlipVertical(img);
	return 0;
}

static int l_ImageFlipHorizontal(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	ImageFlipHorizontal(img);
	return 0;
}

static int l_ImageColorTint(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Color color = checkColor(L, 2);
	ImageColorTint(img, color);
	return 0;
}

static int l_ImageColorGrayscale(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	ImageColorGrayscale(img);
	return 0;
}

static int l_ImageColorContrast(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	float contrast = (float)luaL_checknumber(L, 2);
	ImageColorContrast(img, contrast);
	return 0;
}

static int l_ImageColorBrightness(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected image table");
	lua_getfield(L, 1, "_ptr");
	Image *img = (Image *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	int brightness = luaL_checkinteger(L, 2);
	ImageColorBrightness(img, brightness);
	return 0;
}

static int l_GenImageColor(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	Color color = checkColor(L, 3);
	Image img = GenImageColor(w, h, color);
	lua_createtable(L, 4, 0);
	lua_pushinteger(L, img.width); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, img.height); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, img.mipmaps); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, img.format); lua_rawseti(L, -2, 4);
	void *ud = lua_newuserdatauv(L, sizeof(Image), 0);
	memcpy(ud, &img, sizeof(Image));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_GenImageGradientV(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	Color top = checkColor(L, 3);
	Color bottom = checkColor(L, 4);
	/* Raylib 6.0: use GenImageGradientLinear with direction 0 (vertical) */
	Image img = GenImageGradientLinear(w, h, 0, top, bottom);
	lua_createtable(L, 4, 0);
	lua_pushinteger(L, img.width); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, img.height); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, img.mipmaps); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, img.format); lua_rawseti(L, -2, 4);
	void *ud = lua_newuserdatauv(L, sizeof(Image), 0);
	memcpy(ud, &img, sizeof(Image));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_GenImageGradientH(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	Color left = checkColor(L, 3);
	Color right = checkColor(L, 4);
	/* Raylib 6.0: use GenImageGradientLinear with direction 90 (horizontal) */
	Image img = GenImageGradientLinear(w, h, 90, left, right);
	lua_createtable(L, 4, 0);
	lua_pushinteger(L, img.width); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, img.height); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, img.mipmaps); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, img.format); lua_rawseti(L, -2, 4);
	void *ud = lua_newuserdatauv(L, sizeof(Image), 0);
	memcpy(ud, &img, sizeof(Image));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_GenImagePerlinNoise(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	int offsetX = luaL_checkinteger(L, 3);
	int offsetY = luaL_checkinteger(L, 4);
	float scale = (float)luaL_checknumber(L, 5);
	Image img = GenImagePerlinNoise(w, h, offsetX, offsetY, scale);
	lua_createtable(L, 4, 0);
	lua_pushinteger(L, img.width); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, img.height); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, img.mipmaps); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, img.format); lua_rawseti(L, -2, 4);
	void *ud = lua_newuserdatauv(L, sizeof(Image), 0);
	memcpy(ud, &img, sizeof(Image));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_GenImageCellular(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	int tileSize = luaL_checkinteger(L, 3);
	Image img = GenImageCellular(w, h, tileSize);
	lua_createtable(L, 4, 0);
	lua_pushinteger(L, img.width); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, img.height); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, img.mipmaps); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, img.format); lua_rawseti(L, -2, 4);
	void *ud = lua_newuserdatauv(L, sizeof(Image), 0);
	memcpy(ud, &img, sizeof(Image));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

/* ====== rmodels (3D shapes) ====== */
static int l_DrawModel(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected model table");
	lua_getfield(L, 1, "_ptr");
	if (lua_isnil(L, -1))
		return luaL_error(L, "invalid model");
	Model *mdl = (Model *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	Vector3 pos = checkVector3(L, 2);
	float scale = (float)luaL_checknumber(L, 3);
	Color tint = checkColor(L, 4);
	DrawModel(*mdl, pos, scale, tint);
	return 0;
}

static int l_DrawCube(lua_State *L) {
	Vector3 pos = checkVector3(L, 1);
	float width = (float)luaL_checknumber(L, 2);
	float height = (float)luaL_checknumber(L, 3);
	float length = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawCube(pos, width, height, length, color);
	return 0;
}

static int l_DrawCubeV(lua_State *L) {
	Vector3 position = checkVector3(L, 1);
	Vector3 size = checkVector3(L, 2);
	Color color = checkColor(L, 3);
	DrawCubeV(position, size, color);
	return 0;
}

static int l_DrawCubeWires(lua_State *L) {
	Vector3 position = checkVector3(L, 1);
	float width = (float)luaL_checknumber(L, 2);
	float height = (float)luaL_checknumber(L, 3);
	float length = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawCubeWires(position, width, height, length, color);
	return 0;
}

static int l_DrawSphere(lua_State *L) {
	Vector3 centerPos = checkVector3(L, 1);
	float radius = (float)luaL_checknumber(L, 2);
	Color color = checkColor(L, 3);
	DrawSphere(centerPos, radius, color);
	return 0;
}

static int l_DrawSphereEx(lua_State *L) {
	Vector3 centerPos = checkVector3(L, 1);
	float radius = (float)luaL_checknumber(L, 2);
	int rings = luaL_checkinteger(L, 3);
	int slices = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawSphereEx(centerPos, radius, rings, slices, color);
	return 0;
}

static int l_DrawSphereWires(lua_State *L) {
	Vector3 centerPos = checkVector3(L, 1);
	float radius = (float)luaL_checknumber(L, 2);
	int rings = luaL_checkinteger(L, 3);
	int slices = luaL_checkinteger(L, 4);
	Color color = checkColor(L, 5);
	DrawSphereWires(centerPos, radius, rings, slices, color);
	return 0;
}

static int l_DrawCylinder(lua_State *L) {
	Vector3 position = checkVector3(L, 1);
	float radiusTop = (float)luaL_checknumber(L, 2);
	float radiusBottom = (float)luaL_checknumber(L, 3);
	float height = (float)luaL_checknumber(L, 4);
	int slices = luaL_checkinteger(L, 5);
	Color color = checkColor(L, 6);
	DrawCylinder(position, radiusTop, radiusBottom, height, slices, color);
	return 0;
}

static int l_DrawCylinderWires(lua_State *L) {
	Vector3 position = checkVector3(L, 1);
	float radiusTop = (float)luaL_checknumber(L, 2);
	float radiusBottom = (float)luaL_checknumber(L, 3);
	float height = (float)luaL_checknumber(L, 4);
	int slices = luaL_checkinteger(L, 5);
	Color color = checkColor(L, 6);
	DrawCylinderWires(position, radiusTop, radiusBottom, height, slices, color);
	return 0;
}

static int l_DrawPlane(lua_State *L) {
	Vector3 centerPos = checkVector3(L, 1);
	Vector2 size = checkVector2(L, 2);
	Color color = checkColor(L, 3);
	DrawPlane(centerPos, size, color);
	return 0;
}

static int l_DrawRay(lua_State *L) {
	float ox = (float)luaL_checknumber(L, 1);
	float oy = (float)luaL_checknumber(L, 2);
	float oz = (float)luaL_checknumber(L, 3);
	float dx = (float)luaL_checknumber(L, 4);
	float dy = (float)luaL_checknumber(L, 5);
	float dz = (float)luaL_checknumber(L, 6);
	Color color = checkColor(L, 7);
	Ray ray = {{ox, oy, oz}, {dx, dy, dz}};
	DrawRay(ray, color);
	return 0;
}

static int l_DrawGrid(lua_State *L) {
	int slices = luaL_checkinteger(L, 1);
	float spacing = (float)luaL_checknumber(L, 2);
	DrawGrid(slices, spacing);
	return 0;
}

static int l_DrawLine3D(lua_State *L) {
	Vector3 startPos = checkVector3(L, 1);
	Vector3 endPos = checkVector3(L, 2);
	Color color = checkColor(L, 3);
	DrawLine3D(startPos, endPos, color);
	return 0;
}

static int l_DrawPoint3D(lua_State *L) {
	Vector3 position = checkVector3(L, 1);
	Color color = checkColor(L, 2);
	DrawPoint3D(position, color);
	return 0;
}

static int l_DrawCircle3D(lua_State *L) {
	Vector3 center = checkVector3(L, 1);
	float radius = (float)luaL_checknumber(L, 2);
	Vector3 rotationAxis = checkVector3(L, 3);
	float rotationAngle = (float)luaL_checknumber(L, 4);
	Color color = checkColor(L, 5);
	DrawCircle3D(center, radius, rotationAxis, rotationAngle, color);
	return 0;
}

static int l_DrawTriangle3D(lua_State *L) {
	Vector3 v1 = checkVector3(L, 1);
	Vector3 v2 = checkVector3(L, 2);
	Vector3 v3 = checkVector3(L, 3);
	Color color = checkColor(L, 4);
	DrawTriangle3D(v1, v2, v3, color);
	return 0;
}

/* ====== Audio ====== */
static int l_InitAudioDevice(lua_State *L) {
	(void)L;
	InitAudioDevice();
	return 0;
}

static int l_CloseAudioDevice(lua_State *L) {
	(void)L;
	CloseAudioDevice();
	return 0;
}

static int l_IsAudioDeviceReady(lua_State *L) {
	lua_pushboolean(L, IsAudioDeviceReady());
	return 1;
}

static int l_SetMasterVolume(lua_State *L) {
	float volume = (float)luaL_checknumber(L, 1);
	SetMasterVolume(volume);
	return 0;
}

static int l_LoadWave(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	Wave wave = LoadWave(filename);
	if (wave.data == NULL)
		return luaL_error(L, "failed to load wave: %s", filename);
	lua_createtable(L, 5, 0);
	lua_pushinteger(L, wave.frameCount); lua_rawseti(L, -2, 1);
	lua_pushinteger(L, wave.sampleRate); lua_rawseti(L, -2, 2);
	lua_pushinteger(L, wave.sampleSize); lua_rawseti(L, -2, 3);
	lua_pushinteger(L, wave.channels); lua_rawseti(L, -2, 4);
	void *ud = lua_newuserdatauv(L, sizeof(Wave), 0);
	memcpy(ud, &wave, sizeof(Wave));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_UnloadWave(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected wave table");
	lua_getfield(L, 1, "_ptr");
	if (!lua_isnil(L, -1)) {
		Wave *w = (Wave *)lua_touserdata(L, -1);
		UnloadWave(*w);
		w->data = NULL;
	}
	return 0;
}

static int l_LoadSound(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	Sound snd = LoadSound(filename);
	if (snd.frameCount == 0)
		return luaL_error(L, "failed to load sound: %s", filename);
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, snd.frameCount); lua_rawseti(L, -2, 1);
	void *ud = lua_newuserdatauv(L, sizeof(Sound), 0);
	memcpy(ud, &snd, sizeof(Sound));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_UnloadSound(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected sound table");
	lua_getfield(L, 1, "_ptr");
	if (!lua_isnil(L, -1)) {
		Sound *snd = (Sound *)lua_touserdata(L, -1);
		UnloadSound(*snd);
	}
	return 0;
}

static int l_PlaySound(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected sound table");
	lua_getfield(L, 1, "_ptr");
	Sound *snd = (Sound *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	PlaySound(*snd);
	return 0;
}

static int l_StopSound(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected sound table");
	lua_getfield(L, 1, "_ptr");
	Sound *snd = (Sound *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	StopSound(*snd);
	return 0;
}

static int l_PauseSound(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected sound table");
	lua_getfield(L, 1, "_ptr");
	Sound *snd = (Sound *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	PauseSound(*snd);
	return 0;
}

static int l_ResumeSound(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected sound table");
	lua_getfield(L, 1, "_ptr");
	Sound *snd = (Sound *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	ResumeSound(*snd);
	return 0;
}

static int l_IsSoundPlaying(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected sound table");
	lua_getfield(L, 1, "_ptr");
	Sound *snd = (Sound *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_pushboolean(L, IsSoundPlaying(*snd));
	return 1;
}

static int l_SetSoundVolume(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected sound table");
	lua_getfield(L, 1, "_ptr");
	Sound *snd = (Sound *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	float volume = (float)luaL_checknumber(L, 2);
	SetSoundVolume(*snd, volume);
	return 0;
}

static int l_LoadMusicStream(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	Music mus = LoadMusicStream(filename);
	if (mus.frameCount == 0)
		return luaL_error(L, "failed to load music: %s", filename);
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, mus.frameCount); lua_rawseti(L, -2, 1);
	void *ud = lua_newuserdatauv(L, sizeof(Music), 0);
	memcpy(ud, &mus, sizeof(Music));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_UnloadMusicStream(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	if (!lua_isnil(L, -1)) {
		Music *mus = (Music *)lua_touserdata(L, -1);
		UnloadMusicStream(*mus);
	}
	return 0;
}

static int l_PlayMusicStream(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	PlayMusicStream(*mus);
	return 0;
}

static int l_StopMusicStream(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	StopMusicStream(*mus);
	return 0;
}

static int l_PauseMusicStream(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	PauseMusicStream(*mus);
	return 0;
}

static int l_ResumeMusicStream(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	ResumeMusicStream(*mus);
	return 0;
}

static int l_IsMusicStreamPlaying(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_pushboolean(L, IsMusicStreamPlaying(*mus));
	return 1;
}

static int l_SetMusicVolume(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	float volume = (float)luaL_checknumber(L, 2);
	SetMusicVolume(*mus, volume);
	return 0;
}

static int l_UpdateMusicStream(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	UpdateMusicStream(*mus);
	return 0;
}

static int l_GetMusicTimeLength(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_pushnumber(L, GetMusicTimeLength(*mus));
	return 1;
}

static int l_GetMusicTimePlayed(lua_State *L) {
	if (!lua_istable(L, 1))
		return luaL_error(L, "expected music table");
	lua_getfield(L, 1, "_ptr");
	Music *mus = (Music *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	lua_pushnumber(L, GetMusicTimePlayed(*mus));
	return 1;
}

/* ====== rshader ====== */

static int l_LoadShader(lua_State *L) {
	const char *vsFileName = lua_isstring(L, 1) ? lua_tostring(L, 1) : NULL;
	const char *fsFileName = lua_isstring(L, 2) ? lua_tostring(L, 2) : NULL;
	Shader shader = LoadShader(vsFileName, fsFileName);
	if (!IsShaderValid(shader))
		return luaL_error(L, "failed to load shader");
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, shader.id); lua_rawseti(L, -2, 1);
	/* Store locs array as a lightuserdata (we don't need to access it from Lua) */
	void *ud = lua_newuserdatauv(L, sizeof(Shader), 0);
	memcpy(ud, &shader, sizeof(Shader));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static int l_LoadShaderFromMemory(lua_State *L) {
	const char *vsCode = lua_isstring(L, 1) ? lua_tostring(L, 1) : NULL;
	const char *fsCode = lua_isstring(L, 2) ? lua_tostring(L, 2) : NULL;
	Shader shader = LoadShaderFromMemory(vsCode, fsCode);
	if (!IsShaderValid(shader))
		return luaL_error(L, "failed to compile shader");
	lua_createtable(L, 2, 0);
	lua_pushinteger(L, shader.id); lua_rawseti(L, -2, 1);
	void *ud = lua_newuserdatauv(L, sizeof(Shader), 0);
	memcpy(ud, &shader, sizeof(Shader));
	lua_setfield(L, -2, "_ptr");
	return 1;
}

static Shader checkShader(lua_State *L, int idx) {
	if (!lua_istable(L, idx))
		luaL_error(L, "expected shader table");
	lua_getfield(L, idx, "_ptr");
	if (lua_isnil(L, -1))
		luaL_error(L, "invalid shader");
	Shader *s = (Shader *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	return *s;
}

static int l_UnloadShader(lua_State *L) {
	Shader shader = checkShader(L, 1);
	UnloadShader(shader);
	return 0;
}

static int l_IsShaderValid(lua_State *L) {
	Shader shader = checkShader(L, 1);
	lua_pushboolean(L, IsShaderValid(shader));
	return 1;
}

static int l_GetShaderLocation(lua_State *L) {
	Shader shader = checkShader(L, 1);
	const char *uniformName = luaL_checkstring(L, 2);
	lua_pushinteger(L, GetShaderLocation(shader, uniformName));
	return 1;
}

static int l_GetShaderLocationAttrib(lua_State *L) {
	Shader shader = checkShader(L, 1);
	const char *attribName = luaL_checkstring(L, 2);
	lua_pushinteger(L, GetShaderLocationAttrib(shader, attribName));
	return 1;
}

static int l_SetShaderValue(lua_State *L) {
	Shader shader = checkShader(L, 1);
	int locIndex = luaL_checkinteger(L, 2);
	if (!lua_istable(L, 3))
		return luaL_error(L, "expected value table (number or list of numbers)");
	int uniformType = luaL_checkinteger(L, 4);

	int count = (int)luaL_len(L, 3);
	if (count < 1) return luaL_error(L, "value table is empty");

	/* For single-value types, just use count=1 from the table length */
	switch (uniformType) {
		case SHADER_UNIFORM_FLOAT: {
			float val;
			if (lua_istable(L, 3)) {
				lua_rawgeti(L, 3, 1);
				val = (float)lua_tonumber(L, -1);
				lua_pop(L, 1);
			} else {
				val = (float)luaL_checknumber(L, 3);
			}
			SetShaderValue(shader, locIndex, &val, uniformType);
			break;
		}
		case SHADER_UNIFORM_VEC2: {
			float vals[2];
			for (int i = 0; i < 2; i++) {
				lua_rawgeti(L, 3, i + 1);
				vals[i] = (float)lua_tonumber(L, -1);
				lua_pop(L, 1);
			}
			SetShaderValue(shader, locIndex, vals, uniformType);
			break;
		}
		case SHADER_UNIFORM_VEC3: {
			float vals[3];
			for (int i = 0; i < 3; i++) {
				lua_rawgeti(L, 3, i + 1);
				vals[i] = (float)lua_tonumber(L, -1);
				lua_pop(L, 1);
			}
			SetShaderValue(shader, locIndex, vals, uniformType);
			break;
		}
		case SHADER_UNIFORM_VEC4: {
			float vals[4];
			for (int i = 0; i < 4; i++) {
				lua_rawgeti(L, 3, i + 1);
				vals[i] = (float)lua_tonumber(L, -1);
				lua_pop(L, 1);
			}
			SetShaderValue(shader, locIndex, vals, uniformType);
			break;
		}
		case SHADER_UNIFORM_INT: {
			int val = (int)luaL_checkinteger(L, 3);
			SetShaderValue(shader, locIndex, &val, uniformType);
			break;
		}
		case SHADER_UNIFORM_IVEC2: {
			int vals[2];
			for (int i = 0; i < 2; i++) {
				lua_rawgeti(L, 3, i + 1);
				vals[i] = (int)lua_tointeger(L, -1);
				lua_pop(L, 1);
			}
			SetShaderValue(shader, locIndex, vals, uniformType);
			break;
		}
		case SHADER_UNIFORM_IVEC3: {
			int vals[3];
			for (int i = 0; i < 3; i++) {
				lua_rawgeti(L, 3, i + 1);
				vals[i] = (int)lua_tointeger(L, -1);
				lua_pop(L, 1);
			}
			SetShaderValue(shader, locIndex, vals, uniformType);
			break;
		}
		case SHADER_UNIFORM_IVEC4: {
			int vals[4];
			for (int i = 0; i < 4; i++) {
				lua_rawgeti(L, 3, i + 1);
				vals[i] = (int)lua_tointeger(L, -1);
				lua_pop(L, 1);
			}
			SetShaderValue(shader, locIndex, vals, uniformType);
			break;
		}
		case SHADER_UNIFORM_SAMPLER2D: {
			int val = (int)luaL_checkinteger(L, 3);
			SetShaderValue(shader, locIndex, &val, uniformType);
			break;
		}
		default:
			return luaL_error(L, "unsupported uniform type: %d", uniformType);
	}
	return 0;
}

static int l_SetShaderValueMatrix(lua_State *L) {
	Shader shader = checkShader(L, 1);
	int locIndex = luaL_checkinteger(L, 2);
	if (!lua_istable(L, 3))
		return luaL_error(L, "expected matrix table of 16 numbers");
	Matrix mat = {0};
	float *m = (float *)&mat;
	for (int i = 0; i < 16; i++) {
		lua_rawgeti(L, 3, i + 1);
		m[i] = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	SetShaderValueMatrix(shader, locIndex, mat);
	return 0;
}

static int l_SetShaderValueTexture(lua_State *L) {
	Shader shader = checkShader(L, 1);
	int locIndex = luaL_checkinteger(L, 2);
	if (!lua_istable(L, 3))
		return luaL_error(L, "expected texture table");
	lua_getfield(L, 3, "_ptr");
	if (lua_isnil(L, -1))
		return luaL_error(L, "invalid texture");
	Texture2D *tex = (Texture2D *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	SetShaderValueTexture(shader, locIndex, *tex);
	return 0;
}

static int l_BeginShaderMode(lua_State *L) {
	Shader shader = checkShader(L, 1);
	BeginShaderMode(shader);
	return 0;
}

static int l_EndShaderMode(lua_State *L) {
	(void)L;
	EndShaderMode();
	return 0;
}

/* ====== Function Registration ====== */
static const luaL_Reg raylib_funcs[] = {
	/* == rcore: window == */
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
	{"SetWindowTitle", l_SetWindowTitle},
	{"SetWindowPosition", l_SetWindowPosition},
	{"SetWindowMinSize", l_SetWindowMinSize},
	{"SetWindowSize", l_SetWindowSize},
	{"GetWindowPosition", l_GetWindowPosition},
	{"GetWindowScaleDPI", l_GetWindowScaleDPI},
	{"GetScreenWidth", l_GetScreenWidth},
	{"GetScreenHeight", l_GetScreenHeight},
	{"GetRenderWidth", l_GetRenderWidth},
	{"GetRenderHeight", l_GetRenderHeight},

	/* rcore: timing */
	{"GetTime", l_GetTime},
	{"GetFrameTime", l_GetFrameTime},
	{"GetFPS", l_GetFPS},
	{"SetTargetFPS", l_SetTargetFPS},

	/* rcore: misc */
	{"SetConfigFlags", l_SetConfigFlags},
	{"TraceLog", l_TraceLog},
	{"TakeScreenshot", l_TakeScreenshot},
	{"SetRandomSeed", l_SetRandomSeed},
	{"GetRandomValue", l_GetRandomValue},
	{"TextFormat", l_TextFormat},
	{"UpdateCamera", l_UpdateCamera},

	/* rcore: cursor */
	{"ShowCursor", l_ShowCursor},
	{"HideCursor", l_HideCursor},
	{"DisableCursor", l_DisableCursor},
	{"EnableCursor", l_EnableCursor},
	{"IsCursorHidden", l_IsCursorHidden},
	{"IsCursorOnScreen", l_IsCursorOnScreen},
	{"SetMousePosition", l_SetMousePosition},
	{"SetMouseOffset", l_SetMouseOffset},
	{"SetMouseScale", l_SetMouseScale},
	{"GetMousePosition", l_GetMousePosition},
	{"GetMouseDelta", l_GetMouseDelta},
	{"GetMouseWheelMove", l_GetMouseWheelMove},

	/* rcore: touch */
	{"GetTouchX", l_GetTouchX},
	{"GetTouchY", l_GetTouchY},
	{"GetTouchPosition", l_GetTouchPosition},
	{"GetTouchPointCount", l_GetTouchPointCount},

	/* rcore: gamepad */
	{"IsGamepadAvailable", l_IsGamepadAvailable},
	{"GetGamepadName", l_GetGamepadName},
	{"IsGamepadButtonPressed", l_IsGamepadButtonPressed},
	{"IsGamepadButtonDown", l_IsGamepadButtonDown},
	{"IsGamepadButtonReleased", l_IsGamepadButtonReleased},
	{"IsGamepadButtonUp", l_IsGamepadButtonUp},
	{"GetGamepadAxisMovement", l_GetGamepadAxisMovement},

	/* rcore: keyboard */
	{"IsKeyPressed", l_IsKeyPressed},
	{"IsKeyDown", l_IsKeyDown},
	{"IsKeyReleased", l_IsKeyReleased},
	{"IsKeyUp", l_IsKeyUp},
	{"IsKeyPressedRepeat", l_IsKeyPressedRepeat},
	{"GetKeyPressed", l_GetKeyPressed},
	{"GetCharPressed", l_GetCharPressed},
	{"SetExitKey", l_SetExitKey},

	/* rcore: mouse buttons */
	{"IsMouseButtonPressed", l_IsMouseButtonPressed},
	{"IsMouseButtonUp", l_IsMouseButtonUp},
	{"IsMouseButtonReleased", l_IsMouseButtonReleased},

	/* rcore: drawing modes */
	{"ClearBackground", l_ClearBackground},
	{"BeginDrawing", l_BeginDrawing},
	{"EndDrawing", l_EndDrawing},
	{"BeginMode2D", l_BeginMode2D},
	{"EndMode2D", l_EndMode2D},
	{"BeginMode3D", l_BeginMode3D},
	{"EndMode3D", l_EndMode3D},
	{"BeginTextureMode", l_BeginTextureMode},
	{"EndTextureMode", l_EndTextureMode},
	{"BeginScissorMode", l_BeginScissorMode},
	{"EndScissorMode", l_EndScissorMode},
	{"BeginBlendMode", l_BeginBlendMode},
	{"EndBlendMode", l_EndBlendMode},

	/* rshapes: pixels */
	{"DrawPixel", l_DrawPixel},
	{"DrawPixelV", l_DrawPixelV},

	/* rshapes: lines */
	{"DrawLine", l_DrawLine},
	{"DrawLineV", l_DrawLineV},
	{"DrawLineEx", l_DrawLineEx},
	{"DrawLineBezier", l_DrawLineBezier},
	{"DrawLineStrip", l_DrawLineStrip},

	/* rshapes: circles */
	{"DrawCircle", l_DrawCircle},
	{"DrawCircleV", l_DrawCircleV},
	{"DrawCircleGradient", l_DrawCircleGradient},
	{"DrawCircleLines", l_DrawCircleLines},
	{"DrawCircleLinesV", l_DrawCircleLinesV},

	/* rshapes: ellipses */
	{"DrawEllipse", l_DrawEllipse},
	{"DrawEllipseLines", l_DrawEllipseLines},

	/* rshapes: rectangles */
	{"DrawRectangle", l_DrawRectangle},
	{"DrawRectangleV", l_DrawRectangleV},
	{"DrawRectangleRec", l_DrawRectangleRec},
	{"DrawRectangleGradientV", l_DrawRectangleGradientV},
	{"DrawRectangleGradientH", l_DrawRectangleGradientH},
	{"DrawRectangleGradientEx", l_DrawRectangleGradientEx},
	{"DrawRectangleLines", l_DrawRectangleLines},
	{"DrawRectangleLinesEx", l_DrawRectangleLinesEx},
	{"DrawRectangleRounded", l_DrawRectangleRounded},
	{"DrawRectangleRoundedLines", l_DrawRectangleRoundedLines},

	/* rshapes: triangles/polygons */
	{"DrawTriangle", l_DrawTriangle},
	{"DrawTriangleLines", l_DrawTriangleLines},
	{"DrawPoly", l_DrawPoly},
	{"DrawPolyLines", l_DrawPolyLines},
	{"DrawPolyLinesEx", l_DrawPolyLinesEx},

	/* rshapes: collision */
	{"CheckCollisionRecs", l_CheckCollisionRecs},
	{"CheckCollisionCircles", l_CheckCollisionCircles},
	{"CheckCollisionCircleRec", l_CheckCollisionCircleRec},
	{"CheckCollisionPointRec", l_CheckCollisionPointRec},
	{"CheckCollisionPointCircle", l_CheckCollisionPointCircle},
	{"CheckCollisionPointTriangle", l_CheckCollisionPointTriangle},
	{"GetCollisionRec", l_GetCollisionRec},

	/* rtext */
	{"DrawFPS", l_DrawFPS},
	{"DrawText", l_DrawText},
	{"DrawTextEx", l_DrawTextEx},
	{"MeasureText", l_MeasureText},
	{"MeasureTextEx", l_MeasureTextEx},

	/* rtextures: image loading */
	{"LoadImage", l_LoadImage},
	{"UnloadImage", l_UnloadImage},
	{"GenImageColor", l_GenImageColor},
	{"GenImageGradientV", l_GenImageGradientV},
	{"GenImageGradientH", l_GenImageGradientH},
	{"GenImagePerlinNoise", l_GenImagePerlinNoise},
	{"GenImageCellular", l_GenImageCellular},
	{"ImageResize", l_ImageResize},
	{"ImageFormat", l_ImageFormat},
	{"ImageToPOT", l_ImageToPOT},
	{"ImageCrop", l_ImageCrop},
	{"ImageFlipVertical", l_ImageFlipVertical},
	{"ImageFlipHorizontal", l_ImageFlipHorizontal},
	{"ImageColorTint", l_ImageColorTint},
	{"ImageColorGrayscale", l_ImageColorGrayscale},
	{"ImageColorContrast", l_ImageColorContrast},
	{"ImageColorBrightness", l_ImageColorBrightness},
	{"GetPixelColor", l_GetPixelColor},

	/* rtextures: texture */
	{"LoadTexture", l_LoadTexture},
	{"LoadTextureFromImage", l_LoadTextureFromImage},
	{"UnloadTexture", l_UnloadTexture},
	{"DrawTexture", l_DrawTexture},
	{"DrawTextureEx", l_DrawTextureEx},
	{"DrawTextureRec", l_DrawTextureRec},
	{"DrawTexturePro", l_DrawTexturePro},
	{"GenTextureMipmaps", l_GenTextureMipmaps},
	{"SetTextureFilter", l_SetTextureFilter},
	{"SetTextureWrap", l_SetTextureWrap},
	{"LoadRenderTexture", l_LoadRenderTexture},
	{"UnloadRenderTexture", l_UnloadRenderTexture},

	/* rmodels: 3D primitives */
	{"DrawModel", l_DrawModel},
	{"DrawCube", l_DrawCube},
	{"DrawCubeV", l_DrawCubeV},
	{"DrawCubeWires", l_DrawCubeWires},
	{"DrawSphere", l_DrawSphere},
	{"DrawSphereEx", l_DrawSphereEx},
	{"DrawSphereWires", l_DrawSphereWires},
	{"DrawCylinder", l_DrawCylinder},
	{"DrawCylinderWires", l_DrawCylinderWires},
	{"DrawPlane", l_DrawPlane},
	{"DrawRay", l_DrawRay},
	{"DrawGrid", l_DrawGrid},
	{"DrawLine3D", l_DrawLine3D},
	{"DrawPoint3D", l_DrawPoint3D},
	{"DrawCircle3D", l_DrawCircle3D},
	{"DrawTriangle3D", l_DrawTriangle3D},

	/* raudio */
	{"InitAudioDevice", l_InitAudioDevice},
	{"CloseAudioDevice", l_CloseAudioDevice},
	{"IsAudioDeviceReady", l_IsAudioDeviceReady},
	{"SetMasterVolume", l_SetMasterVolume},
	{"LoadWave", l_LoadWave},
	{"UnloadWave", l_UnloadWave},
	{"LoadSound", l_LoadSound},
	{"UnloadSound", l_UnloadSound},
	{"PlaySound", l_PlaySound},
	{"StopSound", l_StopSound},
	{"PauseSound", l_PauseSound},
	{"ResumeSound", l_ResumeSound},
	{"IsSoundPlaying", l_IsSoundPlaying},
	{"SetSoundVolume", l_SetSoundVolume},
	{"LoadMusicStream", l_LoadMusicStream},
	{"UnloadMusicStream", l_UnloadMusicStream},
	{"PlayMusicStream", l_PlayMusicStream},
	{"StopMusicStream", l_StopMusicStream},
	{"PauseMusicStream", l_PauseMusicStream},
	{"ResumeMusicStream", l_ResumeMusicStream},
	{"IsMusicStreamPlaying", l_IsMusicStreamPlaying},
	{"SetMusicVolume", l_SetMusicVolume},
	{"UpdateMusicStream", l_UpdateMusicStream},
	{"GetMusicTimeLength", l_GetMusicTimeLength},
	{"GetMusicTimePlayed", l_GetMusicTimePlayed},

	/* rshader */
	{"LoadShader", l_LoadShader},
	{"LoadShaderFromMemory", l_LoadShaderFromMemory},
	{"UnloadShader", l_UnloadShader},
	{"IsShaderValid", l_IsShaderValid},
	{"GetShaderLocation", l_GetShaderLocation},
	{"GetShaderLocationAttrib", l_GetShaderLocationAttrib},
	{"SetShaderValue", l_SetShaderValue},
	{"SetShaderValueMatrix", l_SetShaderValueMatrix},
	{"SetShaderValueTexture", l_SetShaderValueTexture},
	{"BeginShaderMode", l_BeginShaderMode},
	{"EndShaderMode", l_EndShaderMode},

	{NULL, NULL}
};

/* ====== Module Initialization ====== */
void luaopen_raylib (lua_State *L) {
	registerColors(L);
	registerKeys(L);
	luaL_newlib(L, raylib_funcs);
	lua_setglobal(L, "raylib");
}
