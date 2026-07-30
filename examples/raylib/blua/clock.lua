-- This is a copy of https://github.com/Amir-Merchad/C-Analog_Clock
-- Rewritten for BLua by vmunix

local width = 800 -- Width in pixels of the window
local height = 600 -- Ditto, but with height

local radius = height/2 -- Radius
local centerX = width/2 -- The center on the X axis
local centerY = height/2 -- The center on the Y axis

local hourHandLength = radius * 0.45 -- How long the hour hand is
local minuteHandLength = radius * 0.75 -- How long the minute hand is
local secondHandLength = radius * 0.85 -- How long the second hand is

-- Setting background colors
local background = BLACK	-- The color of the background behind the actual circle of the clock (default: BLACK)
local clockbackground = RAYWHITE-- The color of the background of the clock itself (default: RAYWHITE)
local tickcolor = BLACK		-- The color of the normal ticks on the clock (default: BLACK)
local smalltickcolor = GRAY	-- The color of the small ticks on the clock (default: GRAY)
local hourhandcolor = BLACK	-- The color of the hour hand on the clock (default: BLACK)
local minutehandcolor = BLACK	-- The color of the minute hand on the clock (default: BLACK)
local secondhandcolor = RED	-- The color of the second hand on the clock (default: RED)
local textcolor = BLUE		-- The color of the text that says the time (default: BLUE)

local function DrawClock()
	raylib.DrawCircle(centerX, centerY, radius, clockbackground) -- Draws the clock background

	for i = 0, 59 do
		-- Math stuff
		local angle = i * 2 * math.pi / 60 - math.pi / 2

		local xInner = centerX + radius * 0.92 * math.cos(angle)
		local yInner = centerY + radius * 0.92 * math.sin(angle)

		local xInnerExt = centerX + radius * 0.88 * math.cos(angle)
                local yInnerExt = centerY + radius * 0.88 * math.sin(angle)

		local xOuter = centerX + radius * math.cos(angle)
		local yOuter = centerY + radius * math.sin(angle)

		-- Drawing the ticks on the clock
		if i % 5 == 0 then raylib.DrawLineEx( { xInnerExt, yInnerExt }, { xOuter, yOuter }, 5, tickcolor)
		else raylib.DrawLineEx( { xInner, yInner }, { xOuter, yOuter }, 3, smalltickcolor) end
	end
end

local function DrawHand(angle, length, color)
	local endX = math.floor(centerX + length * math.cos(angle))
	local endY = math.floor(centerY + length * math.sin(angle))
	raylib.DrawLineEx({ centerX, centerY }, { endX, endY }, 3, color)
end

raylib.InitWindow(width, height, "Clock")
raylib.SetTargetFPS(60)

while not raylib.WindowShouldClose() do
	local time = os.date("*t", os.time()) -- Get the current time

	-- Make the angle of the hour hand
	local hourAngle = (time.hour % 12 + time.min / 60) * (2 * math.pi / 12) - math.pi / 2
	-- Make the angle of the minute hand
	local minuteAngle = (time.min + time.sec / 60) * (2 * math.pi / 60) - math.pi / 2
	-- Make the angle of the second hand
	local secondAngle = time.sec * (2 * math.pi / 60) - math.pi / 2

	raylib.BeginDrawing()
		raylib.ClearBackground(background)

		DrawClock() -- Draw the clock from the DrawClock() function before

		-- Make the time look better for the text
		if time.hour > 12 then pm = true else pm = false end -- If hours is over 12, then it is PM (by default time.hour is 24 hour)
		-- Add a leading 0 to single digit numbers
		if time.hour < 10 then betterhours = 0 .. time.hour else betterhours = time.hour end
		if time.min < 10 then bettermins = 0 .. time.min else bettermins = time.min end
		if time.sec < 10 then bettersecs = 0 .. time.sec else bettersecs = time.sec end

		-- Draw the time
		if pm == true then
			raylib.DrawText(betterhours - 12 --[[This is subtracted by 12 so it can get the time in PM.]] .. ":" .. bettermins .. ":" .. bettersecs .. " PM",
					width//2 - 30, height//2 + 100, 20, textcolor)
		elseif pm == false then
			raylib.DrawText(betterhours .. ":" .. bettermins .. ":" .. bettersecs .. " AM",
					width//2 - 30, height//2 + 100, 20, textcolor)
		else
			-- Oops! If it's not either than throw an error for the time text.
			raylib.DrawText("err", width//2 - 30, height//2 + 100, 20, RED)
		end

		-- Draw the hands of the clock
		DrawHand(hourAngle, hourHandLength, hourhandcolor)
		DrawHand(minuteAngle, minuteHandLength, minutehandcolor)
		DrawHand(secondAngle, secondHandLength, secondhandcolor)

	raylib.EndDrawing()
end
raylib.CloseWindow()
