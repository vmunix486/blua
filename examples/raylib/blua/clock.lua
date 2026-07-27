-- This is a copy of https://github.com/Amir-Merchad/C-Analog_Clock
-- Rewritten for BLua by vmunix

local width = 800
local height = 600

local radius = height/2-100
local centerX = width/2
local centerY = height/2

local hourHandLength = radius * 0.45
local minuteHandLength = radius * 0.75
local secondHandLength = radius * 0.85

local function DrawClock()
	raylib.DrawCircle(centerX, centerY, radius, RAYWHITE)
	for i = 0, 59 do
		local angle = i * 2 * math.pi / 60 - math.pi / 2

		local xInner = centerX + radius * 0.92 * math.cos(angle)
		local yInner = centerY + radius * 0.92 * math.sin(angle)

		local xInnerExt = centerX + radius * 0.88 * math.cos(angle)
                local yInnerExt = centerY + radius * 0.88 * math.sin(angle)

		local xOuter = centerX + radius * math.cos(angle)
		local yOuter = centerY + radius * math.sin(angle)

		if i % 5 == 0 then
			raylib.DrawLine(math.floor(xInnerExt), math.floor(yInnerExt), math.floor(xOuter), math.floor(yOuter), BLACK)
		else
			raylib.DrawLine(math.floor(xInner), math.floor(yInner), math.floor(xOuter), math.floor(yOuter), GRAY)
		end
	end
end

local function DrawHand(angle, length, color)
	local endX = centerX + length * math.cos(angle)
	local endY = centerY + length * math.sin(angle)
	raylib.DrawLine(math.floor(centerX), math.floor(centerY), math.floor(endX), math.floor(endY), color)
end

raylib.InitWindow(width, height, "Clock")
raylib.SetTargetFPS(60)

while not raylib.WindowShouldClose() do
	local time = os.date("*t", os.time())

	local hourAngle = time.hour % 12 + time.min / 60 * 2 * math.pi / 12 - math.pi / 2
	local minuteAngle = time.min + time.sec / 60 * 2 * math.pi / 60 - math.pi / 2
	local secondAngle = time.sec * 2 * math.pi / 60 - math.pi / 2

	raylib.BeginDrawing()
		raylib.ClearBackground(BLACK)

		DrawClock()
		DrawHand(hourAngle, hourHandLength, BLACK)
		DrawHand(minuteAngle, minuteHandLength, BLACK)
		DrawHand(secondAngle, secondHandLength, RED)
	raylib.EndDrawing()
end
raylib.CloseWindow()
