# TODO

 - Add all the functions from Raylib
 - Add a lot more functions from FLTK
 - Add lots of examples (port ones from raylib, add others for other libraries)
 - Fix Raylib crashing on some 3D stuff
 - Fix support on Windows
 - Add support for a `blua.sleep(N)` function
 - Add more functions from curses
 - Add library detection eg.
 ```lua
 if not blua.library("raylib") then
	 os.exit(1)
 end
 ```
 - Add switch statements. To make porting C programs to Lua easier.
 - Add `range(N)`, like Python.

## To be added libraries

 - SDL2 https://www.libsdl.org/
 - Networking (`arpa/inet.h`, `sys/socket.h`, `netinet/in.h`)
 - X11 (Xlib, Athena Widgets, Motif) https://www.x.org/ https://xfree86.org/ https://www.xlibre.net/ https://motif.ics.com/motif/
 - Nano-X (MicroWindows, NX11, Pure Nano-X) https://github.com/ghaerr/microwindows https://microwindows.org
 - DirectFB2 https://directfb2.github.io/ https://github.com/directfb2/DirectFB2
 - Linux framebuffer (`/dev/fb0`)
 - Nuklear UI https://github.com/Immediate-Mode-UI/Nuklear https://immediate-mode-ui.github.io/Nuklear/
 - Miniaudio https://miniaud.io/ https://github.com/mackron/miniaudio
 - GMP (replacing default Lua math library) https://gmplib.org/
