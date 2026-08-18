local win = fltk.Fl_Window(400, 300, "BLua - FLTK - Button")

local btn = fltk.Fl_Button(150, 120, 100, 40, "Click me")
btn:callback(function()
    print("Button clicked!")
end)

win:finish()
win:show()
fltk.run()
