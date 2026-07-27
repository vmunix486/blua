local fltk = fltk

local window = fltk.Fl_Window(300, 180, "FLTK Hello")
local box = fltk.Fl_Box(20, 40, 260, 100, "Hello, World!")
box:box(FL_UP_BOX)
box:labelsize(36)
box:labelfont(FL_BOLD + FL_ITALIC)
box:labeltype(FL_SHADOW_LABEL)
window:finish()
window:show()

return fltk.run()
