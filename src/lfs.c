/*
** LuaFileSystem bindings for BLua
** Based on LuaFileSystem (https://github.com/lunarmodules/luafilesystem)
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "lua.h"
#include "lauxlib.h"

#ifdef LUA_USE_POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <utime.h>
#endif


/* ======================== fs.attributes ======================== */

#ifdef LUA_USE_POSIX

static const char *mode2string (mode_t mode) {
  if (S_ISREG(mode))        return "file";
  else if (S_ISDIR(mode))   return "directory";
  else if (S_ISLNK(mode))   return "link";
  else if (S_ISFIFO(mode))  return "named pipe";
  else if (S_ISSOCK(mode))  return "socket";
  else if (S_ISCHR(mode))   return "char device";
  else if (S_ISBLK(mode))   return "block device";
  else                      return "other";
}

static int push_st_info (lua_State *L, const char *path, const struct stat *info) {
  lua_newtable(L);
  lua_pushinteger(L, (lua_Integer)info->st_dev);
  lua_setfield(L, -2, "dev");
  lua_pushinteger(L, (lua_Integer)info->st_ino);
  lua_setfield(L, -2, "ino");
  lua_pushstring(L, mode2string(info->st_mode));
  lua_setfield(L, -2, "mode");
  lua_pushinteger(L, (lua_Integer)info->st_nlink);
  lua_setfield(L, -2, "nlink");
  lua_pushinteger(L, (lua_Integer)info->st_uid);
  lua_setfield(L, -2, "uid");
  lua_pushinteger(L, (lua_Integer)info->st_gid);
  lua_setfield(L, -2, "gid");
  lua_pushinteger(L, (lua_Integer)info->st_rdev);
  lua_setfield(L, -2, "rdev");
  lua_pushnumber(L, (lua_Number)info->st_atime);
  lua_setfield(L, -2, "access");
  lua_pushnumber(L, (lua_Number)info->st_mtime);
  lua_setfield(L, -2, "modification");
  lua_pushnumber(L, (lua_Number)info->st_ctime);
  lua_setfield(L, -2, "change");
  lua_pushinteger(L, (lua_Integer)info->st_size);
  lua_setfield(L, -2, "size");
  lua_pushinteger(L, (lua_Integer)info->st_blocks);
  lua_setfield(L, -2, "blocks");
  lua_pushinteger(L, (lua_Integer)info->st_blksize);
  lua_setfield(L, -2, "blksize");
  lua_pushstring(L, path);
  lua_setfield(L, -2, "devname");
  return 1;
}

static int lfs_attributes (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  const char *request = lua_type(L, 2) == LUA_TSTRING ? lua_tostring(L, 2) : NULL;
  struct stat info;
  if (stat(path, &info) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", path, strerror(errno));
    return 2;
  }
  if (request) {
    if (strcmp(request, "mode") == 0) {
      lua_pushstring(L, mode2string(info.st_mode));
    } else if (strcmp(request, "size") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_size);
    } else if (strcmp(request, "access") == 0) {
      lua_pushnumber(L, (lua_Number)info.st_atime);
    } else if (strcmp(request, "modification") == 0) {
      lua_pushnumber(L, (lua_Number)info.st_mtime);
    } else if (strcmp(request, "change") == 0) {
      lua_pushnumber(L, (lua_Number)info.st_ctime);
    } else if (strcmp(request, "dev") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_dev);
    } else if (strcmp(request, "ino") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_ino);
    } else if (strcmp(request, "nlink") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_nlink);
    } else if (strcmp(request, "uid") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_uid);
    } else if (strcmp(request, "gid") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_gid);
    } else if (strcmp(request, "rdev") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_rdev);
    } else if (strcmp(request, "blocks") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_blocks);
    } else if (strcmp(request, "blksize") == 0) {
      lua_pushinteger(L, (lua_Integer)info.st_blksize);
    } else {
      return luaL_error(L, "unknown attribute '%s'", request);
    }
    return 1;
  }
  return push_st_info(L, path, &info);
}


/* ======================== fs.symlinkattributes ======================== */

static int lfs_symlinkattributes (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  struct stat info;
  if (lstat(path, &info) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", path, strerror(errno));
    return 2;
  }
  push_st_info(L, path, &info);
  /* readlink for target field */
  {
    char buf[1024];
    ssize_t len = readlink(path, buf, sizeof(buf) - 1);
    if (len >= 0) {
      buf[len] = '\0';
      lua_pushstring(L, buf);
    } else {
      lua_pushnil(L);
    }
    lua_setfield(L, -2, "target");
  }
  return 1;
}


/* ======================== fs.chdir ======================== */

static int lfs_chdir (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  if (chdir(path) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s", strerror(errno));
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}


/* ======================== fs.currentdir ======================== */

static int lfs_currentdir (lua_State *L) {
  char buf[1024];
  if (getcwd(buf, sizeof(buf)) == NULL) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s", strerror(errno));
    return 2;
  }
  lua_pushstring(L, buf);
  return 1;
}


/* ======================== fs.dir ======================== */

typedef struct {
  DIR *dir;
} dir_data;

static int lfs_dir_close (lua_State *L) {
  dir_data *ud = (dir_data *)luaL_checkudata(L, 1, "fs.dir");
  if (ud->dir) {
    closedir(ud->dir);
    ud->dir = NULL;
  }
  return 0;
}

static int lfs_dir_next (lua_State *L) {
  dir_data *ud = (dir_data *)luaL_checkudata(L, 1, "fs.dir");
  struct dirent *entry;
  if (!ud->dir) return luaL_error(L, "closed directory");
  entry = readdir(ud->dir);
  if (!entry) {
    lua_pushnil(L);
    return 1;
  }
  lua_pushstring(L, entry->d_name);
  return 1;
}

static int lfs_dir (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  DIR *dir = opendir(path);
  dir_data *ud;
  if (!dir) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", path, strerror(errno));
    return 2;
  }
  ud = (dir_data *)lua_newuserdatauv(L, sizeof(dir_data), 0);
  ud->dir = dir;
  luaL_getmetatable(L, "fs.dir");
  lua_setmetatable(L, -2);
  lua_pushcfunction(L, lfs_dir_next);
  lua_insert(L, -2);
  return 2;
}


/* ======================== fs.mkdir ======================== */

static int lfs_mkdir (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  if (mkdir(path, 0777) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s", strerror(errno));
    lua_pushinteger(L, errno);
    return 3;
  }
  lua_pushboolean(L, 1);
  return 1;
}


/* ======================== fs.rmdir ======================== */

static int lfs_rmdir (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  if (rmdir(path) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s", strerror(errno));
    lua_pushinteger(L, errno);
    return 3;
  }
  lua_pushboolean(L, 1);
  return 1;
}


/* ======================== fs.touch ======================== */

static int lfs_touch (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  struct utimbuf times;
  times.actime = (lua_Number)luaL_optnumber(L, 2, (lua_Number)time(NULL));
  times.modtime = lua_gettop(L) >= 3 ? (lua_Number)luaL_checknumber(L, 3) : times.actime;
  if (utime(path, &times) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s", strerror(errno));
    lua_pushinteger(L, errno);
    return 3;
  }
  lua_pushboolean(L, 1);
  return 1;
}


/* ======================== fs.link ======================== */

static int lfs_link (lua_State *L) {
  const char *old = luaL_checkstring(L, 1);
  const char *new = luaL_checkstring(L, 2);
  int use_symlink = lua_toboolean(L, 3);
  int ok;
  if (use_symlink)
    ok = symlink(old, new);
  else
    ok = link(old, new);
  if (ok != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s", strerror(errno));
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

#endif /* LUA_USE_POSIX */


/* ======================== fs.copy ======================== */

#define COPY_BUF_SIZE 8192

static int lfs_copy (lua_State *L) {
  const char *src = luaL_checkstring(L, 1);
  const char *dst = luaL_checkstring(L, 2);
  FILE *fin, *fout;
  char buf[COPY_BUF_SIZE];
  size_t n;

  fin = fopen(src, "rb");
  if (!fin) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", src, strerror(errno));
    return 2;
  }
  fout = fopen(dst, "wb");
  if (!fout) {
    fclose(fin);
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", dst, strerror(errno));
    return 2;
  }
  while ((n = fread(buf, 1, COPY_BUF_SIZE, fin)) > 0) {
    if (fwrite(buf, 1, n, fout) != n) {
      fclose(fout);
      fclose(fin);
      lua_pushnil(L);
      lua_pushfstring(L, "error writing %s: %s", dst, strerror(errno));
      return 2;
    }
  }
  fclose(fout);
  fclose(fin);
  lua_pushboolean(L, 1);
  return 1;
}


/* ======================== fs.remove ======================== */

static int lfs_remove (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
  if (remove(path) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s: %s", path, strerror(errno));
    lua_pushinteger(L, errno);
    return 3;
  }
  lua_pushboolean(L, 1);
  return 1;
}


/* ======================== fs.rename ======================== */

static int lfs_rename (lua_State *L) {
  const char *old = luaL_checkstring(L, 1);
  const char *new = luaL_checkstring(L, 2);
  if (rename(old, new) != 0) {
    lua_pushnil(L);
    lua_pushfstring(L, "%s -> %s: %s", old, new, strerror(errno));
    lua_pushinteger(L, errno);
    return 3;
  }
  lua_pushboolean(L, 1);
  return 1;
}


/* ======================== fs.exists ======================== */

static int lfs_exists (lua_State *L) {
  const char *path = luaL_checkstring(L, 1);
#ifdef LUA_USE_POSIX
  struct stat info;
  lua_pushboolean(L, stat(path, &info) == 0);
#else
  FILE *f = fopen(path, "rb");
  if (f) { fclose(f); lua_pushboolean(L, 1); }
  else lua_pushboolean(L, 0);
#endif
  return 1;
}


/* ======================== Module registration ======================== */

static const luaL_Reg fs_funcs[] = {
  {"copy",        lfs_copy},
  {"remove",      lfs_remove},
  {"rename",      lfs_rename},
  {"exists",      lfs_exists},
#ifdef LUA_USE_POSIX
  {"attributes",          lfs_attributes},
  {"symlinkattributes",   lfs_symlinkattributes},
  {"chdir",       lfs_chdir},
  {"currentdir",  lfs_currentdir},
  {"dir",         lfs_dir},
  {"mkdir",       lfs_mkdir},
  {"rmdir",       lfs_rmdir},
  {"touch",       lfs_touch},
  {"link",        lfs_link},
#endif
  {NULL, NULL}
};


int luaopen_fs (lua_State *L) {
  luaL_newlib(L, fs_funcs);

#ifdef LUA_USE_POSIX
  /* Create metatable for dir objects */
  luaL_newmetatable(L, "fs.dir");
  lua_pushcfunction(L, lfs_dir_close);
  lua_setfield(L, -2, "__gc");
  lua_pushcfunction(L, lfs_dir_close);
  lua_setfield(L, -2, "close");
  lua_pushcfunction(L, lfs_dir_next);
  lua_setfield(L, -2, "next");
  lua_pop(L, 1);
#endif

  lua_setglobal(L, "fs");
  return 0;
}
