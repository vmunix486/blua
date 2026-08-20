local namesnum = 10
local names = { -- Some of my favorite Touhou characters
	"Reimu Hakurei",
	"Aunn Komano",
	"Rumia",
	"Hong Meiling",
	"Rinnosuke Morichika",
	"Fujiwara no Mokou",
	"Suika Ibuki",
	"Kogasa Tatara",
	"Yuuka Kazami",
	"Yuyuko Saigyoiji"
}

local window = fltk.Fl_Window(400, 300, "BLua - FLTK - Scrollable List")

local list = fltk.Fl_Hold_Browser(10, 10, 200, 150)
local button = fltk.Fl_Button(250, 10, 50, 40, "Enter")
local box = fltk.Fl_Box(250, 200, 50, 40, "Select a name")

for i = 1, namesnum do
	list:add(names[i])
end

button:callback(function()
	print(names[list:value()]) -- list:value() returns the index number that's selected,
				   -- So I put the list:value() into the names table, which
				   -- returns the correct name.
	box:label(names[list:value()] .. " Selected!")
	fltk.redraw()
end)

window:finish()
window:show()
fltk.run()
