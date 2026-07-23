local screenWidth = 800
local screenHeight = 600
local px = 1
local py = 1

raylib.InitWindow(screenWidth, screenHeight, "BLua - Static")

raylib.SetTargetFPS(60)

while not raylib.WindowShouldClose() do
	raylib.BeginDrawing()

		for py = 1, screenHeight do
			for px = 1, screenWidth do
				local pc = math.random(0, 255)
				raylib.DrawPixel(px, py, { pc, pc, pc, 255 })
			end
		end

		raylib.DrawFPS(10, 10)

	raylib.EndDrawing()
end

raylib.CloseWindow()
