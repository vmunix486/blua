--[[ 

vmunix note:

This took me a good hour to make by hand, adding and removing things as I went on.
one of the hardest parts to figure out was why DrawText was complaining about integers.
By default, every number in Lua is a float. Meaning if you divide 12 by 5, you will get 2.4.
DrawText() does not take a float. It takes an integer. It took me so long to figure that out.
I looked up how you turn a float into an integer in Lua, and I got math.tointeger().

That just did not work. 

I did a little more digging, and then found that math.floor() turns it into an integer.
As I kept reading, I saw that to forward slashes does the equivelant of math.floor(12/5)
automatically.

Here you go. Something that I'd like to have is fullscreen setting support, but I could not
figure out why it would not work. If you wanna try, go for it.

--]]

local config = ini.parse("rayini.ini") -- This inits the parsing.

local screenWidth = config.width
local resizedscreenWidth = screenWidth
local screenHeight = config.height
local resizedscreenHeight = screenHeight

raylib.SetConfigFlags(FLAG_WINDOW_RESIZABLE)

raylib.InitWindow(screenWidth, screenHeight, "BLua example - INI parser + Raylib")

while not raylib.WindowShouldClose() do
	raylib.BeginDrawing()
		raylib.ClearBackground(RAYWHITE)
		
		-- This changes the width and height of things everytime the windows resized
	    	if raylib.IsWindowResized() == true then
			resizedscreenWidth = raylib.GetScreenWidth()
			resizedscreenHeight = raylib.GetScreenHeight()
			-- Debug
			--print("Width: " .. resizedscreenWidth)
			--print("Height: " .. resizedscreenHeight)
		end 

		raylib.DrawText("Width: " .. resizedscreenWidth, resizedscreenWidth//2, resizedscreenHeight//2-10, 20, GRAY)

		raylib.DrawText("Height: " .. resizedscreenHeight, resizedscreenWidth//2, resizedscreenHeight//2+10, 20, GRAY)

		raylib.DrawText("Press (S) to save!", resizedscreenWidth//2, resizedscreenHeight//2+40, 20, GRAY)
	
		-- If S is pressed, then save
		if raylib.IsKeyPressed(KEY_S) == true then
			local settings = io.open("rayini.ini", "w")
			io.output(settings)

			io.write("width = " .. resizedscreenWidth .. "\n")
			io.write("height = " .. resizedscreenHeight .. "\n")

			io.close(settings)
			print("Saved!")
		end

	raylib.EndDrawing()
end

raylib.CloseWindow()
