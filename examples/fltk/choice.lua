local names = 5
local choicenames = { -- The names come from the band Phish
	"Trey Anastasio",
	"Mike Gordon",
	"Page McConnell",
	"Jon Fishman",
	"Marco Esquandolas"
}

local window = fltk.Fl_Window(400, 300, "BLua - FLTK - Choice")

local choice = fltk.Fl_Choice(200, 15, 150, 30)
local button = fltk.Fl_Button(100, 15, 75, 40, "Enter")
local box = fltk.Fl_Box(75, 100, 100, 50, "Choose a name")

for i = 1, names do
	choice:add(choicenames[i])
end

button:callback(function()
	print(choice:text())
	box:label(choice:text() .. " Selected!") -- Yes, Lua string concatenation works EVERYWHERE! LUA ROCKS!!!
	fltk.redraw()
end)

window:finish()
window:show()
fltk.run()
