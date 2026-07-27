# Lua core sources
LUA_CORE_SRC = \
	lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c \
	lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c \
	lparser.c lstate.c lstring.c ltable.c ltm.c \
	lundump.c lvm.c lzio.c

# Lua library sources
LUA_LIB_SRC = \
	lauxlib.c lbaselib.c lcorolib.c ldblib.c liolib.c \
	lmathlib.c loadlib.c loslib.c lstrlib.c ltablib.c \
	lutf8lib.c linit.c

# blua extensions
BLUA_SRC = raylua.c ini.c linih.c

# All library sources
LIB_SRC = $(LUA_CORE_SRC) $(LUA_LIB_SRC) $(BLUA_SRC)
LIB_OBJ = $(LIB_SRC:.c=.o)

# Compiler/linker flags
if HAVE_RAYLIB
AM_CFLAGS = -D_RAYLIB
endif

lib_LIBRARIES = liblua.a
liblua_a_SOURCES = $(LIB_SRC)
liblua_a_CFLAGS = $(AM_CFLAGS)

# Interpreter
bin_PROGRAMS = blua luac

blua_SOURCES = lua.c
blua_CFLAGS = $(AM_CFLAGS)
blua_LDADD = liblua.a

luac_SOURCES = luac.c
luac_CFLAGS = $(AM_CFLAGS)
luac_LDADD = liblua.a

# Compiler-specific flags for parser/lexer (smaller code)
llex.o: llex.c
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) $(CMCFLAGS) -c -o $@ $<

lparser.o: lparser.c
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) $(CMCFLAGS) -c -o $@ $<

lcode.o: lcode.c
	$(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) $(CMCFLAGS) -c -o $@ $<

# Extra clean files
CLEANFILES = *.o
