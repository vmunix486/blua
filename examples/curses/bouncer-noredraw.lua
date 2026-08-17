-- vmunix note: This is good for making art. I think I'll make something like this with raylib
-- and both random colors, random palettes (2-N), and also set palettes as well with INI parsing.
-- as of writing, it's late and I'm tired so I'll do it Wednesday (8/19/26, today is 8/16)
local char = string.byte("#") -- The character to use for the bouncer
local frametime = 1/1000 -- When the next frame is

local dx = 1 -- X velocity
local dy = 1 -- Y velocity
local x = 10 -- starting X coordinate
local y = 10 -- starting Y coordinate

curses.initscr() -- Start curses
curses.curs_set(0) -- Hide the cursor

local width = COLS
local height = LINES
local color = 1 -- Set the first color

local colornum = 7 -- Set the amount of colors to use

if curses.has_colors() then -- If there are colors
	curses.start_color() -- Then start the colors
	colors = {
		COLOR_RED, 
		COLOR_GREEN,
		COLOR_YELLOW,
		COLOR_BLUE,
		COLOR_MAGENTA,
		COLOR_CYAN,
		COLOR_WHITE
	} -- And declare all the colors available

	-- This for loop increments from 1 to colornum (7), which means every color gets defined
	-- Without 7 lines of code. 7 -> 3 LOC
	for n = 1, colornum do
		curses.init_pair(n, colors[n], COLOR_BLACK)
	end
end

while true do
	x = x + dx -- Move X
	y = y + dy -- Move Y

	-- This is the wall bounce collision detection code. Changes color on bounce.
	if y == height then dy = -dy color = (color % colornum) + 1
	elseif y == 0 then dy = math.abs(dy) color = (color % colornum) + 1
	elseif x == width then dx = -dx color = (color % colornum) + 1
	elseif x == 0 then dx = math.abs(dx) color = (color % colornum) + 1 end

	-- Applies the current color after the bounce.
	curses.attron(curses.color_pair(color))
	curses.mvaddch(y, x, char) -- Draws the character
	curses.attroff(curses.color_pair(color))
	-- Turn off the color for the character

	curses.refresh() -- Draw to the screen

	blua.sleep(frametime) -- Sleep for frametime (defined near the top)
end

curses.endwin() -- ik this never gets ran (from CTRL-C, but idc). It ends curses
