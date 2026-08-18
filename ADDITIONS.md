# BLua additions

Things added to BLua that's not in Lua. Documentation about what they added, and how to use them.

## Raylib

See https://raylib.com

## FLTK

See https://fltk.org

### Quick start

```lua
-- Simple FLTK window
local win = fltk.Fl_Window(400, 300, "My App")

local btn = fltk.Fl_Button(150, 120, 100, 40, "Click me")
btn:callback(function()
    print("Button clicked!")
end)

win:finish()
win:show()
fltk.run()
```

Widget constructors all follow the same pattern: `fltk.Fl_Widget(x, y, w, h, label)`.
After creating a widget, you add it to a container (like `Fl_Window`) by creating it before calling `window:finish()`.
All widget methods return the widget itself (except getters), so you can chain: `btn:label("Hi"):color(FL_RED)`.

### Widget constructors

| Constructor | Explanation | Code |
| --- | --- | --- |
| `fltk.Fl_Window(w, h [, title])` | Creates a window. | `local win = fltk.Fl_Window(400, 300, "Hello")` |
| `fltk.Fl_Box(x, y, w, h [, label])` | Creates a box (non-interactive container). | `local box = fltk.Fl_Box(10, 10, 200, 50, "Label")` |
| `fltk.Fl_Button(x, y, w, h [, label])` | Creates a push button. | `local btn = fltk.Fl_Button(10, 10, 100, 40, "Click")` |
| `fltk.Fl_Input(x, y, w, h [, label])` | Creates a text input field. | `local inp = fltk.Fl_Input(10, 10, 200, 30)` |
| `fltk.Fl_Output(x, y, w, h [, label])` | Creates a read-only text display. | `local out = fltk.Fl_Output(10, 10, 200, 30)` |
| `fltk.Fl_Slider(x, y, w, h [, label])` | Creates a slider. | `local sld = fltk.Fl_Slider(10, 10, 200, 30)` |
| `fltk.Fl_Check_Button(x, y, w, h [, label])` | Creates a checkbox. | `local chk = fltk.Fl_Check_Button(10, 10, 100, 30, "Enable")` |
| `fltk.Fl_Choice(x, y, w, h [, label])` | Creates a dropdown menu. | `local cho = fltk.Fl_Choice(10, 10, 150, 30)` |
| `fltk.Fl_Hold_Browser(x, y, w, h [, label])` | Creates a scrollable list. | `local br = fltk.Fl_Hold_Browser(10, 10, 200, 150)` |
| `fltk.Fl_Spinner(x, y, w, h [, label])` | Creates a number spinner. | `local sp = fltk.Fl_Spinner(10, 10, 100, 30)` |
| `fltk.Fl_Progress(x, y, w, h [, label])` | Creates a progress bar. | `local pr = fltk.Fl_Progress(10, 10, 200, 30)` |

### Widget methods (all widgets)

These work on every widget type:

| Method | Explanation | Code |
| --- | --- | --- |
| `widget:box([type])` | Get or set the box type. | `btn:box(FL_DOWN_BOX)` |
| `widget:label([text])` | Get or set the label text. | `btn:label("Click me")` |
| `widget:labelsize([size])` | Get or set the label font size. | `btn:labelsize(16)` |
| `widget:labelfont([font])` | Get or set the label font. | `btn:labelfont(FL_BOLD)` |
| `widget:labeltype([type])` | Get or set the label type. | `btn:labeltype(FL_SHADOW_LABEL)` |
| `widget:color([c])` | Get or set the widget color. | `btn:color(FL_RED)` |
| `widget:tooltip([text])` | Set the tooltip text (shown on hover). | `btn:tooltip("Click this button")` |
| `widget:callback(fn)` | Set the callback function. Called with a lightuserdata handle. | `btn:callback(function() print("clicked") end)` |
| `widget:position(x, y)` | Move the widget. | `btn:position(50, 50)` |
| `widget:w()` | Get the width. | `local w = btn:w()` |
| `widget:h()` | Get the height. | `local h = btn:h()` |
| `widget:show()` | Show the widget. | `btn:show()` |
| `widget:hide()` | Hide the widget. | `btn:hide()` |
| `widget:resize(x, y, w, h)` | Move and resize the widget. | `btn:resize(10, 10, 200, 50)` |
| `widget:visible()` | Returns `true` if the widget is visible. | `if btn:visible() then ... end` |
| `widget:active()` | Returns `true` if the widget is active (not greyed out). | `if btn:active() then ... end` |
| `widget:activate()` | Enable the widget. | `btn:activate()` |
| `widget:deactivate()` | Disable the widget (greyed out). | `btn:deactivate()` |
| `widget:redraw()` | Force the widget to redraw. | `btn:redraw()` |

### Window methods

| Method | Explanation | Code |
| --- | --- | --- |
| `window:finish()` | End the window — call after creating all child widgets. | `win:finish()` |
| `window:show()` | Show the window. | `win:show()` |
| `window:make_current()` | Make the window the current GL/drawing context. | `win:make_current()` |

### Button methods

| Method | Explanation | Code |
| --- | --- | --- |
| `btn:value([b])` | Get or set the button state (`true`/`false`). For toggle buttons. | `btn:value(true)` |

### Input methods

These work on both `Fl_Input` and `Fl_Output`:

| Method | Explanation | Code |
| --- | --- | --- |
| `input:value([text])` | Get or set the text content. | `inp:value("hello")` |
| `input:insert(text)` | Insert text at the current cursor position. | `inp:insert(" world")` |
| `input:position([n])` | Get or set the cursor position. | `inp:position(5)` |
| `input:mark([n])` | Get or set the selection mark. | `inp:mark(0)` |
| `input:textsize([size])` | Get or set the text font size. | `inp:textsize(14)` |
| `input:textfont([font])` | Get or set the text font. | `inp:textfont(FL_BOLD)` |
| `input:textcolor([c])` | Get or set the text color. | `inp:textcolor(FL_BLUE)` |

### Slider methods

| Method | Explanation | Code |
| --- | --- | --- |
| `slider:value([v])` | Get or set the current value. | `sld:value(50)` |
| `slider:bounds(min, max)` | Set the minimum and maximum values. | `sld:bounds(0, 100)` |
| `slider:step([s])` | Get or set the step increment. | `sld:step(1)` |

### Check Button methods

| Method | Explanation | Code |
| --- | --- | --- |
| `check:value([b])` | Get or set the checked state. | `chk:value(true)` |

### Choice (dropdown) methods

| Method | Explanation | Code |
| --- | --- | --- |
| `choice:add(text)` | Add an item to the dropdown. Returns the item index. | `cho:add("Option 1")` |
| `choice:value([n])` | Get or set the selected item index. | `cho:value(0)` |
| `choice:text()` | Get the text of the currently selected item. | `local t = cho:text()` |

### Hold Browser (list) methods

| Method | Explanation | Code |
| --- | --- | --- |
| `browser:add(text)` | Add an item to the list. | `br:add("Item 1")` |
| `browser:clear()` | Remove all items. | `br:clear()` |
| `browser:value([n])` | Get or set the selected item index. | `br:value(3)` |
| `browser:text(n)` | Get the text of item `n`. | `local t = br:text(1)` |
| `browser:size()` | Get the number of items. | `local n = br:size()` |

### Spinner methods

| Method | Explanation | Code |
| --- | --- | --- |
| `spinner:value([v])` | Get or set the current value. | `sp:value(5)` |
| `spinner:step([s])` | Get or set the step increment. | `sp:step(0.5)` |
| `spinner:min([v])` | Get or set the minimum value. | `sp:min(0)` |
| `spinner:max([v])` | Get or set the maximum value. | `sp:max(100)` |

### Progress methods

| Method | Explanation | Code |
| --- | --- | --- |
| `progress:value([v])` | Get or set the current progress (0.0 to 1.0 range by default). | `pr:value(0.75)` |
| `progress:minimum([v])` | Get or set the minimum value. | `pr:minimum(0)` |
| `progress:maximum([v])` | Get or set the maximum value. | `pr:maximum(100)` |

### FLTK utility functions

| Function | Explanation | Code |
| --- | --- | --- |
| `fltk.run()` | Run the FLTK event loop. Returns when all windows are closed. | `fltk.run()` |
| `fltk.wait()` | Process one event cycle. Returns seconds until next timer. | `fltk.wait()` |
| `fltk.check()` | Process pending events without blocking. | `fltk.check()` |
| `fltk.redraw()` | Force all widgets to redraw. | `fltk.redraw()` |
| `fltk.scheme([name])` | Get or set the UI scheme (`"gtk"`, `"plastic"`, `"base"`, or `""`). | `fltk.scheme("gtk")` |
| `fltk.version()` | Get the FLTK version number. | `print(fltk.version())` |
| `fltk.api_version()` | Get the FLTK API version number. | `print(fltk.api_version())` |
| `fltk.display(name)` | Set the X11 display. Must be called before any widgets. | `fltk.display(":0")` |
| `fltk.option(opt [, val])` | Get or set a global FLTK option. | `fltk.option(FL_OPTION_USE_SYSTEM_COLORS, true)` |
| `fltk.focus([widget])` | Get or set keyboard focus. | `fltk.focus(inp)` |
| `fltk.grab([widget])` | Get or set the grab (modal) widget. | `fltk.grab(win)` |
| `fltk.release_grab()` | Release the grab. | `fltk.release_grab()` |
| `fltk.screen_count()` | Get the number of screens. | `local n = fltk.screen_count()` |
| `fltk.screen_xywh([num])` | Get `x, y, w, h` of screen `num` (default 0). | `local x, y, w, h = fltk.screen_xywh(0)` |

### Constants

#### Box types

| Constant | Meaning |
| --- | --- |
| `FL_UP_BOX` | Raised box (default for buttons) |
| `FL_DOWN_BOX` | Sunken box (default for inputs) |
| `FL_FLAT_BOX` | Flat box |
| `FL_THIN_UP_BOX` | Thin raised box |
| `FL_THIN_DOWN_BOX` | Thin sunken box |
| `FL_ENGRAVED_BOX` | Engraved border |
| `FL_EMBOSSED_BOX` | Embossed border |

#### Colors

| Constant | Meaning |
| --- | --- |
| `FL_BLACK` | Black |
| `FL_RED` | Red |
| `FL_GREEN` | Green |
| `FL_YELLOW` | Yellow |
| `FL_BLUE` | Blue |
| `FL_MAGENTA` | Magenta |
| `FL_CYAN` | Cyan |
| `FL_WHITE` | White |
| `FL_FOREGROUND_COLOR` | Default foreground |
| `FL_BACKGROUND_COLOR` | Default background (light grey) |
| `FL_INACTIVE_COLOR` | Greyed-out color |
| `FL_SELECTION_COLOR` | Selection highlight color |

#### Fonts

| Constant | Meaning |
| --- | --- |
| `FL_BOLD` | Bold font |
| `FL_ITALIC` | Italic font |
| `FL_BOLD_ITALIC` | Bold italic font |

#### Label types

| Constant | Meaning |
| --- | --- |
| `FL_NORMAL_LABEL` | Normal label (default) |
| `FL_SHADOW_LABEL` | Label with a drop shadow |
| `FL_ENGRAVED_LABEL` | Engraved-style label |
| `FL_EMBOSSED_LABEL` | Embossed-style label |

#### Alignment

| Constant | Meaning |
| --- | --- |
| `FL_ALIGN_CENTER` | Center alignment |
| `FL_ALIGN_TOP` | Align label to top |
| `FL_ALIGN_BOTTOM` | Align label to bottom |
| `FL_ALIGN_LEFT` | Align label to left |
| `FL_ALIGN_RIGHT` | Align label to right |
| `FL_ALIGN_INSIDE` | Place label inside widget |
| `FL_ALIGN_TEXT_OVER_IMAGE` | Text drawn over image |
| `FL_ALIGN_IMAGE_BACKDROP` | Image drawn as backdrop |
| `FL_ALIGN_CLIP` | Clip label to widget bounds |
| `FL_ALIGN_WRAP` | Wrap label text |

#### When (callback trigger conditions)

| Constant | Meaning |
| --- | --- |
| `FL_WHEN_NEVER` | Never call callback (default for most widgets) |
| `FL_WHEN_CHANGED` | Callback on every value change |
| `FL_WHEN_NOT_CHANGED` | Callback on change, but not from programmatic set |
| `FL_WHEN_RELEASE` | Callback when user releases (default for buttons) |
| `FL_WHEN_RELEASE_ALWAYS` | Callback on release, even if value didn't change |
| `FL_WHEN_ENTER_KEY` | Callback on Enter key in input fields |
| `FL_WHEN_ENTER_KEY_ALWAYS` | Callback on Enter, even if value didn't change |

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
