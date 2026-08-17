# BLua

A better version of Lua that I made for myself. The only thing that's different about this version of Lua 5.5.1 is that it has support for a lot of other libraries, such as Raylib.

Another good thing about BLua is that everything is compiled into the executable itself, and not as a Lua module that for whatever reason needs CMake to compile. (looking at you, FLTK modules...)

# Libraries

 - Raylib
 - inih (INI parser) (read-only, want to add writing INI's)
 - FLTK (a few functions)
 - curses

# Extra Commands

 - `blua.sleep(N)` - Does what it looks like it does. `N` is any number, such as an integer or a float.
