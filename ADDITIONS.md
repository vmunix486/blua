# BLua additions

Things added to BLua that's not in Lua. Documentation about what they added, and how to use them.

## Raylib

See https://raylib.com

## FLTK

See https://fltk.org

## blua

### Functions

| Function | Explanation | Code |
| --- | --- | --- |
| `blua.sleep(seconds)` | Sleeps for the given number of seconds (supports fractions) | `blua.sleep(0.5)` |

## curses

### Functions

#### Screen

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.initscr()` | Starts curses mode. Sets global `COLS` and `LINES` to terminal size. | `curses.initscr()` |
| `curses.endwin()` | Ends curses mode and restores the terminal. | `curses.endwin()` |
| `curses.erase()` | Clears the screen. | `curses.erase()` |
| `curses.refresh()` | Redraws the screen. Also updates `COLS` and `LINES`. | `curses.refresh()` |

#### Input

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.getch()` | Waits for a keypress and returns the key code. | `local ch = curses.getch()` |
| `curses.ungetch(ch)` | Pushes a key code back onto the input buffer. | `curses.ungetch(ch)` |
| `curses.getstr()` | Reads a string from the terminal (until newline). | `local str = curses.getstr()` |
| `curses.has_key(ch)` | Returns `true` if the terminal supports the given key code. | `if curses.has_key(KEY_UP) then ... end` |

#### Cursor

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.move(y, x)` | Moves the cursor to the given position. | `curses.move(10, 20)` |
| `curses.getyx()` | Returns `y, x` of the current cursor position. | `local y, x = curses.getyx()` |
| `curses.getmaxyx()` | Returns `maxy, maxx` of the screen dimensions. | `local h, w = curses.getmaxyx()` |

#### Output

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.addch(ch)` | Writes a character at the current cursor position. | `curses.addch(string.byte("X"))` |
| `curses.addstr(str)` | Writes a string at the current cursor position. | `curses.addstr("hello")` |
| `curses.printw(str)` | Writes a string at the current cursor position (like `addstr`). | `curses.printw("hello")` |
| `curses.mvaddch(y, x, ch)` | Moves the cursor and writes a character. | `curses.mvaddch(5, 10, string.byte("X"))` |
| `curses.mvaddstr(y, x, str)` | Moves the cursor and writes a string. | `curses.mvaddstr(5, 10, "hello")` |
| `curses.mvprintw(y, x, str)` | Moves the cursor and writes a string. | `curses.mvprintw(5, 10, "hello")` |
| `curses.clrtoeol()` | Clears from the cursor to the end of the line. | `curses.clrtoeol()` |
| `curses.clrtobot()` | Clears from the cursor to the bottom of the screen. | `curses.clrtobot()` |
| `curses.deleteln()` | Deletes the line at the cursor, scrolling lines below up. | `curses.deleteln()` |
| `curses.insertln()` | Inserts a blank line at the cursor, scrolling lines below down. | `curses.insertln()` |

#### Attributes

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.attron(attr)` | Turns on the given attribute(s). | `curses.attron(A_BOLD)` |
| `curses.attroff(attr)` | Turns off the given attribute(s). | `curses.attroff(A_BOLD)` |
| `curses.attrset(attr)` | Sets the attribute(s), replacing any current ones. | `curses.attrset(A_BOLD)` |
| `curses.color_pair(n)` | Returns the attribute value for color pair `n`. | `curses.attron(curses.color_pair(1))` |

#### Colors

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.start_color()` | Enables color support. | `curses.start_color()` |
| `curses.init_pair(n, fg, bg)` | Defines color pair `n` with foreground `fg` and background `bg`. | `curses.init_pair(1, COLOR_RED, COLOR_BLACK)` |
| `curses.pair_content(n)` | Returns `fg, bg` of color pair `n`. | `local fg, bg = curses.pair_content(1)` |
| `curses.has_colors()` | Returns `true` if the terminal supports colors. | `if curses.has_colors() then ... end` |
| `curses.use_default_colors()` | Enables the default color pair (pair 0). | `curses.use_default_colors()` |

#### Mode

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.cbreak()` | Enables cbreak mode (line buffering disabled, signals still processed). | `curses.cbreak()` |
| `curses.nocbreak()` | Disables cbreak mode (returns to normal buffered mode). | `curses.nocbreak()` |
| `curses.echo()` | Enables input echoing. | `curses.echo()` |
| `curses.noecho()` | Disables input echoing. | `curses.noecho()` |
| `curses.raw()` | Enables raw mode (no signal processing at all). | `curses.raw()` |
| `curses.noraw()` | Disables raw mode. | `curses.noraw()` |
| `curses.curs_set(vis)` | Sets cursor visibility: 0 = invisible, 1 = normal, 2 = very visible. | `curses.curs_set(0)` |
| `curses.nodelay(flag)` | If `true`, `getch()` returns `-1` immediately if no key is pressed. | `curses.nodelay(true)` |
| `curses.timeout(ms)` | Sets the delay before `getch()` returns `-1` (in milliseconds). | `curses.timeout(100)` |
| `curses.keypad(flag)` | If `true`, enables extended key codes (arrows, function keys, etc). | `curses.keypad(true)` |

#### Window

| Function | Explanation | Code |
| --- | --- | --- |
| `curses.border()` | Draws a border around the screen using default characters. | `curses.border()` |
| `curses.box(v, h)` | Draws a box with vertical char `v` and horizontal char `h`. | `curses.box(0, 0)` |
| `curses.hline(ch, n)` | Draws a horizontal line of `n` characters of `ch` at the cursor. | `curses.hline(string.byte("-"), 40)` |
| `curses.vline(ch, n)` | Draws a vertical line of `n` characters of `ch` at the cursor. | `curses.vline(string.byte("|"), 20)` |
| `curses.scroll()` | Scrolls the screen up one line. | `curses.scroll()` |
| `curses.scrollok(flag)` | If `true`, allows the screen to scroll when the cursor moves past the bottom. | `curses.scrollok(true)` |

### Constants

#### Return codes

| Constant | Value | Meaning |
| --- | --- | --- |
| `OK` | `0` | Successful return |
| `ERR` | `-1` | Error return |

#### Screen

| Constant | Meaning |
| --- | --- |
| `COLS` | Terminal width (columns) |
| `LINES` | Terminal height (rows) |

#### Attributes

| Constant | Meaning |
| --- | --- |
| `A_NORMAL` | Normal text |
| `A_BOLD` | Bold text |
| `A_UNDERLINE` | Underlined text |
| `A_REVERSE` | Reverse video |
| `A_STANDOUT` | Terminal standout mode |
| `A_DIM` | Dim text |
| `A_BLINK` | Blinking text |
| `A_INVIS` | Invisible text |
| `A_ITALIC` | Italic text |

#### Colors

| Constant | Value |
| --- | --- |
| `COLORS` | Number of colors the terminal supports |
| `COLOR_PAIRS` | Number of color pairs available |
| `COLOR_BLACK` | `0` |
| `COLOR_RED` | `1` |
| `COLOR_GREEN` | `2` |
| `COLOR_YELLOW` | `3` |
| `COLOR_BLUE` | `4` |
| `COLOR_MAGENTA` | `5` |
| `COLOR_CYAN` | `6` |
| `COLOR_WHITE` | `7` |

#### Keys

| Constant | Meaning |
| --- | --- |
| `KEY_UP` | Up arrow |
| `KEY_DOWN` | Down arrow |
| `KEY_LEFT` | Left arrow |
| `KEY_RIGHT` | Right arrow |
| `KEY_ENTER` | Enter key |
| `KEY_BACKSPACE` | Backspace |
| `KEY_DC` | Delete key |
| `KEY_IC` | Insert key |
| `KEY_HOME` | Home key |
| `KEY_END` | End key |
| `KEY_NPAGE` | Page Down |
| `KEY_PPAGE` | Page Up |
| `KEY_RESIZE` | Terminal resize event |
| `KEY_F0` | Base for function keys |
| `KEY_F1` .. `KEY_F12` | Function keys F1 through F12 |

## fs

Requires POSIX (`--enable-c89` disables it).

### Functions

| Function | Explanation | Code |
| --- | --- | --- |
| `fs.copy(src, dst)` | Copies a file from `src` to `dst`. | `fs.copy("a.txt", "b.txt")` |
| `fs.remove(path)` | Deletes a file. | `fs.remove("a.txt")` |
| `fs.rename(old, new)` | Renames/moves a file. | `fs.rename("a.txt", "b.txt")` |
| `fs.exists(path)` | Returns `true` if the file or directory exists. | `if fs.exists("foo") then ... end` |
| `fs.attributes(path [, request])` | Returns a table of file attributes, or a single attribute if `request` is given. | `local info = fs.attributes("foo")` <br> `local size = fs.attributes("foo", "size")` |
| `fs.symlinkattributes(path)` | Like `fs.attributes`, but follows symlinks for `lstat`. Returns a `target` field with the symlink target. | `local info = fs.symlinkattributes("link")` |
| `fs.chdir(path)` | Changes the current working directory. | `fs.chdir("/tmp")` |
| `fs.currentdir()` | Returns the current working directory. | `local dir = fs.currentdir()` |
| `fs.dir(path)` | Returns an iterator function and a userdata that yields filenames in the directory. | `for name in fs.dir("/tmp") do print(name) end` |
| `fs.mkdir(path)` | Creates a directory. | `fs.mkdir("newdir")` |
| `fs.rmdir(path)` | Removes an empty directory. | `fs.rmdir("newdir")` |
| `fs.touch(path [, atime [, mtime]])` | Sets the access and modification times of a file. Defaults to current time. | `fs.touch("foo")` |
| `fs.link(old, new [, symlink])` | Creates a hard link. Pass `true` as the third argument to create a symlink instead. | `fs.link("a.txt", "b.txt")` <br> `fs.link("a.txt", "link", true)` |

#### `fs.attributes` fields

| Field | Type | Description |
| --- | --- | --- |
| `mode` | string | `"file"`, `"directory"`, `"link"`, `"named pipe"`, `"socket"`, `"char device"`, `"block device"`, or `"other"` |
| `size` | integer | File size in bytes |
| `access` | number | Last access time (Unix timestamp) |
| `modification` | number | Last modification time (Unix timestamp) |
| `change` | number | Last status change time (Unix timestamp) |
| `dev` | integer | Device ID |
| `ino` | integer | Inode number |
| `nlink` | integer | Number of hard links |
| `uid` | integer | Owner user ID |
| `gid` | integer | Owner group ID |
| `rdev` | integer | Device type (for special files) |
| `blocks` | integer | Number of 512B blocks allocated |
| `blksize` | integer | Filesystem block size |
| `devname` | string | The path passed to `attributes()` |

## inih

See https://github.com/benhoyt/inih

### Functions

| Function | Explanation | Code |
| --- | --- | --- |
| `ini.parse("FILE")` | Parses the specified INI file and returns a table with its sections and keys. | `local foo = ini.parse("bar.ini")` <br> `print(foo.baz)` |
