-- Rewrite of aclock by tenox7 in BLua
local pi = math.pi
local fontratio = 2

local function draw_circle(hand_max, sYcen, sXcen)
	for r = 0, 59 do
		local x = math.cos(r * pi / 180 * 6) * hand_max * fontratio + sXcen
		local y = math.sin(r * pi / 180 * 6) * hand_max + sYcen

		local c
		if r == 0 or r == 5 or r == 10 or r == 15 or r == 20 or r == 25 or
   		r == 30 or r == 35 or r == 40 or r == 45 or r == 50 or r == 55 then
			c = string.byte('o')
		else
			c = string.byte('.')
		end
		curses.mvaddch(math.floor(y), math.floor(x), c)
	end
end

local function draw_hand(minute, hlength, c, sXcen, sYcen)
	local r = (minute - 15) * (pi / 180) * 6

	for n = 1, hlength - 1 do
		local x = math.cos(r) * n * fontratio + sXcen
		local y = math.sin(r) * n + sYcen
		curses.mvaddch(math.floor(y), math.floor(x), string.byte(c))
	end
end

curses.initscr()

while true do
	local time = os.date("*t", os.time())

	local sXmax = COLS
	local sYmax = LINES

	if (sXmax / fontratio <= sYmax) then
		smax = sXmax / fontratio
	else
		smax = sYmax
	end

	local hand_max = (smax / 2) - 1

	local sXcen = sXmax / 2
	local sYcen = sYmax / 2

	curses.erase()
	draw_circle(hand_max, sYcen, sXcen)

	draw_hand((time.hour * 5) + math.floor(time.min / 10), 2 * hand_max / 3, 'h', sXcen, sYcen)
	draw_hand(time.min, hand_max - 2, 'm', sXcen, sYcen)
	draw_hand(time.sec, hand_max - 1, '.', sXcen, sYcen)

	curses.mvaddstr(math.floor(sYcen - (3 * hand_max / 5)), math.floor(sXcen - 5), ".:LCLOCK:.")
	curses.mvprintw(math.floor(sYcen + (3 * hand_max / 5)), math.floor(sXcen - 5), string.format("[%02d:%02d:%02d]", time.hour, time.min, time.sec))

	curses.refresh()
	os.execute("sleep 1")
end

curses.endwin()
