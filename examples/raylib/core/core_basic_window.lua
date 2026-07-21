local screenWidth = 800 -- How many pixels wide the window is
local screenHeight = 450 -- How many pixels tall the window is

raylib.InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window - Lua") 
-- Creates the window with the specified hight, width, and title name

raylib.SetTargetFPS(60) -- Sets it to target 60 FPS (frames per second)

while not raylib.WindowShouldClose() do -- When WindowShouldClose() = 1 then exit this loop
	raylib.BeginDrawing() -- Start drawing to the buffer
		raylib.ClearBackground(RAYWHITE) -- Clear the screen with this color
		raylib.DrawText("Congrats! You created for first window!", 190, 200, 20, LIGHTGRAY)
		-- Draw text with the string starting at the specified pixels with the specified font size, and with the color light gray
	raylib.EndDrawing() -- Stop Drawing to the buffer
end

raylib.CloseWindow() -- Unload everything and close the window gracefully
