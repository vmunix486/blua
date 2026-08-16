-- This is like print but without the newline at the end.
io.write("How many seconds do you want to sleep for? ")

-- This gets the input from the user after the text.
local secs = io.read()

-- This checks if it is actually a number
if tonumber(secs) == nil then
	print("Not a number.")
	os.exit(1)
end

-- Use the new function to do what Meiling does (elite ball Touhou reference)
blua.sleep(secs)
