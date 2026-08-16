local char = string.byte("#")
local frametime = 1/24

local dx = 1
local dy = 1
local x = 10
local y = 10

curses.initscr()

local width = COLS
local height = LINES

while true do
	curses.erase()

	x = x + dx
	y = y + dy

	if y == height then dy = -dy
	elseif y == 0 then dy = math.abs(dy)
	elseif x == width then dx = -dx
	elseif x == 0 then dx = math.abs(dx) end

	curses.mvaddch(y, x, char)

	curses.refresh()

	blua.sleep(frametime)
end

curses.endwin()
