-- Showing off of the slider
local window = fltk.Fl_Window(400, 300, "BLua - FLTK - Slider")

local slider = fltk.Fl_Slider(10, 10, 30, 200)
local button = fltk.Fl_Button(150, 120, 100, 40, "Enter")
local box = fltk.Fl_Box(200, 20, 30, 30, "Hit Enter to start!")

-- So it gives back sane values.
-- By default it returns a float from 0 to 1.
-- By float, I mean float. Uses like 16 digits for the decimal
-- by default. We do not need allat.
slider:bounds(100, 0)
slider:step(1)

button:callback(function()
	print(slider:value())
	box:label(slider:value())
	fltk.redraw() -- The text on the box gets messed up without this
end)

window:finish()
window:show()
fltk.run()
