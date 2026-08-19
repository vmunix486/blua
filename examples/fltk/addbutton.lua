local num = 0

local win = fltk.Fl_Window(400, 300, "BLua - FLTK - Add Button")

local btn = fltk.Fl_Button(150, 120, 100, 40, "Click Me!")
btn:callback(function()
	num = num + 1
	btn:label(num)
	print("Button clicked! " .. num)
end)

win:finish()
win:show()
fltk.run()
