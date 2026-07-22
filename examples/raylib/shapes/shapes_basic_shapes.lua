local screenWidth = 800
local screenHeight = 450

raylib.InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes - Lua")

rotation = 0

raylib.SetTargetFPS(60)

while not raylib.WindowShouldClose() do
	rotation = rotation + 0.2

	raylib.BeginDrawing()
		raylib.ClearBackground(RAYWHITE)

		raylib.DrawText("some basic shapes available on raylib", 20, 20, 20, DARKGRAY)

		raylib.DrawCircle(screenWidth/5, 120, 35, DARKBLUE)
		raylib.DrawCircleGradient({ screenWidth/5, 220 }, 60, GREEN, SKYBLUE)
		raylib.DrawCircleLines(screenWidth/5, 340, 80, DARKBLUE)
		raylib.DrawEllipse(screenWidth/5, 120, 25, 20, YELLOW)
		raylib.DrawEllipseLines(screenWidth/5, 120, 30, 25, YELLOW)

		raylib.DrawRectangle(screenWidth/4*2 - 60, 100, 120, 60, RED)
		raylib.DrawRectangleGradientH(screenWidth/4*2 - 90, 170, 180, 130, MAROON, GOLD)
		raylib.DrawRectangleLines(screenWidth/4*2 - 40, 320, 80, 60, ORANGE)

		raylib.DrawTriangle({ screenWidth / 4 * 3, 80 }, { screenWidth / 4 * 3 - 60, 150 }, { screenWidth / 4 * 3 + 60, 150 }, VIOLET)
		raylib.DrawTriangleLines({ screenWidth / 4 * 3, 160 }, { screenWidth / 4 * 3 - 20, 230 }, { screenWidth / 4 * 3 + 20, 230 }, DARKBLUE)

		raylib.DrawPoly({ screenWidth / 4 * 3, 330 }, 6, 80, rotation, BROWN)
		raylib.DrawPolyLines({ screenWidth / 4 * 3, 330 }, 6, 90, rotation, BROWN)
		raylib.DrawPolyLinesEx({ screenWidth / 4 * 3, 330 }, 6, 85, rotation, 6, BEIGE)

		raylib.DrawLine(18, 42, screenWidth - 18, 42, BLACK);
	raylib.EndDrawing()
end

raylib.CloseWindow()
