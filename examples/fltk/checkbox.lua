local window = fltk.Fl_Window(400, 300, "BLua - FLTK - Check Box")

local checkbutton = fltk.Fl_Check_Button(200, 75, 75, 30, "Check")
local box = fltk.Fl_Box(200, 10, 50, 10, "Check the box")
local box2 = fltk.Fl_Box(200, 125, 50, 10, "")

checkbutton:callback(function()
--	print(checkbutton:value())
	if checkbutton:value() == true then
		box2:label("Button is marked.")
		print("Button is marked.")
	else
		box2:label("Button is unmarked.")
		print("Button is unmarked.")
	end
	fltk.redraw()
end)

window:finish()
window:show()
fltk.run()
