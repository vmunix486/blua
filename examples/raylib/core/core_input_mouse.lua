local screenWidth = 800
local screenHeight = 450

raylib.InitWindow(screenWidth, screenHeight, "raylib [core] example - input mouse - Lua")

local ballPositionX = -100
local ballPositionY = -100

local ballColor = DARKBLUE

raylib.SetTargetFPS(60)

while not raylib.WindowShouldClose() do
	if raylib.IsKeyPressed(KEY_H) then
		if raylib.IsCursorHidden() then raylib.ShowCursor()
		else raylib.HideCursor() end
	end

	local ballPositionX = raylib.GetMouseX()
	local ballPositionY = raylib.GetMouseY()

	-- Uncomment for debug output
	--print("X: " .. ballPositionX .. " Y: " .. ballPositionY)

	if raylib.IsMouseButtonPressed(MOUSE_BUTTON_LEFT) then ballColor = MAROON
	elseif raylib.IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) then ballColor = LIME
	elseif raylib.IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) then ballColor = DARKBLUE
	elseif raylib.IsMouseButtonPressed(MOUSE_BUTTON_SIDE) then ballColor = PURPLE
	elseif raylib.IsMouseButtonPressed(MOUSE_BUTTON_EXTRA) then ballColor = YELLOW
	elseif raylib.IsMouseButtonPressed(MOUSE_BUTTON_FORWARD) then ballColor = ORANGE
	elseif raylib.IsMouseButtonPressed(MOUSE_BUTTON_BACK) then ballColor = BEIGE end

	raylib.BeginDrawing()
		raylib.ClearBackground(RAYWHITE)
		raylib.DrawCircle(ballPositionX, ballPositionY, 40, ballColor)
		raylib.DrawText("move ball with mouse and clock mouse button to change color", 10, 10, 20, DARKGRAY)
		raylib.DrawText("Press 'H' to toggle cursor visibility", 10, 30, 20, DARKGRAY)

		if raylib.IsCursorHidden() then raylib.DrawText("CURSOR HIDDEN", 20, 60, 20, RED)
		else raylib.DrawText("CURSOR VISIBLE", 20, 60, 20, LIME) end
	raylib.EndDrawing()
end

raylib.CloseWindow()
