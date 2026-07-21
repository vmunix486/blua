-- A simple bouncer program

local function main()
	screenWidth = 800
	screenHeight = 600

	raylib.InitWindow(screenWidth, screenHeight, "raybouncer")
	raylib.SetTargetFPS(60)

	pos = { 100, 100 }
	speed = { 1, 1 }
	size = { 20, 20 }

	raylib.BeginDrawing()
	raylib.ClearBackground(RAYWHITE)

	while not raylib.WindowShouldClose() do

		--pos[1] = pos[1] + speed[1]
		--pos[2] = pos[2] + speed[2]

		--raylib.DrawRectangle(pos[1], pos[2], size[1], size[2], RED)
	end
end

main()
