#include <time.h>

#include "lua.h"
#include "lauxlib.h"


static int l_blua_sleep (lua_State *L) {
  double secs = luaL_checknumber(L, 1);
  struct timespec ts;
  ts.tv_sec = (time_t)secs;
  ts.tv_nsec = (long)((secs - ts.tv_sec) * 1e9);
  nanosleep(&ts, NULL);
  return 0;
}


static const luaL_Reg blua_funcs[] = {
  {"sleep", l_blua_sleep},
  {NULL, NULL}
};


int luaopen_blua (lua_State *L) {
  luaL_newlib(L, blua_funcs);
  lua_setglobal(L, "blua");
  return 0;
}
