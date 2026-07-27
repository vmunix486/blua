-- Originally written for raylib in C
-- Rewritten into BLua by vmunix486

local screenWidth = 800
local screenHeight = 600

raylib.SetConfigFlags(FLAG_WINDOW_RESIZABLE)
raylib.InitWindow(screenWidth, screenHeight, "raylib [shaders] example - raymarching rendering - BLua")

local camera = {
	{2.5, 2.5, 3.0},   -- position
	{0.0, 0.0, 0.7},   -- target
	{0.0, 1.0, 0.0},   -- up
	65,                 -- fovy
	CAMERA_PERSPECTIVE  -- projection
}

local shader = raylib.LoadShader(nil, "resources/raymarching.fs")

local viewEyeLoc = raylib.GetShaderLocation(shader, "viewEye")
local viewCenterLoc = raylib.GetShaderLocation(shader, "viewCenter")
local runTimeLoc = raylib.GetShaderLocation(shader, "rumTime")
local resolutionLoc = raylib.GetShaderLocation(shader, "resolution")

local resolution = { screenWidth, screenHeight }
raylib.SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2)

local runTime = 0

raylib.DisableCursor()
raylib.SetTargetFPS(60)

while not raylib.WindowShouldClose() do
	raylib.UpdateCamera(camera, CAMERA_FIRST_PERSON)

	local cameraPos = { camera[1][1], camera[1][2], camera[1][3] }
	local cameraTarget = { camera[2][1], camera[2][2], camera[2][3] }

	local deltaTime = raylib.GetFrameTime()
	runTime = runTime + deltaTime

	raylib.SetShaderValue(shader, viewEyeLoc, cameraPos, SHADER_UNIFORM_VEC3)
	raylib.SetShaderValue(shader, viewCenterLoc, cameraTarget, SHADER_UNIFORM_VEC3)
	raylib.SetShaderValue(shader, runTimeLoc, {runTime}, SHADER_UNIFORM_FLOAT)

	if raylib.IsWindowResized() then
		resolution[1] = raylib.GetScreenWidth()
		resolution[2] = raylib.GetScreenHeight()
		raylib.SetShaderValue(shader, resolutionLoc, resolution, SHADER_UNIFORM_VEC2)
	end

	raylib.BeginDrawing()
		raylib.ClearBackground(RAYWHITE)
		raylib.BeginShaderMode(shader)
			raylib.DrawRectangle(0, 0, raylib.GetScreenWidth(), raylib.GetScreenHeight(), WHITE)
		raylib.EndShaderMode()

		raylib.DrawText("(c) Raymarching shader by Iñigo Quilez. MIT License. Rewritten into BLua by vmunix.", raylib.GetScreenWidth() - 280, raylib.GetScreenHeight() - 20, 10, BLACK)
	raylib.EndDrawing()
end

raylib.UnloadShader(shader)
raylib.CloseWindow()
