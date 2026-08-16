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
BLUA_SRC = raylua.c ini.c linih.c blua.c

if HAVE_NCURSES
BLUA_SRC += lcurses.c
endif

# FLTK extension (C++) - compiled separately, linked into blua directly
if HAVE_FLTK
BLUA_FLTK_SRC = lfltk.cxx
endif

# All library sources
LIB_SRC = $(LUA_CORE_SRC) $(LUA_LIB_SRC) $(BLUA_SRC)
LIB_OBJ = $(LIB_SRC:.c=.o)

# Compiler/linker flags
# _RAYLIB is defined by AC_DEFINE in configure.ac

lib_LIBRARIES = liblua.a
liblua_a_SOURCES = $(LIB_SRC)

if HAVE_FLTK
liblua_a_CXXFLAGS = @FLTK_CXXFLAGS@
endif

# Interpreter
bin_PROGRAMS = blua luac

blua_SOURCES = lua.c
blua_LDADD = liblua.a

if HAVE_NCURSES
blua_LDADD += -lncurses
endif

if HAVE_FLTK
blua_SOURCES += lfltk.cxx
blua_CXXFLAGS = @FLTK_CXXFLAGS@
blua_LDADD += @FLTK_LIBS@
endif

luac_SOURCES = luac.c
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
