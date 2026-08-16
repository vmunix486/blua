# BLua additions

Things added to BLua that's not in Lua. Documentation about what they added, and how to use them.

## Raylib

See https://raylib.com

## FLTK

See https://fltk.org

## Curses

`¯\(ツ)/¯`

### Functions

| Function/Variable | Explaination | Code |
| --- | --- | --- |
| `curses.initscr()` | Starts curses | `curses.initscr()` |
| `curses.endwin()` | Ends curses | `curses.endwin()` |
| `curses.erase()` | Clears the terminal | `curses.erase()` |
| `curses.refresh()` | Draws everything to the terminal | `curses.refresh()` |
| `curses.mvaddch(y, x, "X")` | Move the cursor to the specified location and add the given character | `curses.mvaddch(10, 100, "o")` |
| `curses.mvaddstr(y, x, "string")` | Print the string at the specified location | `curses.mvaddstr(100, 50, "Foobar")` |
| `curses.mvprintw(y, x, "formatted text", variables)` | Ncurses `printf` function. | `curses.mvprintw(69, 420, string.format("%d", foo))` |

## inih

See https://github.com/benhoyt/inih

### Functions

| Function | Explaination | Code |
| --- | --- | --- |
| `ini.parse("FILE")` | Starts the parsing of the specified file in the argument. Has to be in double `""` or single `''` quotes. | `local foo = ini.parse("bar.ini")` <br><br> `print(foo.baz)`|

## More to be added
