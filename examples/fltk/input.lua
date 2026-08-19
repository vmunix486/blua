local window = fltk.Fl_Window(400, 300, "BLua - FLTK - Input Box")

local input = fltk.Fl_Input(10, 10, 200, 30) -- The text box
local button = fltk.Fl_Button(100, 50, 100, 40, "Enter") -- The button that accepts the text box
button:callback(function()
	print(input:value()) -- Prints whatever's inside of the textbox
end)

window:finish()
window:show()
fltk.run()
