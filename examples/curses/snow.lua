-- Little example that emulates snow

local frametime = 1/4 -- Frametime
local particles = 100 -- The amount of particles
local character = "*" -- The character to use

curses.initscr()
curses.curs_set(0)
curses.noecho()
curses.keypad(true)

local width = COLS
local height = LINES

while true do
	curses.erase()

	for i = 1, particles do
		local x = math.random(0, width - 1)
		local y = math.random(0, height - 1)
		curses.move(y, x)
		curses.addch(string.byte(character))
	end

	curses.refresh()
	blua.sleep(frametime)
end

curses.endwin()
