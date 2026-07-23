#include <string.h>
#include "linih.h"
#include "ini.h"
#include "lua.h"
#include "lauxlib.h"

typedef struct {
	lua_State *L;
	int table;  /* reference to result table */
} ParseCtx;

static int handler(void *user, const char *section, const char *name,
                   const char *value) {
	ParseCtx *ctx = (ParseCtx *)user;
	lua_State *L = ctx->L;

	/* Get or create the section table */
	lua_rawgeti(L, LUA_REGISTRYINDEX, ctx->table);
	if (section[0] != '\0') {
		lua_getfield(L, -1, section);
		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushvalue(L, -1);  /* duplicate for setfield */
			lua_setfield(L, -3, section);
		}
	} else {
		/* No section - use the root table */
		lua_pushvalue(L, -1);
	}

	/* Set key = value */
	lua_pushstring(L, value);
	lua_setfield(L, -2, name);

	lua_pop(L, 2);  /* pop section table and root table */
	return 1;
}

static int l_ini_parse(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	ParseCtx ctx;
	ctx.L = L;

	/* Create result table */
	lua_newtable(L);
	lua_pushvalue(L, -1);
	ctx.table = luaL_ref(L, LUA_REGISTRYINDEX);

	int error = ini_parse(filename, handler, &ctx);

	/* Clean up registry reference */
	luaL_unref(L, LUA_REGISTRYINDEX, ctx.table);

	if (error != 0) {
		lua_pushnil(L);
		lua_pushinteger(L, error);
		return 2;
	}

	/* Return the table (already on stack) */
	return 1;
}

static int l_ini_parsestring(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	ParseCtx ctx;
	ctx.L = L;

	/* Create result table */
	lua_newtable(L);
	lua_pushvalue(L, -1);
	ctx.table = luaL_ref(L, LUA_REGISTRYINDEX);

	int error = ini_parse_string(str, handler, &ctx);

	/* Clean up registry reference */
	luaL_unref(L, LUA_REGISTRYINDEX, ctx.table);

	if (error != 0) {
		lua_pushnil(L);
		lua_pushinteger(L, error);
		return 2;
	}

	/* Return the table (already on stack) */
	return 1;
}

static const luaL_Reg ini_funcs[] = {
	{"parse", l_ini_parse},
	{"parsestring", l_ini_parsestring},
	{NULL, NULL}
};

void luaopen_ini(lua_State *L) {
	luaL_newlib(L, ini_funcs);
	lua_setglobal(L, "ini");
}
