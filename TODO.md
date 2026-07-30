# TODO

 - Add all the functions from Raylib
 - Add a lot more functions from FLTK
 - Add lots of examples (port ones from raylib, add others for other libraries)
 - Fix Raylib crashing on some 3D stuff
 - Fix support on Windows
 - Add support for a `sleep(N)` function
 - Add more functions from curses
 - Add library detection eg.
 ```lua
 if not blua.library("raylib") then
	 os.exit(1)
 end
 ```
 - Add switch statements. To make porting C programs to Lua easier.

## To be added libraries

 - SDL2
 - Networking (`arpa/inet.h`, `sys/socket.h`, `netinet/in.h`)
 - X11 (Xlib, Athena Widgets, Motif)
 - Nano-X (MicroWindows, NX11, Pure Nano-X)
 - DirectFB2
 - Linux framebuffer (`/dev/fb0`)
 - Nuklear UI
 - Miniaudio
 - GMP (replacing default Lua math library)
