curses.initscr()

curses.mvprintw(LINES//2, (COLS//2)-6, "Type a key!")
curses.refresh()

while true do
	local char = curses.getch()

	if char ~= nil then
		curses.endwin()
		print(char .. " was pressed!")
		os.exit(0)
	end
end
