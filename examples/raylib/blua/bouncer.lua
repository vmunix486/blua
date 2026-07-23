-- A simple bouncer program in Lua

local screenWidth = 800
local screenHeight = 600

raylib.InitWindow(screenWidth, screenHeight, "luabouncer")
raylib.SetTargetFPS(60)

local posx = 100
local posy = 100

local speedx = 1
local speedy = 1

local size = 20

while not raylib.WindowShouldClose() do
	raylib.BeginDrawing()
		raylib.ClearBackground(BLACK)

		posx = posx + speedx
		posy = posy + speedy

		raylib.DrawRectangle(posx, posy, size, size, RAYWHITE)

		if posy > screenHeight - size then
			speedy = -1
		end

		if posx > screenWidth - size then
			speedx = -1
		end

		if posy < 0 then
			speedy = 1
		end

		if posx < 0 then
			speedx = 1
		end

	raylib.EndDrawing()
end

raylib.CloseWindow()
