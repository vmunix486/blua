local screenWidth = 800
local screenHeight = 450

raylib.InitWindow(screenWidth, screenHeight, "raylib [core] example - input keys - Lua")

local ballPositionX = screenWidth/2
local ballPositionY = screenHeight/2

raylib.SetTargetFPS(60)

while not raylib.WindowShouldClose() do
	if raylib.IsKeyDown(KEY_RIGHT) then ballPositionX = ballPositionX + 2 end
	if raylib.IsKeyDown(KEY_LEFT) then ballPositionX = ballPositionX - 2 end
	if raylib.IsKeyDown(KEY_UP) then ballPositionY = ballPositionY - 2 end
	if raylib.IsKeyDown(KEY_DOWN) then ballPositionY = ballPositionY + 2 end

	raylib.BeginDrawing()
		raylib.ClearBackground(RAYWHITE)
		raylib.DrawText("move the ball with the arrow keys", 10, 10, 20, DARKGRAY)
		raylib.DrawCircle(ballPositionX, ballPositionY, 50, MAROON)
	raylib.EndDrawing()
end

raylib.CloseWindow()

